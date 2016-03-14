#include "acr_account_rule.h"
#include "bts_hashtable.h"
#include "bts_util.h"

bts_hashtable_t acr_account_rule;

uint32_t
account_rule_hash_func(void *data)
{
    acr_account_rule_t *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }

    entry = (acr_account_rule_t *) data; 

    return bts_hash(entry->account, strlen(entry->account));
}

int
account_rule_cmp_func(void *d1, void *d2)
{
    acr_account_rule_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (acr_account_rule_t *) d1;
    e2 = (acr_account_rule_t *) d2;

    return bts_str_cmp((void *)(e1->account), (void *)(e2->account));
}

berr 
acr_account_rule_init(uint32_t number)
{
    return bts_hashtable_init(&acr_account_rule, number, account_rule_hash_func, account_rule_cmp_func, NULL);
}

acr_account_rule_t*
acr_account_rule_lookup(void *account)
{
    return (acr_account_rule_t *) bts_hashtable_lookup(&acr_account_rule,  account);
}

berr
acr_account_rule_add(acr_account_rule_t *entry)
{
    return bts_hashtable_add(&acr_account_rule, (void *) entry);
}

berr
acr_account_rule_del(acr_account_rule_t *entry)
{
    return bts_hashtable_del(&acr_account_rule, (void *) entry);
}

berr
acr_account_rule_clear(void)
{
    //return bts_hashtable_clear(tab);
    return bts_hashtable_del_all(&acr_account_rule);
}

/* End of file */
