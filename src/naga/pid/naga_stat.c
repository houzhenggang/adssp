#include "pid.h"
#include <string.h>



# if 0
struct pid_port_stat_s gpid_stats[PID_STAT_MAX_IDX];
//extern struct pid_port_stat_s gpid_stats;

void gpid_stats_init(void)
{
    memset(gpid_stats, 0x0, sizeof(gpid_stats));
    return ;
}

void printf_pid_stat(void)
{
    printf("Pid statistics ===============================\n");
    printf("Unhandle packet ---------------------------------\n");
    printf("Error  packet:      %-20ld\n", (uint64_t)gpid_stats[ERR_PACKET].stat.cnt);
    //printf("Header expect:      %-20ld\n", (uint64_t)gpid_stats[HEADER_UNEXPECT].stat.cnt);     
    printf("\n----------------- OuterL3 -------------------\n");
    printf("OuterL3 ipv4:       %-20ld\n", (uint64_t)gpid_stats[OUTERL3_IPV4].stat.cnt); 
    printf("OuterL3 ipv6:       %-20ld\n", (uint64_t)gpid_stats[OUTERL3_IPV6].stat.cnt);      
    printf("OuterL3 noip:       %-20ld\n", (uint64_t)gpid_stats[OUTERL3_NOIP].stat.cnt);
    printf("OuterL3 Header Err: %-20ld\n", (uint64_t)gpid_stats[OUTERL3_HD].stat.cnt);
    printf("\n----------------- OuterL4 ---------------------\n");
    printf("OuterL4 tcp:        %-20ld\n",  (uint64_t)gpid_stats[OUTERL4_TCP].stat.cnt);
    printf("OuterL4 udp:        %-20ld\n",  (uint64_t)gpid_stats[OUTERL4_UDP].stat.cnt);
    printf("OuterL4 sctp:       %-20ld\n",  (uint64_t)gpid_stats[OUTERL4_SCTP].stat.cnt);    
    printf("OuterL4 other:      %-20ld\n",  (uint64_t)gpid_stats[OUTERL4_OTHER].stat.cnt);
    printf("OuterL4 Header Err: %-20ld\n", (uint64_t)gpid_stats[OUTERL4_HD].stat.cnt);
    printf("\n------------------ GTP ------------------------\n");
    printf("Gtp-U:              %-20ld\n",  (uint64_t)gpid_stats[GTP_U].stat.cnt);
    printf("Gtp-U_No_Payload:   %-20ld\n",  (uint64_t)gpid_stats[GTP_U_NOPAYLOAD].stat.cnt);  
    printf("Gtpv2-C:            %-20ld\n",  (uint64_t)gpid_stats[GTPV2_C].stat.cnt);  
    printf("Gtp-u Header Err:   %-20ld\n", (uint64_t)gpid_stats[GTP_U_HD].stat.cnt);    
    printf("\n----------------- InnerL3 ---------------------\n");
    printf("InnerL3 ipv4:       %-20ld\n", (uint64_t)gpid_stats[INNERL3_IPV4].stat.cnt); 
    printf("InnerL3 ipv6:       %-20ld\n", (uint64_t)gpid_stats[INNERL3_IPV6].stat.cnt);      
    printf("InnerL3 noip:       %-20ld\n", (uint64_t)gpid_stats[INNERL3_NOIP].stat.cnt);
    printf("InnerL3 Header Err: %-20ld\n", (uint64_t)gpid_stats[INNERL3_HD].stat.cnt);      
    printf("\n----------------- InnerL4 ---------------------\n");
    printf("InnerL4 tcp:        %-20ld\n",  (uint64_t)gpid_stats[INNERL4_TCP].stat.cnt);
    printf("InnerL4 udp:        %-20ld\n",  (uint64_t)gpid_stats[INNERL4_UDP].stat.cnt);
    printf("OuterL4 sctp:       %-20ld\n",  (uint64_t)gpid_stats[INNERL4_SCTP].stat.cnt);     
    printf("InnerL4 other:      %-20ld\n",  (uint64_t)gpid_stats[INNERL4_OTHER].stat.cnt); 
    printf("InnerL4 Header Err: %-20ld\n", (uint64_t)gpid_stats[INNERL4_HD].stat.cnt);   
   
    printf("\n---------------- Application -------------------\n");
    printf("Http:               %-20ld\n",  (uint64_t)gpid_stats[APP_HTTP].stat.cnt); 
    printf("Other:              %-20ld\n",  (uint64_t)gpid_stats[APP_OTHER].stat.cnt); 
    printf("==============================================\n");    
}




inline void pid_incr_count(enum pid_pkt_type pid_type) 
{
    if (likely(pid_type < PID_STAT_MAX_IDX))
    {
          return  rte_atomic64_inc(&(gpid_stats[pid_type].stat));      
    }
    else
    {
        ;
    }
/*    
    switch(pid_type)
    {
        case OUTERL3_IPV4:
               return  rte_atomic64_inc(&gpid_stats.st_outerl3_ip4);            
        case OUTERL3_IPV6:
               return  rte_atomic64_inc(&gpid_stats.st_outerl3_ip6);
        case OUTERL3_NOIP:
               return  rte_atomic64_inc(&gpid_stats.st_outerl3_noip);             
        case OUTERL4_TCP:
               return rte_atomic64_inc(&gpid_stats.st_outerl4_tcp);  
        case OUTERL4_UDP:
               return rte_atomic64_inc(&gpid_stats.st_outerl4_udp);
        case OUTERL4_OTHER:
               return rte_atomic64_inc(&gpid_stats.st_outerl4_other);

        case INNERL3_IPV4:
                return rte_atomic64_inc(&gpid_stats.st_innerl3_ip4);
        case INNERL3_IPV6:
                return rte_atomic64_inc(&gpid_stats.st_innerl3_ip6);
        case INNERL3_NOIP:
                return rte_atomic64_inc(&gpid_stats.st_innerl3_noip);
        case INNERL4_TCP:
                return rte_atomic64_inc(&gpid_stats.st_innerl4_tcp);  
        case INNERL4_UDP:
                return rte_atomic64_inc(&gpid_stats.st_innerl4_udp);
        case INNERL4_OTHER:
                return rte_atomic64_inc(&gpid_stats.st_innerl4_other);
        case GTP_U:
                return rte_atomic64_inc(&gpid_stats.st_gtp_u);
        case HTTP:
                return rte_atomic64_inc(&gpid_stats.st_inner_and_outer_l5_http);
    }
 */   
    return;
}
#endif
