/**************************************************************
* Copyright (C) 2014-2015 All rights reserved.
* @Version: 1.0
* @Created: 2015-10-15 07:06
* @Author: SamLiu - shan3275@gmail.com
* @Description: 
*
* @History: 
**************************************************************/
#include "bts_cnt.h"
#include "dnet.h"

extern uint32_t dnetseg_default_action;
berr dnetseg_dp_match(uint32_t ip, dnet_t **rule)
{
    int i;
    int rv = E_SUCCESS;
	dnetseg_t *seg = NULL;

	seg = api_get_dnetseg_ptr();
	if ((NULL == seg)||(NULL == rule))
	{
		return E_FAIL;
	}
	
    for ( i = 0; i < DNETSEG__RULE_NUM_MAX; i++)
    {

		if(seg[i].effective == DNETSEG__RULE_UNEFFECTIVE)
		{
			continue;			
		}
		if ( (ip & seg[i].net.mask) == (seg[i].net.ip & seg[i].net.mask) )
		{
			*rule = &seg[i].net;
			return E_SUCCESS;
		}
		else
		{
			continue;
		}
		
    }
    return E_NULL;
}

berr dnet_dp_process(hytag_t *hytag)
{
    int rv;
	dnet_t *rule = NULL;

    if ( NULL == hytag )
    {
        return E_PARAM;
    }

    /*add recv statistics */
    cnt_inc(DNET_PKTS);

    /*check protocol type*/
    if ( hytag->app_type != APP_TYPE_HTTP_GET_OR_POST)
    {
        /* add statistics */
        cnt_inc(DNET_UNURLPKTS);
        return E_SUCCESS;
    }

    /* add IP_UDP_GTP_IP_URL packet statistics */
    cnt_inc(DNET_URLPKTS);

    /* IP_UDP_GTP_IP_URL packet process */
    rv = dnetseg_dp_match(hytag->outer_dstip4, &rule);
    
    if((E_SUCCESS != rv)|| (NULL == rule))
    {
        /*add not match statistics */
        cnt_inc(DNET_UNMATCHPKTS);
        hytag->match &= 0xfffe;
		HYTAG_ACL_SET(hytag->acl, dnetseg_default_action);
    }
    else
    {
        /* add match statistics */
        cnt_inc(DNET_MATCHPKTS);
        hytag->match |= 1;


		ACL_HIT(rule->acl);


		if (0 == (hytag->acl.actions & ACT_DROP))
	
		{
			ACL_PRE_NOT_DROP_HIT(rule->acl);
		}
        if(hytag->pushed_second_assert)
        {
            ACL_PUSHED_ASSERT_HIT(rule->acl);
        }
        
		HYTAG_ACL_MERGE(hytag->acl, rule->acl);
    }
    return E_SUCCESS; 
}
