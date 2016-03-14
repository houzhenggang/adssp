
/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/home/shan/work/rose/src/test-1/template\packet.c
#       @date         :2015-09-22 23:34
#       @algorithm    :
=============================================================================*/
#include "unistd.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <rte_cycles.h>
#include <rte_byteorder.h>
#include <rte_common.h>
#include <rte_debug.h>
#include <rte_ethdev.h>
#include <rte_log.h>
#include <rte_lcore.h>
#include <rte_memory.h>
#include <rte_string_fns.h>
#include <rte_eth_bond.h>
#include "boots.h"
#include "pid.h"
#include "rte_tcp.h"
#include "rte_ip.h"

//#define DEBUG
#ifdef  DEBUG   
#define debug(fmt,args...)  printf ("func(%s), line(%d)"fmt"\n" ,__FUNCTION__, __LINE__, ##args)
#else   
#define debug(fmt,args...)   
#endif  /* DEBUG */ 



struct tcp_option_timestamp_hdr
{
    uint8_t  kind;
    uint8_t  length;
    uint32_t value;
    uint32_t echo;
} __attribute__((__packed__));

#define TCP_OPTION_TIMESTAMP_OFFSET_START 2
#define DIRECTION_DIFFERENT 1
#define DIRECTION_SAME 2

berr
ads_tcp_head_modify(struct tcp_hdr *tcphdr, hytag_t *hytag, uint8_t direction)
{
    uint16_t sport, dport;
    uint32_t seq, ack;
    struct tcp_option_timestamp_hdr *timestamp_hdr = NULL;
    uint32_t time, echo; /* tcp option timestamp */

    if ( NULL == tcphdr || NULL == hytag )
    {
        BRET(E_PARAM);
    }

    if ( DIRECTION_DIFFERENT != direction &&  DIRECTION_SAME != direction)
    {
        BRET(E_PARAM);
    }

    /* switch sport and dport */
    if ( DIRECTION_DIFFERENT == direction)
    {
        sport = tcphdr->src_port;
        dport = tcphdr->dst_port;
        debug("tcp_hdr switch before: sport(%d), dport(%d)", ntohs(sport), ntohs(dport));
        tcphdr->src_port = dport;
        tcphdr->dst_port = sport;
        debug("tcp_hdr switch  after: sport(%d), dport(%d)", ntohs(tcphdr->src_port), ntohs(tcphdr->dst_port));
    }

    /*calc the seq and ack, and then switch */
    seq = ntohl(tcphdr->sent_seq);
    ack = ntohl(tcphdr->recv_ack);
    debug("tcp_hdr switch  before: seq(%x-%u), ack(%x-%u)", seq, seq, ack, ack);

    if ( DIRECTION_DIFFERENT == direction)
    {
        tcphdr->sent_seq =  htonl(ack);
        tcphdr->recv_ack =   htonl(seq + hytag->l5_len);
    }
        
    else
    if ( DIRECTION_SAME == direction)
    {
        tcphdr->sent_seq = htonl(seq + hytag->l5_len);
        tcphdr->recv_ack = htonl(ack);
    }
    debug("tcp_hdr switch  after: seq(%x-%u), ack(%x-%u)", 
            ntohl(tcphdr->sent_seq), ntohl(tcphdr->sent_seq)
                , ntohl(tcphdr->recv_ack), ntohl(tcphdr->recv_ack));


    /* switch the option timestamp */
    uint8_t tcphd_len = (tcphdr->data_off >>4) &0xf;
    tcphd_len = tcphd_len * 4; 

        if(tcphd_len >= 32)
        {
            timestamp_hdr = (struct tcp_option_timestamp_hdr *) 
                        (((char *)tcphdr) + TCP_OPTION_TIMESTAMP_OFFSET_START + 20);
            if ( NULL == timestamp_hdr )
            {
                BRET(E_NULL);
            } 
            if ( DIRECTION_DIFFERENT == direction)
            {
            
                if(timestamp_hdr->kind == 8 && timestamp_hdr->length == 10)
                {
                    time = timestamp_hdr->value;
                    echo = timestamp_hdr->echo;
                    debug("tcp_hdr switch  before: time(%d), echo(%d)", ntohl(time), ntohl(echo));
                    timestamp_hdr->value = echo;
                    timestamp_hdr->echo  = time;
                    debug("tcp_hdr switch  after: time(%d), echo(%d)", ntohl(timestamp_hdr->value), ntohl(timestamp_hdr->echo));
                }   
            }
            else
            {
                 timestamp_hdr->value =htonl( ntohl(timestamp_hdr->value) + 20001);
            }
        }
   	else if(tcphd_len < 20)
	{
		printf("tcphdr len = %d\n", tcphd_len);	
		
	}
    	else
	{
	}
    return E_SUCCESS;
}


