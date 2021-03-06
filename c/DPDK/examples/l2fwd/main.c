/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2016 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * http://dpdk.org/doc/guides/sample_app_ug/l2_forward_job_stats.html
 *
 * TODO: 
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

static volatile bool force_quit;

/* MAC updating enabled by default */
static int mac_updating = 1;

#define RTE_LOGTYPE_L2FWD RTE_LOGTYPE_USER1

#define NB_MBUF   8192

#define MAX_PKT_BURST 32
#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */
#define MEMPOOL_CACHE_SIZE 256

/*
 * Configurable number of RX/TX ring descriptors
 */
#define RTE_TEST_RX_DESC_DEFAULT 128
#define RTE_TEST_TX_DESC_DEFAULT 512
static uint16_t nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
static uint16_t nb_txd = RTE_TEST_TX_DESC_DEFAULT;

/* ethernet addresses of ports */
static struct ether_addr l2fwd_ports_eth_addr[RTE_MAX_ETHPORTS];

/* mask of enabled ports */
static uint32_t l2fwd_enabled_port_mask = 0;

/* list of enabled ports */
static uint32_t l2fwd_dst_ports[RTE_MAX_ETHPORTS];

static unsigned int l2fwd_rx_queue_per_lcore = 1;

#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT 16
/*
 * The list of queues that must be polled for a given lcore is stored in a 
 * private structure called struct lcore_queue_conf.
 *
 * n_rx_port and rx_port_list[] are used in the main packet processing loop.
 * */
struct lcore_queue_conf {
	unsigned n_rx_port;
	unsigned rx_port_list[MAX_RX_QUEUE_PER_LCORE];
} __rte_cache_aligned;
struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

/*
 * rte_eth_dev_tx_buffer 
 *
 * #include <rte_ethdev.h>
 *
 * Data Fields
 * uint16_t 	size
 * uint16_t 	length
 * struct rte_mbuf * 	pkts []
 *
 * Detailed Description
 * Structure used to buffer packets for future TX Used by APIs rte_eth_tx_buffer and rte_eth_tx_buffer_flush
 * */
static struct rte_eth_dev_tx_buffer *tx_buffer[RTE_MAX_ETHPORTS];

static const struct rte_eth_conf port_conf = {
	.rxmode = {
		.split_hdr_size = 0,
		.header_split   = 0, /**< Header Split disabled */
		.hw_ip_checksum = 0, /**< IP checksum offload disabled */
		.hw_vlan_filter = 0, /**< VLAN filtering disabled */
		.jumbo_frame    = 0, /**< Jumbo Frame Support disabled */
		.hw_strip_crc   = 0, /**< CRC stripped by hardware */
	},
	.txmode = {
		.mq_mode = ETH_MQ_TX_NONE,
	},
};

struct rte_mempool * l2fwd_pktmbuf_pool = NULL;

/* Per-port statistics struct */
struct l2fwd_port_statistics {
	uint64_t tx;
	uint64_t rx;
	uint64_t dropped;
} __rte_cache_aligned;
struct l2fwd_port_statistics port_statistics[RTE_MAX_ETHPORTS];

#define MAX_TIMER_PERIOD 86400 /* 1 day max */
/* A tsc-based timer responsible for triggering statistics printout */
static uint64_t timer_period = 10; /* default period is 10 seconds */

/* Print out statistics on packets dropped */
static void
print_stats(void)
{
	uint64_t total_packets_dropped, total_packets_tx, total_packets_rx;
	unsigned portid;

	total_packets_dropped = 0;
	total_packets_tx = 0;
	total_packets_rx = 0;

	const char clr[] = { 27, '[', '2', 'J', '\0' };
	const char topLeft[] = { 27, '[', '1', ';', '1', 'H','\0' };

		/* Clear screen and move to top left */
	printf("%s%s", clr, topLeft);

	printf("\nPort statistics ====================================");

	for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++) {
		/* skip disabled ports */
		if ((l2fwd_enabled_port_mask & (1 << portid)) == 0)
			continue;
		printf("\nStatistics for port %u ------------------------------"
			   "\nPackets sent: %24"PRIu64
			   "\nPackets received: %20"PRIu64
			   "\nPackets dropped: %21"PRIu64,
			   portid,
			   port_statistics[portid].tx,
			   port_statistics[portid].rx,
			   port_statistics[portid].dropped);

		total_packets_dropped += port_statistics[portid].dropped;
		total_packets_tx += port_statistics[portid].tx;
		total_packets_rx += port_statistics[portid].rx;
	}
	printf("\nAggregate statistics ==============================="
		   "\nTotal packets sent: %18"PRIu64
		   "\nTotal packets received: %14"PRIu64
		   "\nTotal packets dropped: %15"PRIu64,
		   total_packets_tx,
		   total_packets_rx,
		   total_packets_dropped);
	printf("\n====================================================\n");
}

