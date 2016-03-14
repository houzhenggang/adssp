#include "naga_debug.h"
#include "bts_log.h"

#define IP4_STR_SZ 20
#define IP6_STR_SZ 128

#define HYTAG_U16_DUMP(_tag, _field) { \
    printf("HYTAG[%d] %s: %.4d(0x%.4x)\n", rte_lcore_id(), #_field, _tag->_field, _tag->_field); \
}

#define HYTAG_U32_DUMP(_tag, _field) { \
    printf("HYTAG[%d] %s: %.8d(0x%.8x)\n", rte_lcore_id(), #_field, _tag->_field, _tag->_field); \
}

#define HYTAG_IP4_DUMP(_tag, _field) { \
    char _ipstr[IP4_STR_SZ]; \
    bts_ip_string(_tag->_field, _ipstr); \
    printf("HYTAG[%d] %s: %s\n", rte_lcore_id(), #_field, _ipstr); \
}

#define HYTAG_IP6_DUMP(_tag, _field) { \
    char _ipstr[IP6_STR_SZ]; \
    ip6_string(_tag->_field, _ipstr); \
    printf("HYTAG[%d] %s: %s\n", rte_lcore_id(), #_field, _ipstr); \
}

#define HYTAG_STR_DUMP(_tag, _field) { \
    printf("HYTAG[%d] %s: %s\n", rte_lcore_id(), #_field, _tag->_field); \
}

void hytag_print(hytag_t *tag)
{
#if 1
    HYTAG_IP4_DUMP(tag, outer_srcip4);
    HYTAG_IP4_DUMP(tag, outer_dstip4);
    HYTAG_IP4_DUMP(tag, inner_srcip4);
    HYTAG_IP4_DUMP(tag, inner_dstip4);    
#if 0
    HYTAG_IP6_DUMP(tag, outer_srcip6);
    HYTAG_IP6_DUMP(tag, outer_dstip6);
    HYTAG_IP6_DUMP(tag, inner_srcip6);
    HYTAG_IP6_DUMP(tag, inner_dstip6);
#endif

    HYTAG_U32_DUMP(tag, teid);

    HYTAG_U16_DUMP(tag, outer_srcport);
    HYTAG_U16_DUMP(tag, outer_dstport);
    HYTAG_U16_DUMP(tag, inner_srcport);
    HYTAG_U16_DUMP(tag, inner_dstport);

    HYTAG_STR_DUMP(tag, url);
#endif    
}


void hytag_log(hytag_t *tag)
{
	if(1)
	{
		if((APP_TYPE_HTTP_GET_OR_POST == tag->app_type) && (tag->match & 0xc))
		{
                        //printf("hijack match is : %d\n", tag->match);
			bts_zlog(LOG_ALERT, "%d.%d.%d.%d %d.%d.%d.%d %d %d %d %d %s", 
		                    (tag->outer_srcip4 >>24) &0xff,
		                    (tag->outer_srcip4 >>16) &0xff,
		                    (tag->outer_srcip4 >>8) &0xff,
		                    (tag->outer_srcip4) &0xff,
		                    (tag->outer_dstip4 >>24) &0xff,
		                    (tag->outer_dstip4 >>16) &0xff,
		                    (tag->outer_dstip4 >>8) &0xff,
		                    (tag->outer_dstip4) &0xff,
		                    tag->outer_srcport,
		                    tag->outer_dstport,
		                    tag->match,
                                    tag->hijack_rule_id,
		                    tag->url);
		}
	}
	else
	{
		if(APP_TYPE_HTTP_GET_OR_POST == tag->app_type 
		&&	(ACT_LOG == (tag->acl.actions & ACT_LOG)))
		{
			bts_zlog(LOG_ALERT, "%d.%d.%d.%d|%d|%d.%d.%d.%d|%d|%d|%d|%s|AD<%s>", 
		                    (tag->outer_srcip4 >>24) &0xff,
		                    (tag->outer_srcip4 >>16) &0xff,
		                    (tag->outer_srcip4 >>8) &0xff,
		                    (tag->outer_srcip4) &0xff,
		                     tag->outer_srcport,
		                    (tag->outer_dstip4 >>24) &0xff,
		                    (tag->outer_dstip4 >>16) &0xff,
		                    (tag->outer_dstip4 >>8) &0xff,
		                    (tag->outer_dstip4) &0xff,
		                    tag->outer_dstport,
		                    tag->outer_protocol,
		                    tag->snet_hit_id,
		                    tag->host, tag->ad_act == AD_SUCCESS ? "P":"N");        	
		}
		if(tag->pushed_second_assert)
		{

			bts_zlog(LOG_ALERT, "%d.%d.%d.%d|%d|%d.%d.%d.%d|%d|%d|%d|%s|AD<S>", 
		                    (tag->outer_srcip4 >>24) &0xff,
		                    (tag->outer_srcip4 >>16) &0xff,
		                    (tag->outer_srcip4 >>8) &0xff,
		                    (tag->outer_srcip4) &0xff,
		                    tag->outer_srcport,
		                    (tag->outer_dstip4 >>24) &0xff,
		                    (tag->outer_dstip4 >>16) &0xff,
		                    (tag->outer_dstip4 >>8) &0xff,
		                    (tag->outer_dstip4) &0xff,
		                    tag->outer_dstport,
		                    tag->outer_protocol,
		                    (tag->snet_hit_id),
		                    tag->host);        	
		}					
		
	}	
}

/* End of file */
