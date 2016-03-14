#ifndef __PID_UDP_H
#define __PID_UDP_H


#define UDP_HLEN 8

struct udp_hdr {
  uint16_t src;
  uint16_t dest;  /* src/dest UDP ports */
  uint16_t len;
  uint16_t chksum;
};

#define  GREV1  1
#define  GREV2  2
#define  GRE_GTP 1

#define UDP_PORT_GTP_U	    2152
#define UDP_PORT_GTPV2_C    2123

berr pid_udp(struct pbuf *p, hytag_t *hytag, int inner_outer);

#endif