static void
l2fwd_mac_updating(struct rte_mbuf *m, unsigned dest_portid)
{
	struct ether_hdr *eth;
	void *tmp;

	eth = rte_pktmbuf_mtod(m, struct ether_hdr *);

	/* 02:00:00:00:00:xx */
	tmp = &eth->d_addr.addr_bytes[0];
	*((uint64_t *)tmp) = 0x000000000002 + ((uint64_t)dest_portid << 40);

	/* src addr */
	ether_addr_copy(&l2fwd_ports_eth_addr[dest_portid], &eth->s_addr);
}

static void
l2fwd_simple_forward(struct rte_mbuf *m, unsigned portid)
{
	unsigned dst_port;
	int sent;
	struct rte_eth_dev_tx_buffer *buffer;

	dst_port = l2fwd_dst_ports[portid];

	if (mac_updating)
		l2fwd_mac_updating(m, dst_port);

	buffer = tx_buffer[dst_port];
	/*
	 * static uint16_t rte_eth_tx_buffer(uint8_t 	port_id,
	 * 									uint16_t 	queue_id,
	 * 									struct rte_eth_dev_tx_buffer * 	buffer,
	 * 									struct rte_mbuf * 	tx_pkt 
	 * )		
	 * Buffer a single packet for future transmission on a port and queue.
	 *
	 * This function takes a single mbuf/packet and buffers it for later transmission on the particular port and 
	 * queue specified. Once the buffer is full of packets, an attempt will be made to transmit all the buffered packets. 
	 * In case of error, where not all packets can be transmitted, a callback is called with the unsent packets as 
	 * a parameter. If no callback is explicitly set up, the unsent packets are just freed back to the owning mempool. 
	 * The function returns the number of packets actually sent i.e. 0 if no buffer flush occurred, otherwise the 
	 * number of packets successfully flushed.
	 *
	 * Parameters
	 * port_id	The port identifier of the Ethernet device.
	 * queue_id	The index of the transmit queue through which output packets must be sent. The 
	 * 			value must be in the range [0, nb_tx_queue - 1] previously supplied to rte_eth_dev_configure().
	 * buffer	Buffer used to collect packets to be sent.
	 * tx_pkt	Pointer to the packet mbuf to be sent.
	 *
	 * Returns
	 * 0 = packet has been buffered for later transmission N > 0 = packet has been buffered, and the 
	 * buffer was subsequently flushed, causing N packets to be sent, and the error callback to be called for the rest.
	 * */
	sent = rte_eth_tx_buffer(dst_port, 0, buffer, m);
	if (sent)
		port_statistics[dst_port].tx += sent;
}

