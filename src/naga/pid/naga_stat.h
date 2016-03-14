#ifndef _NAGA_STAT_H
#define _NAGA_STAT_H

#include "rte_atomic.h"
#include "bts_cnt.h"
typedef rte_atomic64_t pid_atomic64_cnt_t ;

struct pid_port_stat_s
{
    pid_atomic64_cnt_t  stat;
    
#if 0
    pid_atomic64_cnt_t  st_outerl3_ip6;
    pid_atomic64_cnt_t  st_outerl3_noip;
    pid_atomic64_cnt_t  st_outerl4_tcp;
    pid_atomic64_cnt_t  st_outerl4_udp;
    pid_atomic64_cnt_t  st_outerl4_other;

    pid_atomic64_cnt_t  st_innerl3_ip4;
    pid_atomic64_cnt_t  st_innerl3_ip6;
    pid_atomic64_cnt_t  st_innerl3_noip;    
    pid_atomic64_cnt_t  st_innerl4_tcp;
    pid_atomic64_cnt_t  st_innerl4_udp;
    pid_atomic64_cnt_t  st_innerl4_other;
    pid_atomic64_cnt_t  st_gtp_u;
    pid_atomic64_cnt_t  st_inner_and_outer_l5_http;
#endif    
}__rte_cache_aligned;

# if 0
enum pid_pkt_type
{
    OUTERL3_HD,   /*Head Err, length not compare or infomation err*/
    OUTERL3_IPV4,
    OUTERL3_IPV6,
    OUTERL3_NOIP,
    
    OUTERL4_HD,  
    OUTERL4_TCP,
    OUTERL4_UDP,
    OUTERL4_SCTP,
    OUTERL4_OTHER,

    INNERL3_HD,
    INNERL3_IPV4,
    INNERL3_IPV6,
    INNERL3_NOIP,

    INNERL4_HD,
    INNERL4_TCP,
    INNERL4_UDP,
    INNERL4_SCTP,    
    INNERL4_OTHER,

    GTP_U_HD,
    GTP_U,
    GTP_U_NOPAYLOAD,
    
    GTPC_HD,
    GTPV1_C,
    GTPV2_C,
    
    APP_HTTP,
    APP_OTHER,
    ERR_PACKET,
    HEADER_UNEXPECT,

    PID_STAT_MAX_IDX,
};

#endif

//void pid_incr_count(enum pid_pkt_type pid_type) ;
void gpid_stats_init(void);

void printf_pid_stat(void);



#define pid_incr_count cnt_inc

#endif
