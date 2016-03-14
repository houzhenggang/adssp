#ifndef __PID_H_
#define __PID_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <rte_branch_prediction.h>

#include "boots.h"
#include "bts_debug.h"

#include "naga_types.h"

static inline berr check_pbuf_len(struct pbuf *p, int incr_len)
{
	if(p->ptr_offset + incr_len > p->len)
	{
	 	BRET(E_EXCEED);
	}
	else
	{
		return E_SUCCESS;
	}
}

#define UPDATE_PBUF_OFFSET(_pbuf, _off) (_pbuf->ptr_offset+=_off)

#define PBUF_CUR_FORMAT(_type, _ptr, _p) (_ptr = (_type)(_p->ptr_offset+(char *)_p->ptr))
#define PBUF_OFF_FORMAT(_type, _ptr, _p,_len) (_ptr = (_type)(_len+(char *)_p->ptr))


#define PBUF_PTR(_p, _len)  (_len+(char *)_p->ptr)

#define PRINTF_PKT(_p) \
    do{ \
         int _i; \
         for(_i=0; _i<p->len; _i++)\
         {\
            if(_i%15==0)\
            {\
                printf("\n");\
            }\
            printf("%02x ", *((uint8_t *)p->ptr + _i) );\
         }\
     }while(0)


#define PBUF_DUMP(_p, _len) \
do{\
    int i = 0;\
    uint8_t *ot = NULL;\
    PBUF_CUR_FORMAT(uint8_t *, ot, _p);\
    for(i = 0; i < _len; i++) \
    {\
        if (0 == i%7)\
            printf("\n");\
        printf("%.2x ", ot[i]);\
    }\
    printf("\n");\
}while(0)


//#define BYTE_ORDER  LITTLE_ENDIAN
#define __LITTLE_ENDIAN_BITFIELD

#define HEADER_OUTER  0
#define HEADER_INNER  1



//#define DEBUG_ENABLE
#ifdef  DEBUG_ENABLE

#define DEBUG_PRINTF(format, args...) printf(format, ##args) 
#else
#define DEBUG_PRINTF(format, ...)

#endif

#include "pid_ethernet.h"
#include "pid_outerIp4.h"
#include "pid_l4.h"
#include "pid_innerIp4.h"
#include "pid_gre.h"
#include "pid_http.h"
#include "rte_mbuf.h"
#include "naga_stat.h"

//berr naga_pid_dpdk(struct rte_mbuf *m);
berr naga_pid(hytag_t *hytag);


#endif