/* main processing loop */
static void
l2fwd_main_loop(void)
{
	struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
	struct rte_mbuf *m;
	int sent;
	unsigned lcore_id;
	uint64_t prev_tsc, diff_tsc, cur_tsc, timer_tsc;
	unsigned i, j, portid, nb_rx;
	struct lcore_queue_conf *qconf;
	const uint64_t drain_tsc = (rte_get_tsc_hz() + US_PER_S - 1) / US_PER_S *
			BURST_TX_DRAIN_US;
	struct rte_eth_dev_tx_buffer *buffer;

	prev_tsc = 0;
	timer_tsc = 0;

	/*
	 * static unsigned rte_lcore_id(void)	
	 *
	 * Return the ID of the execution unit we are running on.
	 *
	 * Returns
	 * Logical core ID (in EAL thread) or LCORE_ID_ANY (in non-EAL thread)
	 * */
	lcore_id = rte_lcore_id();

	qconf = &lcore_queue_conf[lcore_id];

	if (qconf->n_rx_port == 0) {
		RTE_LOG(INFO, L2FWD, "lcore %u has nothing to do\n", lcore_id);
		return;
	}

	RTE_LOG(INFO, L2FWD, "entering main loop on lcore %u\n", lcore_id);

	for (i = 0; i < qconf->n_rx_port; i++) {
		portid = qconf->rx_port_list[i];
		RTE_LOG(INFO, L2FWD, " -- lcoreid=%u portid=%u\n", lcore_id,
			portid);
	}

	while (!force_quit) {
		/*
		 * 每经过一个时钟周期，tsc寄存器就自动加1， rte_rdtsc()只是获得tsc寄存器的值
		 * */
		cur_tsc = rte_rdtsc();

		/*
		 * TX burst queue drain
		 */
		diff_tsc = cur_tsc - prev_tsc;
		if (unlikely(diff_tsc > drain_tsc)) {
			for (i = 0; i < qconf->n_rx_port; i++) {
				portid = l2fwd_dst_ports[qconf->rx_port_list[i]];
				buffer = tx_buffer[portid];

				/*
				 * static uint16_t rte_eth_tx_buffer_flush(uint8_t port_id,
				 * 											uint16_t 	queue_id,
				 * 											struct rte_eth_dev_tx_buffer * 	buffer 
				 * 										  )		
				 *
				 * Send any packets queued up for transmission on a port and HW queue
				 *
				 * This causes an explicit flush of packets previously buffered via the rte_eth_tx_buffer() function. 
				 * It returns the number of packets successfully sent to the NIC, and calls the error callback for 
				 * any unsent packets. Unless explicitly set up otherwise, the default callback simply frees the 
				 * unsent packets back to the owning mempool.
				 *
				 * Parameters
				 * port_id	The port identifier of the Ethernet device.
				 * queue_id	The index of the transmit queue through which output packets must be sent. The value 
				 * 			must be in the range [0, nb_tx_queue - 1] previously supplied to rte_eth_dev_configure().
				 * buffer	Buffer of packets to be transmit.
				 *
				 * Returns
				 * The number of packets successfully sent to the Ethernet device. The error callback is 
				 * called for any packets which could not be sent.
				 * */
				sent = rte_eth_tx_buffer_flush(portid, 0, buffer);
				if (sent)
					port_statistics[portid].tx += sent;
			}

			/* if timer is enabled */
			if (timer_period > 0) {

				/* advance the timer */
				timer_tsc += diff_tsc;

				/* if timer has reached its timeout */
				if (unlikely(timer_tsc >= timer_period)) {

					/* do this only on master core */
					if (lcore_id == rte_get_master_lcore()) {
						print_stats();
						/* reset the timer */
						timer_tsc = 0;
					}
				}
			}

			prev_tsc = cur_tsc;
		}

		/*
		 * Read packet from RX queues
		 */
		for (i = 0; i < qconf->n_rx_port; i++) {

			portid = qconf->rx_port_list[i];

			/*
			 * 
			 * static uint16_t rte_eth_rx_burst(uint8_t port_id,
			 * 							uint16_t 	queue_id,
			 * 							struct rte_mbuf ** 	rx_pkts,
			 * 							const uint16_t 	nb_pkts 
			 * 						)	
			 *
			 * Retrieve a burst of input packets from a receive queue of an Ethernet device. 
			 * The retrieved packets are stored in rte_mbuf structures whose pointers are supplied in the rx_pkts array.
			 *
			 * The rte_eth_rx_burst() function loops, parsing the RX ring of the receive queue, up to nb_pkts packets, 
			 * and for each completed RX descriptor in the ring, it performs the following operations:
			 * 		1.Initialize the rte_mbuf data structure associated with the RX descriptor according to 
			 * 		  the information provided by the NIC into that RX descriptor.
			 * 		2.Store the rte_mbuf data structure into the next entry of the rx_pkts array.
			 * 		3.Replenish the RX descriptor with a new rte_mbuf buffer allocated from the memory pool 
			 * 		  associated with the receive queue at initialization time.
			 *
			 * When retrieving an input packet that was scattered by the controller into multiple receive descriptors, 
			 * the rte_eth_rx_burst() function appends the associated rte_mbuf buffers to the first buffer of the packet.
			 * */
			nb_rx = rte_eth_rx_burst((uint8_t) portid, 0,
						 pkts_burst, MAX_PKT_BURST);

			port_statistics[portid].rx += nb_rx;

			for (j = 0; j < nb_rx; j++) {
				m = pkts_burst[j];
				/*
				 * static void rte_prefetch0(const volatile void *p)	
				 *
				 * Prefetch a cache line into all cache levels.
				 *
				 * Parameters
				 * p	Address to prefetch
				 *
				 * */

				/*
				 * #define rte_pktmbuf_mtod(m,t) rte_pktmbuf_mtod_offset(m, t, 0)
				 *
				 * A macro that points to the start of the data in the mbuf.
				 *
				 * The returned pointer is cast to type t. Before using this function, the user must ensure that 
				 * the first segment is large enough to accommodate its data.
				 *
				 * Parameters
				 * m	The packet mbuf.
				 * t	The type to cast the result into.
				 * */
				rte_prefetch0(rte_pktmbuf_mtod(m, void *));
				l2fwd_simple_forward(m, portid);
			}
		}
	}
}

