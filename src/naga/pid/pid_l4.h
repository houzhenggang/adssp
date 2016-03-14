#ifndef __PID_L4_H
#define __PID_L4_H

#include "pid_tcp.h"
#include "pid_udp.h"

#define IP_PROTO_ICMP    1
#define IP_PROTO_IGMP    2
#define IP_PROTO_UDP     17
#define IP_PROTO_UDPLITE 136
#define IP_PROTO_TCP     6
#define IP_PROTO_STCP    132

berr pid_L4(struct pbuf *p, hytag_t *hytag,  uint8_t protocol, int outer_inner);

#endif
