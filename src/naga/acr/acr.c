#include "bts_hash.h"
#include "bts_hashtable.h"
#include "bts_linklist.h"
#include "bts_util.h"
#include "bts_debug.h"

#include "acr.h"

bts_hashtable_t acr_table;
uint32_t account_default_action = 0;


uint32_t 
acr_hash_func(void *data)
{
    acr_t *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }

    entry = (acr_t *) data; 

    return bts_hash(entry->account, entry->account_len);
}

int 
acr_cmp_func(void *d1, void *d2)
{
    acr_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (acr_t *) d1;
    e2 = (acr_t *) d2;

    return bts_str_cmp((void *)(e1->account), (void *)(e2->account));
}

berr
acr_init(uint32_t number)
{
    return bts_hashtable_init(&acr_table, number, acr_hash_func, acr_cmp_func, NULL);
}



acr_t*
acr_get(char *key)
{
	acr_t data;

	memset(&data, 0, sizeof(acr_t));
	data.account_len = strlen(key);
	memcpy(data.account, key, data.account_len);
    return (acr_t *)bts_hashtable_lookup(&acr_table, (void*) (&data));
}

berr
acr_add(acr_t *entry)
{
    return bts_hashtable_add(&acr_table, (void *) entry);
}


void
acr_clear_func(void *data, void *param)
{
    if (NULL == data)
    {
        return;

    }

    bts_hashtable_del(&acr_table, data);
}




berr
acr_del(char *account)
{
    acr_t *entry = NULL;

    entry = acr_get(account);
    if (NULL == entry)
    {
        BRET(E_FOUND);
    }

    return bts_hashtable_del(&acr_table, (void *) entry);
}

berr
acr_clear(void)
{
	#if 0
    return bts_hashtable_del_all(&acr_table);
	#else
	bts_hashtable_iter(&acr_table, acr_clear_func, NULL);
	return E_SUCCESS;
	#endif
}

void
acr_stat_clear_func(void *data, void *param)
{
    acr_t *acr = NULL;
    if (NULL == data)
    {
        return;

    }
    acr = (acr_t *) data;

    bts_atomic64_set(&acr->acl.cnt, 0); 
}

berr
acr_stat_clear(char *account)
{
    acr_t *acr = NULL;

    acr = acr_get(account);

    if (NULL == acr)
    {
        BRET(E_FOUND);
    }

    acr_stat_clear_func(acr, NULL);

    return E_SUCCESS;
}

berr
acr_stat_clear_all(void)
{
    bts_hashtable_iter(&acr_table, acr_stat_clear_func, NULL);

    return E_SUCCESS;
}

berr
acr_iter(bts_iter_func func, void *param)
{
    if (NULL == func)
    {
        BRET(E_PARAM);
    }

    bts_hashtable_iter(&acr_table, func, param); 

    return E_SUCCESS;
}

berr acr_account_default_act_set(uint32_t action)
{
	account_default_action = action;
	return E_SUCCESS;
}


/* End of file */
