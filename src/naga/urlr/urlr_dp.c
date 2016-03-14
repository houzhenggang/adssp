#include "urlr.h"
#include "urlr_dp.h"
#include "naga_types.h"
#include "bts_debug.h"

#include "boots.h"
#include "bts_cnt.h"

#define MAX_URL_RULE_NUM 10000

extern uint32_t url_default_action;

berr naga_urlr(hytag_t *tag)
{
    urlr_t* rule = NULL;

    if (NULL == tag)
    {
        BRET(E_FAIL);
    }

	if( APP_TYPE_HTTP_GET_OR_POST != tag->app_type)
    {
        return E_SUCCESS;
    }
#if 0
	if (0 == (tag->acl.actions & ACT_PUSH))
	{
		return E_SUCCESS;
	}

	tag->acl.actions = tag->acl.actions & (~ACT_PUSH);
#endif	
//	CNT_INC(URLR_PKTS);
    tag->url_len= snprintf(tag->url, MAX_URL_LENGTH, "http://%s%s", tag->host, tag->uri);

    rule = urlr_get((char *)tag->url);

    if (NULL == rule)
    {
        //CNT_INC(URLR_RULE_UNMATCH);
		HYTAG_ACL_SET(tag->acl, url_default_action);
    }
    else
    {
       // CNT_INC(URLR_RULE_MATCH);
        ACL_HIT(rule->acl);


        if(tag->pushed_second_assert)
        {
            ACL_PUSHED_ASSERT_HIT(rule->acl);
        }
        //control by interval 

		if (0 == (tag->acl.actions & ACT_DROP))
	
		{
			 ACL_PRE_NOT_DROP_HIT(rule->acl);

            if(rule->acl.cnt.cnt % rule->interval == 0 )
            {
       
                HYTAG_ACL_MERGE(tag->acl, rule->acl);
            }
            else
            {
                HYTAG_ACL_SET(tag->acl,  ACT_DROP);                 
            }
                 
        }
    
  }

    BRET(E_SUCCESS);
}

void urlr_dp_init(void)
{
	berr rv;
	rv = urlr_init(MAX_URL_RULE_NUM);
	if (E_SUCCESS != rv)
	{
		printf("Domain rule init FAIL!\n");
	}
	else
	{
		printf("Domain rule init success!\n");
	}
	return;
}


/* End of file */ 
