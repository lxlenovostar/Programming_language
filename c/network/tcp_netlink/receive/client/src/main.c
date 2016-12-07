#include	"../lib/lx_sock.h"
#include	"../lib/error.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void *
thread_comm_kernel(void *arg)
{
	int end = 1000;
	int i;
	for(i = 0; i < end; i++) {
		sleep(1);
	}
    return NULL;
}

static void *
thread_comm_server(void *arg)
{
    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t kernel_tid;
    pthread_t server_tid;
	int res;

	res = pthread_create(&kernel_tid, NULL, thread_comm_kernel, NULL);
	if (res != 0) {
		//err_quit("Thread creation failed");
		printf("Thread creation failed\n");
		exit(EXIT_FAILURE);
	}

	res = pthread_join(kernel_tid, NULL);
	if (res != 0) {
		//err_quit("Thread join failed");
		printf("Thread join failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Process will end\n");
    exit(0);
}
