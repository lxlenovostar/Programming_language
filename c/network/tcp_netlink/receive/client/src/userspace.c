#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PAYLOAD 128  /* maximum payload size*/
#define NETLINK_USER 31

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *send_nlh = NULL;
struct nlmsghdr *rece_nlh = NULL;
struct msghdr send_msg;
struct msghdr rece_msg;
struct iovec send_iov;
struct iovec rece_iov;
int sock_fd;

int init_sock() 
{
	int res = 0;

	sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
	if (sock_fd < 0)
		return -1;

 	memset(&src_addr, 0, sizeof(src_addr));
 	src_addr.nl_family = AF_NETLINK;
 	src_addr.nl_pid = getpid();  /* self pid */
 	src_addr.nl_groups = 0;  /* not in mcast groups */

 	bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

 	memset(&dest_addr, 0, sizeof(dest_addr));
 	dest_addr.nl_family = AF_NETLINK;
 	dest_addr.nl_pid = 0;   /* For Linux Kernel */
 	dest_addr.nl_groups = 0; /* unicast */

	return res;
}

void free_send_msg() 
{
	free(send_nlh);
}

int set_send_msg() 
{
 	send_nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	/* Fill the netlink message header */
 	send_nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
 	send_nlh->nlmsg_pid = getpid();  /* self pid */
 	send_nlh->nlmsg_flags = 0;

 	/* Fill in the netlink message payload */
 	strcpy(NLMSG_DATA(send_nlh), "give your present!");

	/* set struct iovec iov */
 	send_iov.iov_base = (void *)send_nlh;		/* starting address of buffer */
 	send_iov.iov_len = send_nlh->nlmsg_len;	    /* size of buffer */

 	send_msg.msg_name = (void *)&dest_addr;	/* protocol address */
 	send_msg.msg_namelen = sizeof(dest_addr); /* size of protocol address */
 	send_msg.msg_iov = &send_iov; /* scatter/gather array */
 	send_msg.msg_iovlen = 1;	/* elements in msg_iov */

	return 0;
}

void free_rece_msg() 
{
	free(rece_nlh);
}

int set_rece_msg() 
{
 	rece_nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	/* Fill the netlink message header */
 	rece_nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);

	/* set struct iovec iov */
 	rece_iov.iov_base = (void *)rece_nlh;		/* starting address of buffer */
 	rece_iov.iov_len = rece_nlh->nlmsg_len;	    /* size of buffer */

 	rece_msg.msg_name = (void *)&dest_addr;	/* protocol address */
 	rece_msg.msg_namelen = sizeof(dest_addr); /* size of protocol address */
 	rece_msg.msg_iov = &rece_iov; /* scatter/gather array */
 	rece_msg.msg_iovlen = 1;	/* elements in msg_iov */
	
	return 0;
}

void send_to_kernel() 
{
	printf("Sending message to kernel\n");
 	sendmsg(sock_fd, &send_msg, 0);
	printf("Waiting for message from kernel\n");
}

int check_from_kernel() 
{
    const char *msg = "get your pid";

 	/* Read message from kernel */
 	memset(rece_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
 	recvmsg(sock_fd, &rece_msg, 0);
 	printf("Received message payload:%s\n", (char *)NLMSG_DATA(rece_nlh));

	if (strcmp(msg, (char *)NLMSG_DATA(rece_nlh)) == 0) 
		return 0;
	else 
		return 1;
}

int check_netlink_status() 
{
	int res = 0;
	int count = 0;

restart:
	send_to_kernel();

	res = check_from_kernel();
	if (res == 0)
		return 0;
	else {
		count++;
		
		if (count < 3) {
			sleep(10);
			goto restart;
		}
		else 
			return 1;
	}
}

void free_resource() 
{
	free_send_msg();
	free_rece_msg();
	
 	/* Close Netlink Socket */
    close(sock_fd);
}

int main() {
	int res = 0;

	init_sock();

	set_send_msg();
	set_rece_msg();

	res = check_netlink_status();  
	if (res != 0)
		printf("kernel module don't insmod. Please insmod it.\n");
	else 
		printf("kernel module insmod succcess.\n");

	return 0;
}