static int
l2fwd_launch_one_lcore(__attribute__((unused)) void *dummy)
{
	l2fwd_main_loop();
	return 0;
}

/* display usage */
static void
l2fwd_usage(const char *prgname)
{
	printf("%s [EAL options] -- -p PORTMASK [-q NQ]\n"
	       "  -p PORTMASK: hexadecimal bitmask of ports to configure\n"
	       "  -q NQ: number of queue (=ports) per lcore (default is 1)\n"
		   "  -T PERIOD: statistics will be refreshed each PERIOD seconds (0 to disable, 10 default, 86400 maximum)\n"
		   "  --[no-]mac-updating: Enable or disable MAC addresses updating (enabled by default)\n"
		   "      When enabled:\n"
		   "       - The source MAC address is replaced by the TX port MAC address\n"
		   "       - The destination MAC address is replaced by 02:00:00:00:00:TX_PORT_ID\n",
	       prgname);
}

static int
l2fwd_parse_portmask(const char *portmask)
{
	char *end = NULL;
	unsigned long pm;

	/* parse hexadecimal string */
	pm = strtoul(portmask, &end, 16);
	if ((portmask[0] == '\0') || (end == NULL) || (*end != '\0'))
		return -1;

	if (pm == 0)
		return -1;

	return pm;
}

static unsigned int
l2fwd_parse_nqueue(const char *q_arg)
{
	char *end = NULL;
	unsigned long n;

	/* parse hexadecimal string */
	n = strtoul(q_arg, &end, 10);
	if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
		return 0;
	if (n == 0)
		return 0;
	if (n >= MAX_RX_QUEUE_PER_LCORE)
		return 0;

	return n;
}

static int
l2fwd_parse_timer_period(const char *q_arg)
{
	char *end = NULL;
	int n;

	/* parse number string */
	n = strtol(q_arg, &end, 10);
	if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
		return -1;
	if (n >= MAX_TIMER_PERIOD)
		return -1;

	return n;
}

/* Parse the argument given in the command line of the application */
static int
l2fwd_parse_args(int argc, char **argv)
{
	int opt, ret, timer_secs;
	char **argvopt;
	int option_index;
	char *prgname = argv[0];
	static struct option lgopts[] = {
		{ "mac-updating", no_argument, &mac_updating, 1},
		{ "no-mac-updating", no_argument, &mac_updating, 0},
		{NULL, 0, 0, 0}
	};

	argvopt = argv;

	while ((opt = getopt_long(argc, argvopt, "p:q:T:",
				  lgopts, &option_index)) != EOF) {

		switch (opt) {
		/* portmask */
		case 'p':
			l2fwd_enabled_port_mask = l2fwd_parse_portmask(optarg);
			if (l2fwd_enabled_port_mask == 0) {
				printf("invalid portmask\n");
				l2fwd_usage(prgname);
				return -1;
			}
			break;

		/*
		 * -q 的意思:
		 *  The application uses one lcore to poll one or several ports, depending on the -q option, 
		 *  which specifies the number of queues per lcore.
		 *
		 *  For example, if the user specifies -q 4, the application is able to poll four ports with 
		 *  one lcore. If there are 16 ports on the target (and if the portmask argument is -p ffff ), 
		 *  the application will need four lcores to poll all the ports.
		 * */
		/* nqueue */
		case 'q':
			l2fwd_rx_queue_per_lcore = l2fwd_parse_nqueue(optarg);
			if (l2fwd_rx_queue_per_lcore == 0) {
				printf("invalid queue number\n");
				l2fwd_usage(prgname);
				return -1;
			}
			break;

		/* timer period */
		case 'T':
			timer_secs = l2fwd_parse_timer_period(optarg);
			if (timer_secs < 0) {
				printf("invalid timer period\n");
				l2fwd_usage(prgname);
				return -1;
			}
			timer_period = timer_secs;
			break;

		/* long options */
		case 0:
			break;

		default:
			l2fwd_usage(prgname);
			return -1;
		}
	}

	if (optind >= 0)
		argv[optind-1] = prgname;

	ret = optind-1;
	optind = 0; /* reset getopt lib */
	return ret;
}

