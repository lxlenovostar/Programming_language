#include    <stdio.h>       /* for convenience */
#include    <stdlib.h>      /* for convenience */
#include    <string.h>      /* for convenience */
#include    <unistd.h>      /* for convenience */
#include    <pthread.h>
#include	"../lib/error.h"
#include	"../lib/lx_netlink.h"
#include	"../lib/libevent_api.h"

int sock_fd;

int ping_pong_kernel(void) {
	int res = 0;

	init_sock();

	set_send_msg();
	set_rece_msg();

	res = check_netlink_status();  
	if (res != 0)
		printf("kernel module don't insmod. Please insmod it.\n");
	else 
		printf("kernel module insmod succcess.\n");

	return res;
} 

static void* thread_comm_kernel(void *arg) {
	int res;

	res = ping_pong_kernel();
	
	if (res == 0) {
		/* kernel module install success. */
		free_send_msg();
		rece_from_kernel();

		//TODO 使用select来实现对netlink的监控，从内核中收到的信息将打印出来。
		/* ok, we wait information from kernel. */
		int maxfdp1;
		fd_set rset;
		int n = 0;

		for (;;) {
			FD_SET(sock_fd, &rset);
			maxfdp1 = sock_fd + 1;

			
			Select(maxfdp1, &rset, NULL, NULL, NULL);
			
			/* socket is readable */
			if (FD_ISSET(sockfd, &rset)) {
				n = rece_from_kernel();
				if (n == 0) {
					err_quit("kernel maybe terminated prematurely.");
				} else if (n == -1) {
					err_quit("something error.");
				}
	
				debug_info();
			}

		}

		/*
         TODO 涉及线程互斥,通知有数据加入链表  
         */
	}

	free_rece_msg(); 
    return NULL;
}

static void* thread_comm_server(void *arg) 
{
	/* start socket by libevent. */
	run();

	return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t kernel_tid, socket_tid;
	int res;

	//TODO 注册消息处理函数处理15信号，用于关闭进程。
	//TODO 如何实现心跳 

	res = pthread_create(&kernel_tid, NULL, thread_comm_kernel, NULL);
	if (res != 0) {
		err_quit("netlink thread creation failed");
	}
	
	res = pthread_create(&socket_tid, NULL, thread_comm_server, NULL);
	if (res != 0) {
		err_quit("tcp socket thread creation failed");
	}

	res = pthread_join(kernel_tid, NULL);
	if (res != 0) {
		err_quit("Thread join failed");
	}

	res = pthread_join(socket_tid, NULL);
	if (res != 0) {
		err_quit("Thread join failed");
	}

	printf("Process will end\n");
    exit(0);
}