uint16_t
ads_tcpudp_cksum(const struct ipv4_hdr *ipv4_hdr, const void *l4_hdr)
{
    return  rte_ipv4_udptcp_cksum(ipv4_hdr, l4_hdr);
}

berr
ads_ip_head_modify(struct ipv4_hdr* ip_hdr, hytag_t *hytag, uint8_t direction)
{
    uint32_t sip,dip;
    
    if ( NULL == ip_hdr || NULL == hytag)
    {
        BRET(E_PARAM);
    }

    if ( DIRECTION_DIFFERENT != direction && DIRECTION_SAME != direction)
    {
        BRET(E_PARAM);
    }

    /* update total length */
    uint16_t ip_paylen = (hytag->l5_offset - hytag->l3_offset + hytag->l5_len);
    ip_hdr->total_length = htons(ip_paylen );

    ip_hdr->time_to_live = 64;
    ip_hdr->packet_id ++;
    /* update ip */
    if (DIRECTION_DIFFERENT == direction)
    {
        sip = ip_hdr->src_addr;
        dip = ip_hdr->dst_addr;
        ip_hdr->src_addr = dip;
        ip_hdr->dst_addr = sip;
        
    }

    return E_SUCCESS;
}

berr
ads_ipv4_cksum_update( struct ipv4_hdr *ip_hdr)
{
    if ( NULL == ip_hdr )
    {
        BRET(E_PARAM);
    }

    ip_hdr->hdr_checksum = 0;
    ip_hdr->hdr_checksum = rte_ipv4_cksum(ip_hdr);
    return E_SUCCESS;
}

int   ads_mac_enable[2] = {0,0};
uint8_t ads_mac[2][6];



berr ads_mac_get(int dst_or_src, int *custom, uint8_t * mac)
{
	if(dst_or_src < 0 || dst_or_src > 2)
		return E_PARAM;

	*custom =ads_mac_enable[dst_or_src];
	mac[0] = ads_mac[dst_or_src][0];
	mac[1] = ads_mac[dst_or_src][1];
	mac[2] = ads_mac[dst_or_src][2];
	mac[3] = ads_mac[dst_or_src][3];
	mac[4] = ads_mac[dst_or_src][4];
	mac[5] = ads_mac[dst_or_src][5];
	return E_SUCCESS;
}


berr ads_mac_set(int dst_or_src, int custom, uint8_t* mac )
{
	if(dst_or_src<0 || dst_or_src > 2)
		return E_PARAM;

	if(custom == 0)
	{
		ads_mac_enable[dst_or_src] = 0;
	}
	else
	{
		ads_mac[dst_or_src][0] = mac[0];
		ads_mac[dst_or_src][1] = mac[1];
		ads_mac[dst_or_src][2] = mac[2];
		ads_mac[dst_or_src][3] = mac[3];
		ads_mac[dst_or_src][4] = mac[4];
		ads_mac[dst_or_src][5] = mac[5];

		ads_mac_enable[dst_or_src] = 1;

	}

	return E_SUCCESS;
	
}