/* Check the link status of all ports in up to 9s, and print them finally */
static void
check_all_ports_link_status(uint8_t port_num, uint32_t port_mask)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
	uint8_t portid, count, all_ports_up, print_flag = 0;
	struct rte_eth_link link;

	printf("\nChecking link status");
	fflush(stdout);
	for (count = 0; count <= MAX_CHECK_TIME; count++) {
		if (force_quit)
			return;
		all_ports_up = 1;
		for (portid = 0; portid < port_num; portid++) {
			if (force_quit)
				return;
			if ((port_mask & (1 << portid)) == 0)
				continue;
			memset(&link, 0, sizeof(link));
			rte_eth_link_get_nowait(portid, &link);
			/* print link status if flag set */
			if (print_flag == 1) {
				if (link.link_status)
					printf("Port %d Link Up - speed %u "
						"Mbps - %s\n", (uint8_t)portid,
						(unsigned)link.link_speed,
				(link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
					("full-duplex") : ("half-duplex\n"));
				else
					printf("Port %d Link Down\n",
						(uint8_t)portid);
				continue;
			}
			/* clear all_ports_up flag if any link down */
			if (link.link_status == ETH_LINK_DOWN) {
				all_ports_up = 0;
				break;
			}
		}
		/* after finally printing all link status, get out */
		if (print_flag == 1)
			break;

		if (all_ports_up == 0) {
			printf(".");
			fflush(stdout);
			rte_delay_ms(CHECK_INTERVAL);
		}

		/* set the print_flag if all ports up or timeout */
		if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
			print_flag = 1;
			printf("done\n");
		}
	}
}

static void
signal_handler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM) {
		printf("\n\nSignal %d received, preparing to exit...\n",
				signum);
		force_quit = true;
	}
}

