#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#ifndef WIN32
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/thread.h>
#include <pthread.h>

#ifndef USE_X_PRINT
#define aPrt printf
#define dPrt printf
#define ePrt printf
#define bgPrt printf
#define edPrt printf
#else 
#include "hx_debug.h"
#endif
 
static int flag = 0;
static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 9995;

static void listener_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);
static void conn_writecb(struct bufferevent *, void *);
static void conn_readcb(struct bufferevent *, void *);
static void conn_eventcb(struct bufferevent *, short, void *);
static void signal_cb(evutil_socket_t, short, void *);

// gcc test_libevent_pthread.c -o libevent_pthread  -levent_core -lpthread -levent_pthreads

int 
main(int argc, char **argv) 
{
	int i = 0;
	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;	
	struct sockaddr_in sin;

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");		
		return 1;
	}	

	evthread_use_pthreads();

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	listener = evconnlistener_new_bind(base, listener_cb, (void *)base, 
					LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE|LEV_OPT_THREADSAFE, -1,
					(struct sockaddr*)&sin, sizeof(sin));
	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);
	if (!signal_event || event_add(signal_event, NULL) < 0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}

	char **a = event_get_supported_methods();
	for (i = 0; a[i] != NULL; i++) {
		printf("[%s]\n", a[i]);
	}

	event_base_dispatch(base);
	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");
	return 0;
}

static void
listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = user_data;
	struct bufferevent *bev;
	printf("begin listener_cb\n");

	//创建基于套接字的bufferevent
	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!\n");
		event_base_loopbreak(base);
		return;
	}

	struct timeval tv_read = {10, 0}, tv_write = {10, 0};
	bufferevent_set_timeouts(bev, &tv_read, &tv_write);

	int ret = bufferevent_enable(bev, BEV_OPT_THREADSAFE);
	if (ret < 0) {
		printf("-------------------------------------\n");
	}

	printf("bev addr [%x]\n", (int)bev);
	char *userData = NULL;
	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, userData);

	bufferevent_disable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	printf("end listener_cb\n" );
}

void *testThreadFun(void *bev)
{
	bufferevent_lock(bev);
	printf("2s sleep at testThreadFun\n");
	sleep(2);
	printf("up at testThreadFun\n");
	bufferevent_unlock(bev);
}

static void 
conn_readcb(struct bufferevent *bev, void *user_data)
{
	printf("begin conn_readcb\n");
	printf("bev addr [%x]\n", (int)bev);

	struct evbuffer *input = bufferevent_get_input(bev);
	pthread_t testThread, testThread2;

	int ret = pthread_create(&testThread, NULL, testThreadFun, bev);
	if (-1 == ret) {
		printf("create thread error\n");
	}
	
	ret = pthread_create(&testThread2, NULL, testThreadFun, bev);
	if (-1 == ret) {
		printf("create thread error\n");
	}

	char data[100];
	int len = 0;

	printf("input len [%d]\n", evbuffer_get_length(input));
	len = bufferevent_read(bev, data, sizeof(data) - 1);

	printf("data[%d][%d][%s]\n", len, strlen(data), data);
	printf("input len [%d]\n", evbuffer_get_length(input));

	bufferevent_enable(bev, EV_WRITE);
	printf("will call bufferevent_write\n");

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));

	printf("end conn_readcb\n");
}

static void 
conn_writecb(struct bufferevent *bev, void *user_data)
{
	printf("begin conn_writecb \n");
	printf("bev addr [%x]\n", (int)bev);
	int len = 0;

	struct evbuffer *output = bufferevent_get_output(bev);
	len = evbuffer_get_length(output);
	//if (0 == len && flag ) {
	if (0 == len && flag) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}

	printf("end conn_writecb \n");
}


static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{
	printf( "begin conn_eventcb\n" );
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
	    strerror(errno));/*XXX win32*/
	}

	/* None of the other events can happen here, since we haven't enabled
  	 * timeouts */
	bufferevent_free(bev);
	printf( "end conn_eventcb\n" );
}

static void
signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 2, 0 };
				 
	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");
					 
	event_base_loopexit(base, &delay);
}








































