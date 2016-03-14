#include "pid.h"


berr pid_outerip4(struct pbuf *p,  hytag_t * hytag)
{
	struct ip4_hdr *iphdr = NULL;
	uint16_t  iphdr_hlen;//, iphdr_len;


	if(check_pbuf_len(p, IP_HD_LEN))
	{
        /*drop packet and incr counter, return;*/
		pid_incr_count(OUTERL3_HD);
		BRET(E_EXCEED);
	}

    
    PBUF_CUR_FORMAT(struct ip4_hdr *, iphdr, p);


    if (IPH_V(iphdr) != 4) 
	{
		//incr err  
		
		pid_incr_count(OUTERL3_HD);
		BRET(E_COMPARE);
	}
	//pid_incr_count(OUTERL3_IPV4);
  	/* obtain IP header length in number of 32-bit words */
  	iphdr_hlen = IPH_HL(iphdr);
	/* calculate IP header length in bytes */
  	iphdr_hlen *= 4;
	/* obtain ip length in bytes */
  	//iphdr_len = ntohs(IPH_LEN(iphdr));

	hytag->outer_dstip4 = ntohl(iphdr->dest);
	hytag->outer_srcip4 = ntohl(iphdr->src);
    hytag->outer_protocol = IPH_PROTO(iphdr);
    hytag->total_len =  ntohs(IPH_LEN(iphdr));

	UPDATE_PBUF_OFFSET(p, iphdr_hlen);	
    hytag->l4_offset = p->ptr_offset;

    DEBUG_PRINTF("srcIp=0x%x, DstIp=0x%x, Iphd_len=%d\n",
                hytag->outer_dstip4,
                hytag->outer_srcip4,
                iphdr_hlen
                );


	return pid_L4(p, hytag, IPH_PROTO(iphdr), HEADER_OUTER);
		
}

