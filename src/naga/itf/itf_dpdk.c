/* End of file */
#include "itf.h"
#include "bts_debug.h"

#include "itf_debug.h"

extern struct itf_port_statistics port_statistics[RTE_MAX_ETHPORTS];
extern struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

/* Send the burst of packets on an output interface */
static void
dpdk_send_burst(struct lcore_queue_conf *qconf, unsigned n, uint8_t port)
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
dpdk_send_burst_not_free(struct lcore_queue_conf *qconf, unsigned n, uint8_t port)
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
int dpdk_send_packet(struct rte_mbuf *m, uint8_t port)
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
		dpdk_send_burst(qconf, MAX_PKT_BURST, port);
		len = 0;
	}

	qconf->tx_mbufs[port].len = len;
	return 0;
}


int dpdk_send_packet_imm(struct rte_mbuf *m, uint8_t port)
{
    int ret;
    signed queueid = 0;
    
    ret = rte_eth_tx_burst(port, (uint16_t) queueid, &m, 1);
    
    return ret;
}

void dpdk_tx_burst(void)
{
    unsigned portid;
    unsigned lcore_id = rte_lcore_id();
	struct lcore_queue_conf *qconf;

	qconf = &lcore_queue_conf[lcore_id];

    for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++) {
        if (qconf->tx_mbufs[portid].len == 0)
            continue;
        dpdk_send_burst(&lcore_queue_conf[lcore_id],
                 qconf->tx_mbufs[portid].len,
                 (uint8_t) portid);
        qconf->tx_mbufs[portid].len = 0;
    }
}

extern struct itf_port_statistics port_statistics[RTE_MAX_ETHPORTS];

extern struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

berr itf_port_check(void)
{
    uint32_t i, portid = 0;
    unsigned int lcore_id;
    struct lcore_queue_conf *qconf = NULL;

    lcore_id = rte_lcore_id();
    qconf = &lcore_queue_conf[lcore_id];

    if (qconf->n_rx_port == 0) {
        DBG_INFO(MOD_ITF,"lcore %u has nothing to do\n", lcore_id);
        BRET(E_INIT);
    }

    DBG_INFO(MOD_DBG,"entering main loop on lcore %u\n", lcore_id);

    for (i = 0; i < qconf->n_rx_port; i++) {
        portid = qconf->rx_port_list[i];
        DBG_INFO(MOD_ITF," -- lcoreid=%u portid=%u\n", lcore_id, portid);
    }

    BRET(E_SUCCESS);
}

void dpdk_rx_burst(rx_process_func process_func)
{
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
	struct rte_mbuf *m;
    unsigned int lcore_id;
    struct lcore_queue_conf *qconf = NULL;

    unsigned i, j, portid, nb_rx;

    berr ec = E_SUCCESS;

    lcore_id = rte_lcore_id();
    qconf = &lcore_queue_conf[lcore_id];

    for (i = 0; i < qconf->n_rx_port; i++) {

        portid = qconf->rx_port_list[i];
        nb_rx = rte_eth_rx_burst((uint8_t) portid, 0,
                     pkts_burst, MAX_PKT_BURST);

        port_statistics[portid].rx += nb_rx;

        for (j = 0; j < nb_rx; j++) {
            m = pkts_burst[j];
            rte_prefetch0(rte_pktmbuf_mtod(m, void *));

            ec = process_func(m);
            if(ec != E_SUCCESS) {
                port_statistics[portid].fail += 1;
                DBG_WARN(MOD_ITF, "Packet process fail:%s!\n", berr_msg(ec));

                //DOPT_SELECT(DOPT_FPR, );
            }

            rte_pktmbuf_free(m);
        }
    }
}

/* End of file */
