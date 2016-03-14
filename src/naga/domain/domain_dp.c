#include "domain.h"
#include "domain_dp.h"
#include "naga_types.h"
#include "bts_debug.h"

#include "boots.h"
#include "bts_cnt.h"

#define MAX_HOST_RULE_NUM 100000000

extern uint32_t domin_default_action;

berr naga_domain(hytag_t *tag)
{
    dmr_t* rule = NULL;

	

    if (NULL == tag)
    {
        BRET(E_FAIL);
    }

	if( APP_TYPE_HTTP_GET_OR_POST != tag->app_type)
    {
        return E_SUCCESS;
    }

    rule = domain_get((char *)tag->host);

	
    if (NULL == rule)
    {
    	//printf("dssssss\n");
		rule = malloc(sizeof(dmr_t));
		if (NULL != rule)
		{
			memset(rule, 0x0, sizeof(dmr_t));
		}
		domain_add(rule);
		ACL_HIT(rule->acl);
		
    }
    else
    {
        ACL_HIT(rule->acl);

        if(tag->pushed_second_assert)
        {
            ACL_PUSHED_ASSERT_HIT(rule->acl);
        } 
    }
    BRET(E_SUCCESS);
}

void domain_dp_init(void)
{
	berr rv;
	rv = domain_init(MAX_HOST_RULE_NUM);
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
