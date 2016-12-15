/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define SERV_PORT 9877 
#define MAX_LINE 16384

void read_cb(struct bufferevent *bev, void *ctx)
{
#define MAX_LINE1    256
    char line[MAX_LINE1+1];
    int n;
    evutil_socket_t fd = bufferevent_getfd(bev);

    while (n = bufferevent_read(bev, line, MAX_LINE1), n > 0) {
        line[n] = '\0';
        printf("fd=%u, read line: %s\n", fd, line);

        bufferevent_write(bev, line, n);
    }
}

void error_cb(struct bufferevent *bev, short event, void *arg)
{
	evutil_socket_t fd = bufferevent_getfd(bev);
    printf("fd = %u, ", fd);
    if (event & BEV_EVENT_TIMEOUT) {
        printf("Timed out\n"); //if bufferevent_set_timeouts() called
    }
    else if (event & BEV_EVENT_EOF) {
        printf("connection closed\n");
    }
    else if (event & BEV_EVENT_ERROR) {
        printf("some other error\n");
    }
    bufferevent_free(bev);
}

void write_cb(struct bufferevent *bev, void *ctx)
{
	char *send_msg = "hello world."; 
    bufferevent_write(bev, send_msg, strlen(send_msg));
}


void eventcb(struct bufferevent *bev, short events, void *ptr)
{
    if (events & BEV_EVENT_CONNECTED) {
         /* We're connected to 127.0.0.1:8080.   Ordinarily we'd do
            something here, like start reading or writing. */
		char *send_msg = "hello world."; 
    	bufferevent_write(bev, send_msg, strlen(send_msg));
    } else if (events & BEV_EVENT_ERROR) {
         /* An error occured while connecting. */
    }
}

void
run(void)
{
    struct sockaddr_in servaddr;
    struct event_base *base;
	struct bufferevent *bev;

    base = event_base_new();
    if (!base)
        return; 

	memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
	//inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */

	/*
     BEV_OPT_CLOSE_ON_FREE: When the bufferevent is freed, close the underlying transport. 
	 This will close an underlying socket, free an underlying bufferevent, etc.
     */
	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	/*
	 The bufferevent_setcb() function changes one or more of the callbacks of a bufferevent. 
	 The readcb, writecb, and eventcb functions are called (respectively) when enough data is 
	 read, when enough data is written, or when an event occurs.
     */
	bufferevent_setcb(bev, read_cb, write_cb, eventcb, NULL);

	if (bufferevent_socket_connect(bev, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
     	perror("connect");
        bufferevent_free(bev);
        return;
    }

    event_base_dispatch(base);
  	return; 
}

int
main(int c, char **v)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    run();

    printf("The End.");
    return 0;
}
