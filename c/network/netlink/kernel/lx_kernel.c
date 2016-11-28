#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

/*
 struct netlink_skb_parms
 {
	struct ucred            creds;          // Skb credentials      
    __u32                   pid;
    __u32                   groups;
    __u32                   dst_pid;
    __u32                   dst_groups;
    kernel_cap_t            eff_cap;
 };
*/

struct sock *nl_sk = NULL;

static void nl_data_ready (struct sk_buff *skb)
{
	struct nlmsghdr *nlh = NULL;

	if(skb == NULL) {
		printk("skb is NULL \n");
		return ;
	}

	nlh = (struct nlmsghdr *)skb->data;
	printk(KERN_INFO "%s: received netlink message payload: %s\n", __FUNCTION__, (char *)NLMSG_DATA(nlh));
}

static void netlink_set(void)
{
	int err;
	struct sk_buff *skb = NULL;
	nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, 0, nl_data_ready, NULL, THIS_MODULE);
	//nl_sk = netlink_kernel_create(NETLINK_USER, nl_data_ready);

 	/* wait for message coming down from user-space */
 	skb = skb_recv_datagram(nl_sk, 0, 0, &err);
}

static int __init my_module_init(void)
{
	printk(KERN_INFO "Initializing Netlink Socket");
	netlink_set();
	return 0;
}

static void __exit my_module_exit(void)
{
	printk(KERN_INFO "Goodbye");
	sock_release(nl_sk->sk_socket);
}

module_init(my_module_init);
module_exit(my_module_exit);
MODULE_LICENSE("GPL");

/*
void nl_data_ready (struct sock *sk, int len)
{
	wake_up_interruptible(sk->sleep);
}

void netlink_test() {
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlh = NULL;
	int err;
	u32 pid;

	nl_sk = netlink_kernel_create(NETLINK_TEST, nl_data_ready);
 	// wait for message coming down from user-space 
 	skb = skb_recv_datagram(nl_sk, 0, 0, &err);

 	nlh = (struct nlmsghdr *)skb->data;
 	printk("%s: received netlink message payload:%s\n", __FUNCTION__, NLMSG_DATA(nlh));

 	pid = nlh->nlmsg_pid; // pid of sending process 
 	NETLINK_CB(skb).groups = 0; // not in mcast group //
 	NETLINK_CB(skb).pid = 0;      // from kernel 
 	NETLINK_CB(skb).dst_pid = pid;
 	NETLINK_CB(skb).dst_groups = 0;  // unicast 
 	netlink_unicast(nl_sk, skb, pid, MSG_DONTWAIT);
 	sock_release(nl_sk->socket);
}

*/
