#include "pid.h"
#include <unistd.h>

berr pid_innerip(struct pbuf *p,  hytag_t * hytag)
{


	struct ip4_hdr *iphdr;
	uint16_t  iphdr_hlen;// iphdr_len;

	if(check_pbuf_len(p, IP_HD_LEN))
	{
        
		pid_incr_count(INNERL3_HD);//drop packet and incr counter, then return;
		//PRINTF_PKT(p);
        //sleep(100);
		BRET(E_EXCEED);
	}

    
	PBUF_CUR_FORMAT(struct ip4_hdr *, iphdr, p);

    switch(IPH_V(iphdr))
    {
        case 4:
            break;
        case 6:
            pid_incr_count(INNERL3_IPV6);
            //goto ipv6
            BRET(E_COMPARE);
            break;
        default:
            pid_incr_count(INNERL3_NOIP);
            BRET(E_COMPARE);
            break;
    }
    

	
  	/* obtain IP header length in number of 32-bit words */
  	iphdr_hlen = IPH_HL(iphdr);
	/* calculate IP header length in bytes */
  	iphdr_hlen *= 4;
	/* obtain ip length in bytes */
  	//iphdr_len = ntohs(IPH_LEN(iphdr));

	pid_incr_count(INNERL3_IPV4);
	hytag->inner_dstip4 = ntohl(iphdr->dest);
	hytag->inner_srcip4 = ntohl(iphdr->src);

	UPDATE_PBUF_OFFSET(p, iphdr_hlen);	
	
	return pid_L4(p, hytag, IPH_PROTO(iphdr), HEADER_INNER);
		
	
}