berr
ads_eth_head_modify(struct ether_hdr *eth_hdr, hytag_t *hytag, uint8_t direction)
{
    struct ether_addr src_mac;
    struct ether_addr dst_mac;
    if ( NULL == eth_hdr || NULL == hytag )
    {
        BRET(E_PARAM);
    }

    if ( DIRECTION_DIFFERENT != direction &&  DIRECTION_SAME != direction)
    {
        BRET(E_PARAM);
    }

    if (DIRECTION_DIFFERENT == direction )
    {

        ether_addr_copy(&(eth_hdr->d_addr),&dst_mac);
        ether_addr_copy(&(eth_hdr->s_addr),&src_mac);

        ether_addr_copy(&src_mac, &(eth_hdr->d_addr));
        ether_addr_copy(&dst_mac, &(eth_hdr->s_addr));


	if(ads_mac_enable[0])
	{
		memcpy(eth_hdr->d_addr.addr_bytes, ads_mac[0], 6);	
	}
	if(ads_mac_enable[1])
	{
		memcpy(eth_hdr->s_addr.addr_bytes, ads_mac[1], 6);	
	}

    }

#if 0	
    eth_hdr->s_addr.addr_bytes[0] = 0x90;
    eth_hdr->s_addr.addr_bytes[1] = 0xe2;
    eth_hdr->s_addr.addr_bytes[2] = 0xba;
    eth_hdr->s_addr.addr_bytes[3] = 0x19;
    eth_hdr->s_addr.addr_bytes[4] = 0x5b;
    eth_hdr->s_addr.addr_bytes[5] = 0x58;  



    eth_hdr->d_addr.addr_bytes[0] = 0x00;
    eth_hdr->d_addr.addr_bytes[1] = 0x1d;
    eth_hdr->d_addr.addr_bytes[2] = 0x71;
    eth_hdr->d_addr.addr_bytes[3] = 0xa6;
    eth_hdr->d_addr.addr_bytes[4] = 0x91;
    eth_hdr->d_addr.addr_bytes[5] = 0xc2;

#endif

    return E_SUCCESS;
}







berr
redirect_302_response_generator(void *ptr, hytag_t *hytag, char *url)
{
    int rv;
    char *http_head = NULL;
    struct tcp_hdr *tcp_hdr = NULL;
    struct ipv4_hdr *ip_hdr = NULL;
    struct ether_hdr *eth_hdr = NULL;
    uint8_t direction = DIRECTION_DIFFERENT;

	CYCLE_INIT(0);
    if ( NULL == ptr || NULL == hytag )
    {
        BRET(E_PARAM);
    }

    /* l4 first than l5, because update l4 need use l5 old len */
    /* l4 switch */
	CYCLE_START();
    tcp_hdr = (struct tcp_hdr *)(((char *)ptr) + hytag->l4_offset);
    rv = ads_tcp_head_modify(tcp_hdr, hytag, direction);
	CYCLE_END();
		
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }

    /* l5 fill */
    debug("old l5_len(%d)", hytag->l5_len);
    http_head = ((char *)ptr) + hytag->l5_offset;

	CYCLE_START();

    rv = ads_http_302_fill(http_head, hytag, url);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
    }
	CYCLE_END();

    debug("new l5_len(%d)", hytag->l5_len);

    /* l3 switch */
    ip_hdr = (struct ipv4_hdr *)(((char *)ptr) + hytag->l3_offset);
	CYCLE_START();

    rv = ads_ip_head_modify(ip_hdr, hytag, direction);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();


	CYCLE_START();

    /* tcp checksum update*/
    tcp_hdr->cksum = 0;
    tcp_hdr->cksum =  ads_tcpudp_cksum(ip_hdr, (void *)tcp_hdr);
    debug("tcp chsum(0x%x)", ntohs(tcp_hdr->cksum));
	CYCLE_END();

	CYCLE_START();

    /* ip checksum update */ 
    rv = ads_ipv4_cksum_update(ip_hdr);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();

	CYCLE_START();

    /* l2 switch */
    eth_hdr = (struct ether_hdr *)(((char *)ptr) + hytag->l2_offset);
    rv = ads_eth_head_modify(eth_hdr, hytag, direction);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();

    hytag->data_len = hytag->l5_offset - hytag->l2_offset + hytag->l5_len;
    return E_SUCCESS;
}


