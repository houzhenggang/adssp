#include "bts_hash.h"
#include "bts_hashtable.h"
#include "bts_linklist.h"
#include "bts_util.h"
#include "bts_debug.h"

#include "domain.h"

uint32_t default_action = 0;

bts_hashtable_t domain_table;

uint32_t 
domain_hash_func(void *data)
{
    dmr_t *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }

    entry = (dmr_t *) data; 

    return bts_hash(entry->host, entry->host_len);
}

int 
domain_cmp_func(void *d1, void *d2)
{
    dmr_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (dmr_t *) d1;
    e2 = (dmr_t *) d2;

    return bts_str_cmp((void *)(e1->host), (void *)(e2->host));
}

berr
domain_init(uint32_t number)
{
    return bts_hashtable_init(&domain_table, number, domain_hash_func, domain_cmp_func, NULL);
}

#if 0
void
dmr_get_func(void *data, void *param)
{
	dmr_t *entry = NULL;
	FILE *fp = NULL;
	char action_str[NAGA_ACTION_STR_SZ];
    if (NULL == data)
    {
        return;

    }

	entry = (dmr_t *)data;

	fp = fopen(DOMAIN_TMP_FILE, "a+");
	if (NULL == fp )
	{
		return;
	}
	naga_action_string(&entry->acl.actions, action_str);
	fprintf(fp, "%-32s %-32s %-16ld\n", entry->host, action_str, (uint64_t) entry->acl.cnt.cnt);
	fclose(fp);
}

#endif

dmr_t*
domain_get(char *key)
{
	dmr_t data;

	memset(&data, 0, sizeof(dmr_t));
	data.host_len = strlen(key);
	memcpy(data.host, key, data.host_len);
    return (dmr_t *)bts_hashtable_lookup(&domain_table, (void*) (&data));
}


dmr_t*
domain_get_and_create(char *key)
{
	dmr_t data;

	memset(&data, 0, sizeof(dmr_t));
	data.host_len = strlen(key);
	memcpy(data.host, key, data.host_len);
    return (dmr_t *)bts_hashtable_safe_lookup_and_create
				(&domain_table, (void*) (&data), sizeof(dmr_t));
}



berr
domain_add(dmr_t *entry)
{
    return bts_hashtable_add(&domain_table, (void *) entry);
}


void
domain_clear_func(void *data, void *param)
{
    if (NULL == data)
    {
        return;

    }

    bts_hashtable_del(&domain_table, data);
}




berr
domain_del(char *host)
{
    dmr_t *entry = NULL;

    entry = domain_get(host);
    if (NULL == entry)
    {
        BRET(E_FOUND);
    }

    return bts_hashtable_del(&domain_table, (void *) entry);
}

berr
domain_clear(void)
{
	#if 0
    return bts_hashtable_del_all(&dmr_table);
	#else
	bts_hashtable_iter(&domain_table, domain_clear_func, NULL);
	return E_SUCCESS;
	#endif
}

void
domain_stat_clear_func(void *data, void *param)
{
    dmr_t *dmr = NULL;
    if (NULL == data)
    {
        return;

    }
    dmr = (dmr_t *) data;

    bts_atomic64_set(&dmr->acl.cnt, 0); 
	bts_atomic64_set(&dmr->acl.vcnt, 0);
}

berr
domain_stat_clear(char *host)
{
    dmr_t *dmr = NULL;

    dmr = domain_get(host);

    if (NULL == dmr)
    {
        BRET(E_FOUND);
    }

    domain_stat_clear_func(dmr, NULL);

    return E_SUCCESS;
}

berr
domain_stat_clear_all(void)
{
    bts_hashtable_iter(&domain_table, domain_stat_clear_func, NULL);

    return E_SUCCESS;
}

berr
domain_iter(bts_iter_func func, void *param)
{
    if (NULL == func)
    {
        BRET(E_PARAM);
    }

    bts_hashtable_iter(&domain_table, func, param); 

    return E_SUCCESS;
}


berr domain_domain_default_act_set(uint32_t action)
{
	default_action = action;
	return E_SUCCESS;
}

berr domain_domain_default_act_get(uint32_t *action)
{
	*action = default_action;
	return E_SUCCESS;
}



/* End of file */
