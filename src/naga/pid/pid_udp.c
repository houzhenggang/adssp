#include "pid.h"


berr pid_udp(struct pbuf *p, hytag_t *hytag, int inner_outer)
{

	struct udp_hdr *udphdr = NULL;
	if(check_pbuf_len(p, UDP_HLEN))
	{
        if(inner_outer == HEADER_OUTER)
            pid_incr_count(OUTERL4_HD);//drop packet and incr counter, then return;
        else
            pid_incr_count(INNERL4_HD);
		BRET(E_EXCEED);
	}

	PBUF_CUR_FORMAT(struct udp_hdr *, udphdr, p);
	
	if(inner_outer == HEADER_OUTER)
	{
		/* convert src and dest ports to host byte order */
	  	hytag->outer_srcport= ntohs(udphdr->src);
	  	hytag->outer_dstport= ntohs(udphdr->dest);
        pid_incr_count(OUTERL4_UDP);
	}
	else
	{
		/* convert src and dest ports to host byte order */
	  	hytag->inner_srcport= ntohs(udphdr->src);
	  	hytag->inner_dstport= ntohs(udphdr->dest);	
        pid_incr_count(INNERL4_UDP);
	}


	if (likely(inner_outer == HEADER_OUTER))
	{
        if( hytag->outer_srcport == UDP_PORT_GTP_U
		            || hytag->outer_dstport == UDP_PORT_GTP_U)//gtp-u        
        {
			UPDATE_PBUF_OFFSET(p, UDP_HLEN);
			return pid_gtpu(p, hytag);
        }
	    else if(hytag->outer_srcport == UDP_PORT_GTPV2_C
		                || hytag->outer_dstport == UDP_PORT_GTPV2_C)//gtpv2c
        {
			UPDATE_PBUF_OFFSET(p, UDP_HLEN);
			return pid_gtpv2c(p, hytag);
        }
        else
        {
            
        }
    }
	
    
	return E_SUCCESS;
}


