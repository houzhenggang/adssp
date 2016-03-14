#ifndef __PID_ETHERNET_H
#define __PID_ETHERNET_H

#include <sys/types.h>
#include "pid.h"

#define ETHARP_HWADDR_LEN     6

struct eth_addr {
 	uint8_t addr[ETHARP_HWADDR_LEN];
};


struct eth_hdr {
	struct eth_addr dest;
   	struct eth_addr src;
  	uint16_t ethertype;
} ;


struct eth_vlan_hdr 
{
 	uint16_t v_type;
    uint16_t v_val;	
};


berr pid_ethernet(struct pbuf *p, hytag_t *hytag);




#endif