int
main(int argc, char **argv)
{
	struct lcore_queue_conf *qconf;
	struct rte_eth_dev_info dev_info;
	int ret;
	uint8_t nb_ports;
	uint8_t nb_ports_available;
	uint8_t portid, last_port;
	unsigned lcore_id, rx_lcore_id;
	unsigned nb_ports_in_mask = 0;

	/* init EAL */
	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");
	argc -= ret;
	argv += ret;

	force_quit = false;
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	/* parse application arguments (after the EAL ones) */
	ret = l2fwd_parse_args(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Invalid L2FWD arguments\n");

	printf("MAC updating %s\n", mac_updating ? "enabled" : "disabled");

	/* convert to number of cycles */
	timer_period *= rte_get_timer_hz();

	/*
	 * Once the arguments are parsed, the mbuf pool is created. The mbuf pool 
	 * contains a set of mbuf objects that will be used by the driver and the 
	 * application to store network packet data.
	 * */
	/*
	 * struct rte_mempool* rte_pktmbuf_pool_create	(const char * 	name,
	 * 				unsigned 	n,
	 * 				unsigned 	cache_size,
	 * 				uint16_t 	priv_size,
	 * 				uint16_t 	data_room_size,
	 * 				int 		socket_id 
	 * )	
	 *
	 * Create a mbuf pool.
	 * This function creates and initializes a packet mbuf pool. It is a wrapper to rte_mempool functions.
	 *
	 * Parameters
	 * name		The name of the mbuf pool.
	 * n		The number of elements in the mbuf pool. The optimum size (in terms of memory usage) 
	 * 			for a mempool is when n is a power of two minus one: n = (2^q - 1).
	 * cache_size	Size of the per-core object cache. See rte_mempool_create() for details.
	 * priv_size	Size of application private are between the rte_mbuf structure and the data buffer. 
	 * 				This value must be aligned to RTE_MBUF_PRIV_ALIGN.
	 * data_room_size	Size of data buffer in each mbuf, including RTE_PKTMBUF_HEADROOM.
	 * socket_id	The socket identifier where the memory should be allocated. The value can be 
	 * 				SOCKET_ID_ANY if there is no NUMA constraint for the reserved zone.
	 * */
	/* create the mbuf pool */
	l2fwd_pktmbuf_pool = rte_pktmbuf_pool_create("mbuf_pool", NB_MBUF,
		MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
		rte_socket_id());
	if (l2fwd_pktmbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

	nb_ports = rte_eth_dev_count();
	if (nb_ports == 0)
		rte_exit(EXIT_FAILURE, "No Ethernet ports - bye\n");

	/* reset l2fwd_dst_ports */
	for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++)
		l2fwd_dst_ports[portid] = 0;
	last_port = 0;

	/*
	 * Each logical core is assigned a dedicated TX queue on each port.
	 */
	for (portid = 0; portid < nb_ports; portid++) {
		/* skip ports that are not enabled */
		if ((l2fwd_enabled_port_mask & (1 << portid)) == 0)
			continue;

		if (nb_ports_in_mask % 2) {
			l2fwd_dst_ports[portid] = last_port;
			l2fwd_dst_ports[last_port] = portid;
		}
		else
			last_port = portid;

		nb_ports_in_mask++;

		/*
		 * void rte_eth_dev_info_get(uint8_t port_id, struct rte_eth_dev_info *dev_info)		
		 * Retrieve the contextual information of an Ethernet device.
		 *
		 * Parameters
		 * port_id	The port identifier of the Ethernet device.
		 * dev_info	A pointer to a structure of type rte_eth_dev_info to be filled with the 
		 * 			contextual information of the Ethernet device.
		 * */
		 rte_eth_dev_info_get(portid, &dev_info);
	}

	if (nb_ports_in_mask % 2) {
		printf("Notice: odd number of ports in portmask.\n");
		l2fwd_dst_ports[last_port] = last_port;
	}

	rx_lcore_id = 0;
	qconf = NULL;

	/* Initialize the port/queue configuration of each logical core */
	for (portid = 0; portid < nb_ports; portid++) {
		/* skip ports that are not enabled */
		if ((l2fwd_enabled_port_mask & (1 << portid)) == 0)
			continue;

		/* get the lcore_id for this port */
		/* l2fwd_rx_queue_per_lcore : A number of queues (=ports) per lcore. */
		while (rte_lcore_is_enabled(rx_lcore_id) == 0 ||
		       lcore_queue_conf[rx_lcore_id].n_rx_port ==
		       l2fwd_rx_queue_per_lcore) {
			rx_lcore_id++;
			if (rx_lcore_id >= RTE_MAX_LCORE)
				rte_exit(EXIT_FAILURE, "Not enough cores\n");
		}

		if (qconf != &lcore_queue_conf[rx_lcore_id])
			/* Assigned a new logical core in the loop above. */
			qconf = &lcore_queue_conf[rx_lcore_id];

		qconf->rx_port_list[qconf->n_rx_port] = portid;
		qconf->n_rx_port++;
		printf("Lcore %u: RX port %u\n", rx_lcore_id, (unsigned) portid);
	}

	nb_ports_available = nb_ports;

	/* Initialise each port */
	for (portid = 0; portid < nb_ports; portid++) {
		/* skip ports that are not enabled */
		if ((l2fwd_enabled_port_mask & (1 << portid)) == 0) {
			printf("Skipping disabled port %u\n", (unsigned) portid);
			nb_ports_available--;
			continue;
		}

		/* init port */
		printf("Initializing port %u... ", (unsigned) portid);
		fflush(stdout);
		/*
		 * Configure an Ethernet device. This function must be invoked first before any other 
		 * function in the Ethernet API. This function can also be re-invoked when a device 
		 * is in the stopped state.
		 *
		 * Parameters
		 * port_id	    The port identifier of the Ethernet device to configure.
		 * nb_rx_queue	The number of receive queues to set up for the Ethernet device.
		 * nb_tx_queue	The number of transmit queues to set up for the Ethernet device.
		 * eth_conf		The pointer to the configuration data to be used for the Ethernet device. 
		 * 				The rte_eth_conf structure includes:
		 * 					the hardware offload features to activate, with dedicated fields for each statically configurable offload hardware feature provided by Ethernet devices, such as IP checksum or VLAN tag stripping for example.
		 * 					the Receive Side Scaling (RSS) configuration when using multiple RX queues per port.
		 * */
		ret = rte_eth_dev_configure(portid, 1, 1, &port_conf);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=%u\n",
				  ret, (unsigned) portid);
	
		/*
		 * void rte_eth_macaddr_get	(uint8_t port_id, struct ether_addr * mac_addr)		
		 *
		 * Retrieve the Ethernet address of an Ethernet device.
		 *
		 * Parameters
		 * port_id	The port identifier of the Ethernet device.
		 * mac_addr	A pointer to a structure of type ether_addr to be filled with the 
		 * 			Ethernet address of the Ethernet device.
		 * */
		rte_eth_macaddr_get(portid, &l2fwd_ports_eth_addr[portid]);

		/* init one RX queue */
		fflush(stdout);

		/*
		 * int rte_eth_rx_queue_setup(uint8_t port_id,
		 * 							uint16_t 	rx_queue_id,
		 * 							uint16_t 	nb_rx_desc,
		 * 							unsigned int 	socket_id,
		 * 							const struct rte_eth_rxconf * 	rx_conf,
		 * 							struct rte_mempool * 	mb_pool 
		 * )		
		 *
		 * Allocate and set up a receive queue for an Ethernet device.
		 * The function allocates a contiguous block of memory for nb_rx_desc receive descriptors from a 
		 * memory zone associated with socket_id and initializes each receive descriptor with a network 
		 * buffer allocated from the memory pool mb_pool.
		 *
		 * Parameters
		 *
		 * port_id		The port identifier of the Ethernet device.
		 * rx_queue_id	The index of the receive queue to set up. The value must be in the range [0, nb_rx_queue - 1] 
		 * 				previously supplied to rte_eth_dev_configure().
		 * nb_rx_desc	The number of receive descriptors to allocate for the receive ring.(发送队列的大小)
		 * socket_id	The socket_id argument is the socket identifier in case of NUMA. The value can be 
		 * 				SOCKET_ID_ANY if there is no NUMA constraint for the DMA memory allocated for the 
		 * 				receive descriptors of the ring.
		 * rx_conf		The pointer to the configuration data to be used for the receive queue. NULL value is allowed,
		 * 				in which case default RX configuration will be used. The rx_conf structure contains an 
		 * 				rx_thresh structure with the values of the Prefetch, Host, and Write-Back threshold registers 
		 * 				of the receive ring.
		 * mb_pool		The pointer to the memory pool from which to allocate rte_mbuf network memory buffers to 
		 * 				populate each descriptor of the receive ring.
		 * */
		ret = rte_eth_rx_queue_setup(portid, 0, nb_rxd,
					     rte_eth_dev_socket_id(portid),
					     NULL,
					     l2fwd_pktmbuf_pool);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup:err=%d, port=%u\n",
				  ret, (unsigned) portid);

		/* init one TX queue on each port */
		fflush(stdout);

		/*
		 * int rte_eth_tx_queue_setup(uint8_t port_id,
		 * 							uint16_t 	tx_queue_id,
		 * 							uint16_t 	nb_tx_desc,
		 * 							unsigned int 	socket_id,
		 * 							const struct rte_eth_txconf * 	tx_conf 
		 * )		
		 *
		 * Allocate and set up a transmit queue for an Ethernet device.
		 *
		 * Parameters
		 * port_id		The port identifier of the Ethernet device.
		 * tx_queue_id	The index of the transmit queue to set up. The value must be in the range [0, nb_tx_queue - 1]
		 * 				previously supplied to rte_eth_dev_configure().
		 * nb_tx_desc	The number of transmit descriptors to allocate for the transmit ring.
		 * socket_id	The socket_id argument is the socket identifier in case of NUMA. Its value can be 
		 * 				SOCKET_ID_ANY if there is no NUMA constraint for the DMA memory allocated for the 
		 * 				transmit descriptors of the ring.
		 * tx_conf		The pointer to the configuration data to be used for the transmit queue. NULL value 
		 * 				is allowed, in which case default RX configuration will be used. The tx_conf structure 
		 * 				contains the following data:
		 * 					1.The tx_thresh structure with the values of the Prefetch, Host, and Write-Back threshold 
		 * 					registers of the transmit ring. When setting Write-Back threshold to the value greater 
		 * 					then zero, tx_rs_thresh value should be explicitly set to one.
		 * 					2.The tx_free_thresh value indicates the [minimum] number of network buffers that must 
		 * 					be pending in the transmit ring to trigger their [implicit] freeing by the driver 
		 * 					transmit function.
		 * 					3.The tx_rs_thresh value indicates the [minimum] number of transmit descriptors that 
		 * 					must be pending in the transmit ring before setting the RS bit on a descriptor by the 
		 * 					driver transmit function. The tx_rs_thresh value should be less or equal then 
		 * 					tx_free_thresh value, and both of them should be less then nb_tx_desc - 3.
		 * 					4.The txq_flags member contains flags to pass to the TX queue setup function to 
		 * 					configure the behavior of the TX queue. This should be set to 0 if no special 
		 * 					configuration is required.
		 * 					
		 * 					Note that setting tx_free_thresh or tx_rs_thresh value to 0 forces the transmit 
		 * 					function to use default values.
		 * */
		ret = rte_eth_tx_queue_setup(portid, 0, nb_txd,
				rte_eth_dev_socket_id(portid),
				NULL);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup:err=%d, port=%u\n",
				ret, (unsigned) portid);

		/*
		 * 
		 * void* rte_zmalloc_socket(const char* type,
		 * 								size_t 	size,
		 * 								unsigned 	align,
		 * 								int 	socket 
		 * )		
		 * Allocate zero'ed memory from the heap.
		 * Equivalent to rte_malloc() except that the memory zone is initialised with zeros.
		 *
		 * Parameters
		 * type		A string identifying the type of allocated objects 
		 * 			(useful for debug purposes, such as identifying the cause of a memory leak). Can be NULL.
		 * size		Size (in bytes) to be allocated.
		 * align	If 0, the return is a pointer that is suitably aligned for any kind of variable 
		 * 			(in the same manner as malloc()). Otherwise, the return is a pointer that is a 
		 * 			multiple of align. In this case, it must obviously be a power of two. (Minimum 
		 * 			alignment is the cacheline size, i.e. 64-bytes)
		 * socket	NUMA socket to allocate memory on. If SOCKET_ID_ANY is used, this function will 
		 * 			behave the same as rte_zmalloc().
		 *
		 * Returns
		 * NULL on error. Not enough memory, or invalid arguments (size is 0, align is not a power of two).
		 * Otherwise, the pointer to the allocated object.
		 * */
		/* Initialize TX buffers */
		tx_buffer[portid] = rte_zmalloc_socket("tx_buffer",
				RTE_ETH_TX_BUFFER_SIZE(MAX_PKT_BURST), 0,
				rte_eth_dev_socket_id(portid));
		if (tx_buffer[portid] == NULL)
			rte_exit(EXIT_FAILURE, "Cannot allocate buffer for tx on port %u\n",
					(unsigned) portid);

		rte_eth_tx_buffer_init(tx_buffer[portid], MAX_PKT_BURST);

		ret = rte_eth_tx_buffer_set_err_callback(tx_buffer[portid],
				rte_eth_tx_buffer_count_callback,
				&port_statistics[portid].dropped);
		if (ret < 0)
				rte_exit(EXIT_FAILURE, "Cannot set error callback for "
						"tx buffer on port %u\n", (unsigned) portid);

		/* Start device */
		ret = rte_eth_dev_start(portid);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_dev_start:err=%d, port=%u\n",
				  ret, (unsigned) portid);

		printf("done: \n");

		rte_eth_promiscuous_enable(portid);

		printf("Port %u, MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n\n",
				(unsigned) portid,
				l2fwd_ports_eth_addr[portid].addr_bytes[0],
				l2fwd_ports_eth_addr[portid].addr_bytes[1],
				l2fwd_ports_eth_addr[portid].addr_bytes[2],
				l2fwd_ports_eth_addr[portid].addr_bytes[3],
				l2fwd_ports_eth_addr[portid].addr_bytes[4],
				l2fwd_ports_eth_addr[portid].addr_bytes[5]);

		/* initialize port stats */
		memset(&port_statistics, 0, sizeof(port_statistics));
	}

	if (!nb_ports_available) {
		rte_exit(EXIT_FAILURE,
			"All available ports are disabled. Please set portmask.\n");
	}

	check_all_ports_link_status(nb_ports, l2fwd_enabled_port_mask);

	ret = 0;

	/*
	 * 在每个逻辑核上启动线程，开始转发。
	 * */
	/* launch per-lcore init on every lcore */
	rte_eal_mp_remote_launch(l2fwd_launch_one_lcore, NULL, CALL_MASTER);
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		if (rte_eal_wait_lcore(lcore_id) < 0) {
			ret = -1;
			break;
		}
	}

	for (portid = 0; portid < nb_ports; portid++) {
		if ((l2fwd_enabled_port_mask & (1 << portid)) == 0)
			continue;
		printf("Closing port %d...", portid);

		/*
		 * void rte_eth_dev_stop(uint8_t port_id)	
		 * Stop an Ethernet device. The device can be restarted with a call to rte_eth_dev_start()
		 *
		 * Parameters
		 * port_id	The port identifier of the Ethernet device.
		 *
		 * */
		rte_eth_dev_stop(portid);
		/*
		 * void rte_eth_dev_close(uint8_t port_id)	
		 * Close a stopped Ethernet device. The device cannot be restarted! The function frees all 
		 * resources except for needed by the closed state. To free these resources, call rte_eth_dev_detach().
		 *
		 * Parameters
		 * port_id	The port identifier of the Ethernet device.
		 * */
		rte_eth_dev_close(portid);
		printf(" Done\n");
	}
	printf("Bye...\n");

	return ret;
}
