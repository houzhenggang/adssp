//#include "naga_account_table.h"
#include "string.h"
#include "acr.h"
//#include "acr_account_table.h"
#include "naga_types.h"
#include "bts_debug.h"

#include "boots.h"
#include "bts_cnt.h"


extern uint32_t account_default_action;


#define MAX_ACCOUNT_RULE_NUM 10000


#if 0
void
acr_account_fill(hytag_t *tag, acr_account_rule_t *data)
{
    acr_account_entry_t *entry = NULL;

    entry = acr_account_table_lookup(tag->inner_srcip4);

    if (NULL == entry)
    {
        CNT_INC(ACR_TABLE_NOTFOUND);
        bts_ip_string(tag->inner_srcip4, tag->account);
    }
    else
    {
        CNT_INC(ACR_TABLE_FOUND);
        strcpy(tag->account, entry->account);
    }

	strcpy(data->account, tag->account);
	data->account_len = strlen(tag->account);
	
}
#endif

void acr_dp_init(void)
{
	berr rv;
    rv = acr_init(MAX_ACCOUNT_RULE_NUM);
	if (E_SUCCESS != rv)
	{
		printf("account rule init FAIL!\n");
	}
	else
	{
        printf("account rule init success!\n");
	}
	return;
}


/* End of file */
berr
naga_acr(hytag_t *tag)
{
    acr_t* rule = NULL;
    if (NULL == tag)
    {
		BRET(E_FAIL);
	}

	if( APP_TYPE_HTTP_GET_OR_POST != tag->app_type)
	{
		return E_SUCCESS;
	}

	
    CNT_INC(ACR_PKTS);

   	bts_ip_string(tag->outer_srcip4, tag->account);
    rule = acr_get(tag->account);

    if (NULL == rule)
    {
        CNT_INC(ACR_RULE_UNMATCH);
		HYTAG_ACL_SET(tag->acl, account_default_action);
    }
    else
    {
        CNT_INC(ACR_RULE_MATCH);
        ACL_HIT(rule->acl);
        HYTAG_ACL_MERGE(tag->acl, rule->acl);
    }

    BRET(E_SUCCESS);
}

/* End of file */
