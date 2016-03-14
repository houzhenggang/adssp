#include "itf.h"
//#include "itf_debug.h"
//#include <rte_config.h>
//#include <rte_ethdev.h>
#include "bts_debug.h"
#include "itf_stat.h"
#include "bts_cnt.h"

extern struct itf_port_statistics port_statistics[RTE_MAX_ETHPORTS];
extern struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

/* Send the burst of packets on an output interface */
static void
itf_send_burst(struct lcore_queue_conf *qconf, unsigned n, uint8_t port)
{
	struct rte_mbuf **m_table;
	unsigned ret;
	unsigned queueid =0;

	m_table = (struct rte_mbuf **)qconf->tx_mbufs[port].m_table;

	ret = rte_eth_tx_burst(port, (uint16_t) queueid, m_table, (uint16_t) n);
	port_statistics[port].tx += ret;
	if (unlikely(ret < n)) {
		port_statistics[port].dropped += (n - ret);
		do {
			 rte_pktmbuf_free(m_table[ret]);
		} while (++ret < n);
	}
}
#if 0
static void
itf_send_burst_not_free(struct lcore_queue_conf *qconf, unsigned n, uint8_t port)
{
	struct rte_mbuf **m_table;
	unsigned ret;
	unsigned queueid =0;

	m_table = (struct rte_mbuf **)qconf->tx_mbufs[port].m_table;

	ret = rte_eth_tx_burst(port, (uint16_t) queueid, m_table, (uint16_t) n);
	port_statistics[port].tx += ret;
	if (unlikely(ret < n)) {
		port_statistics[port].dropped += (n - ret);
		do {
			 //rte_pktmbuf_free(m_table[ret]);
		} while (++ret < n);
	}
}
#endif

/* Enqueue packets for TX and prepare them to be sent */
int itf_send_packet(struct rte_mbuf *m, uint8_t port)
{
	unsigned lcore_id, len;
	struct lcore_queue_conf *qconf;

	lcore_id = rte_lcore_id();

	qconf = &lcore_queue_conf[lcore_id];
	len = qconf->tx_mbufs[port].len;
	qconf->tx_mbufs[port].m_table[len] = m;
	len++;

	/* enough pkts to be sent */
	if (unlikely(len == MAX_PKT_BURST)) {
		itf_send_burst(qconf, MAX_PKT_BURST, port);
		len = 0;
	}

	qconf->tx_mbufs[port].len = len;
	return 0;
}


int itf_send_packet_imm(struct rte_mbuf *m, uint8_t port)
{
    int ret = 0;
    signed queueid = 0;
    
    if (itf_tx_is_enable())
    {
        ret = rte_eth_tx_burst(port, (uint16_t) queueid, &m, 1);
        cnt_inc(ITF_OPKTS);
        cnt_add(ITF_OBYTS, m->data_len);
    }
    
    return ret;
}




void itf_tx_burst(void)
{
    unsigned portid;
    unsigned lcore_id = rte_lcore_id();
	struct lcore_queue_conf *qconf;

	qconf = &lcore_queue_conf[lcore_id];

    for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++) {
        if (qconf->tx_mbufs[portid].len == 0)
            continue;
        itf_send_burst(&lcore_queue_conf[lcore_id],
                 qconf->tx_mbufs[portid].len,
                 (uint8_t) portid);
        qconf->tx_mbufs[portid].len = 0;
    }
}

/* End of file */
