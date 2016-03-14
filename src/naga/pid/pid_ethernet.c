#include "pid.h"


#define SIZEOF_ETH_HDR 	(14)
#define ETHERTYPE_IP4 		0x0800
#define ETHERTYPE_IP6 		0x86dd
#define ETHERTYPE_VLAN		0x8100


berr pid_ethernet(struct pbuf *p, hytag_t *hytag)
{

	struct eth_vlan_hdr *eth_v_header = NULL;
    struct eth_hdr * eth_header = NULL;
	uint16_t type = 0;
	uint16_t len = 0;

	
	if(check_pbuf_len(p, SIZEOF_ETH_HDR))
	{
		pid_incr_count(HEADER_UNEXPECT);
		BRET(E_EXCEED);
	}
	
	PBUF_CUR_FORMAT(struct eth_hdr *, eth_header, p);

    hytag->l2_offset = p->ptr_offset;
	type = eth_header->ethertype;
	len += SIZEOF_ETH_HDR; 
	DEBUG_PRINTF("DstMac=%02x:%02x:%02x:%02x:%02x:%02x\n",
                     eth_header->dest.addr[0], eth_header->dest.addr[1], eth_header->dest.addr[2],
                     eth_header->dest.addr[3], eth_header->dest.addr[4], eth_header->dest.addr[5]);
    DEBUG_PRINTF("SrcMac=%02x:%02x:%02x:%02x:%02x:%02x\n", 
                      eth_header->src.addr[0], eth_header->src.addr[1], eth_header->src.addr[2],
                     eth_header->src.addr[3], eth_header->src.addr[4], eth_header->src.addr[5]);
    DEBUG_PRINTF("Ethertype=%04x\n", ntohs(type)); 

    
	/*skip vlan*/
	if(type == htons(ETHERTYPE_VLAN))
	{
		pid_incr_count(VLAN_PKTS);
        //printf("len0 = %d\n", len);
		len += 2;
        //printf("len1 = %d\n", len);
		do{
			eth_v_header = (struct eth_vlan_hdr*)PBUF_PTR(p, len);	
		}while(eth_v_header->v_type== htons(ETHERTYPE_VLAN) && (len += 4));
        //printf("len2 = %d\n", len);
		type = eth_v_header->v_type;
        len += 2;
	}

    UPDATE_PBUF_OFFSET(p, len);	
    hytag->l3_offset = p->ptr_offset;
    
	switch( ntohs(type) )
	{
		case ETHERTYPE_IP4:
			pid_incr_count(OUTERL3_IPV4);
            return pid_outerip4(p, hytag);
		case ETHERTYPE_IP6:
			pid_incr_count(OUTERL3_IPV6);
            break;
		default:
			pid_incr_count(OUTERL3_NOIP);
            break;
	}
	return E_SUCCESS;
}




