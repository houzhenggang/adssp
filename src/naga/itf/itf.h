#ifndef __ITF_H__
#define __ITF_H__


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <rte_config.h>
#include <rte_common.h>
#include <rte_log.h>
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
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

#include "boots.h"
#include "naga_types.h"
#include "bts_list.h"

#define MAX_PKT_BURST 32 
#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT 16

struct mbuf_table {
	unsigned len;
	struct rte_mbuf *m_table[MAX_PKT_BURST];
};

struct lcore_queue_conf {
    unsigned int n_rx_port;
    unsigned int rx_port_list[MAX_RX_QUEUE_PER_LCORE];
    struct mbuf_table tx_mbufs[RTE_MAX_ETHPORTS];
	uint64_t  rx_packet;
	uint16_t  queue;
} __rte_cache_aligned;
/* Per-port statistics struct */
struct itf_port_statistics {
	uint64_t tx;
	uint64_t rx;
	uint64_t dropped;
    uint64_t fail;
} __rte_cache_aligned;
/* Per-port statistics struct */

struct l2fwd_port_statistics {
	uint64_t tx;
	uint64_t rx;
	uint64_t dropped;
} __rte_cache_aligned;


typedef struct
{
	pthread_t recv_thread;
	pcap_t * fp;
	char   *ifname;
	struct list_head node;
}libpcap_handler_t;


typedef berr (*rx_process_func)(struct rte_mbuf *m);

//extern struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

berr itf_port_check(void);

//void itf_rx_burst(rx_process_func process_func);
void itf_rx_burst(rx_process_func process_func, unsigned , uint16_t);

int itf_send_packet(struct rte_mbuf *m, uint8_t port);

int itf_send_packet_imm(struct rte_mbuf *m, uint8_t port);

void itf_tx_burst(void);

berr ift_raw_send_packet(void *fp, uint8_t * buff, int len);

berr itf_raw_socket_init(char *ifname);

void itf_set_hytag_pcap(hytag_t * tag);


berr libpcap_rx_loop_setup(char * ifname);

berr libpcap_rx_loop_unset(char * ifname);


#endif /* !__ITF_H__ */
