#include    <stdio.h>       /* for convenience */
#include    <stdlib.h>      /* for convenience */
#include    <string.h>      /* for convenience */
#include    <unistd.h>      /* for convenience */
#include	"../lib/error.h"
#include	"../lib/lx_netlink.h"

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


static void * 
thread_comm_kernel(void *arg) {
	int res;

	res = ping_pong_kernel();
	
	if (res == 0) {
		/* kernel module install success. */
		free_send_msg();
		rece_from_kernel();

		/*
         TODO 涉及线程互斥  
         */
	}

	free_rece_msg(); 
    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t kernel_tid;
	int res;

	res = pthread_create(&kernel_tid, NULL, thread_comm_kernel, NULL);
	if (res != 0) {
		err_quit("Thread creation failed");
	}

	//TODO 注册消息处理函数处理15信号，用于关闭进程。

	res = pthread_join(kernel_tid, NULL);
	if (res != 0) {
		err_quit("Thread join failed");
	}

	printf("Process will end\n");
    exit(0);
}