berr
ads_response_head_generator(void *ptr, hytag_t *hytag)
{
    int rv;
    char *http_head = NULL;
    struct tcp_hdr *tcp_hdr = NULL;
    struct ipv4_hdr *ip_hdr = NULL;
    struct ether_hdr *eth_hdr = NULL;
    uint8_t direction = DIRECTION_DIFFERENT;

	CYCLE_INIT(0);
    if ( NULL == ptr || NULL == hytag )
    {
        BRET(E_PARAM);
    }

    /* l4 first than l5, because update l4 need use l5 old len */
    /* l4 switch */
	CYCLE_START();
    tcp_hdr = (struct tcp_hdr *)(((char *)ptr) + hytag->l4_offset);
    rv = ads_tcp_head_modify(tcp_hdr, hytag, direction);
	CYCLE_END();
		
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }

    /* l5 fill */
    debug("old l5_len(%d)", hytag->l5_len);
    http_head = ((char *)ptr) + hytag->l5_offset;

	CYCLE_START();
    rv = ads_http_ok_head_fill(http_head, hytag);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
    }
	CYCLE_END();

    debug("new l5_len(%d)", hytag->l5_len);

    /* l3 switch */
    ip_hdr = (struct ipv4_hdr *)(((char *)ptr) + hytag->l3_offset);
	CYCLE_START();

    rv = ads_ip_head_modify(ip_hdr, hytag, direction);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();


	CYCLE_START();

    /* tcp checksum update*/
    tcp_hdr->cksum = 0;
    tcp_hdr->cksum =  ads_tcpudp_cksum(ip_hdr, (void *)tcp_hdr);
    debug("tcp chsum(0x%x)", ntohs(tcp_hdr->cksum));
	CYCLE_END();

	CYCLE_START();

    /* ip checksum update */ 
    rv = ads_ipv4_cksum_update(ip_hdr);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();

	CYCLE_START();

    /* l2 switch */
    eth_hdr = (struct ether_hdr *)(((char *)ptr) + hytag->l2_offset);
    rv = ads_eth_head_modify(eth_hdr, hytag, direction);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();

    hytag->data_len = hytag->l5_offset - hytag->l2_offset + hytag->l5_len;
    return E_SUCCESS;
}

    berr
ads_response_content_generator(void *ptr, hytag_t *hytag)
{
    int rv;
    char *http_head = NULL;
    struct tcp_hdr *tcp_hdr = NULL;
    struct ipv4_hdr *ip_hdr = NULL;
    struct ether_hdr *eth_hdr = NULL;
    uint8_t direction = DIRECTION_SAME;

    if ( NULL == ptr || NULL == hytag )
    {
        BRET(E_PARAM);
    }


	CYCLE_INIT(1);
    /* l4 first than l5, because update l4 need use l5 old len */
    /* l4 switch */
	CYCLE_START();
    tcp_hdr = (struct tcp_hdr *)(((char *)ptr) + hytag->l4_offset);
    rv = ads_tcp_head_modify(tcp_hdr, hytag, direction);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();

    /* l5 fill */
    debug("old l5_len(%d)", hytag->l5_len);
    http_head = ((char *)ptr) + hytag->l5_offset;
	CYCLE_START();	
    rv = ads_http_content_fill(http_head, hytag);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }

    debug("new l5_len(%d)", hytag->l5_len);

	CYCLE_END();

    /* l3 switch */
    ip_hdr = (struct ipv4_hdr *)(((char *)ptr) + hytag->l3_offset);
	CYCLE_START();

    rv = ads_ip_head_modify(ip_hdr, hytag, direction);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();

	CYCLE_START();

    /* tcp checksum update*/
    tcp_hdr->cksum = 0;
    tcp_hdr->cksum =  ads_tcpudp_cksum(ip_hdr, (void *)tcp_hdr);
    debug("tcp chsum(0x%x)", ntohl(tcp_hdr->cksum));
	CYCLE_END();


	CYCLE_START();

    /* ip checksum update */ 
    rv = ads_ipv4_cksum_update( ip_hdr);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();
	
	CYCLE_START();

    /* l2 switch */
    eth_hdr = (struct ether_hdr *)(((char *)ptr) + hytag->l2_offset);
    rv = ads_eth_head_modify(eth_hdr, hytag, direction);
    if (rv)
    {
        printf("%s,%d, rv(%d)\n", __func__, __LINE__, rv);
        return rv;
    }
	CYCLE_END();

    hytag->data_len = hytag->l5_offset - hytag->l2_offset + hytag->l5_len;
    return E_SUCCESS;
}

