#include "bts_hash.h"
#include "bts_hashtable.h"
#include "bts_linklist.h"
#include "bts_util.h"
#include "bts_debug.h"

#include "urlr.h"

uint32_t url_default_action = 0;

bts_hashtable_t urlr_table;

uint32_t 
urlr_hash_func(void *data)
{
    urlr_t *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }

    entry = (urlr_t *) data; 

    return bts_hash(entry->url, entry->url_len);
}

int 
urlr_cmp_func(void *d1, void *d2)
{
    urlr_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (urlr_t *) d1;
    e2 = (urlr_t *) d2;

    return bts_str_cmp((void *)(e1->url), (void *)(e2->url));
}

berr
urlr_init(uint32_t number)
{
    return bts_hashtable_init(&urlr_table, number, urlr_hash_func, urlr_cmp_func, NULL);
}

#if 0
void
urlr_get_func(void *data, void *param)
{
	urlr_t *entry = NULL;
	FILE *fp = NULL;
	char action_str[NAGA_ACTION_STR_SZ];
    if (NULL == data)
    {
        return;

    }

	entry = (urlr_t *)data;

	fp = fopen(URL_TMP_FILE, "a+");
	if (NULL == fp )
	{
		return;
	}
	naga_action_string(&entry->acl.actions, action_str);
	fprintf(fp, "%-32s %-32s %-16ld\n", entry->url, action_str, (uint64_t) entry->acl.cnt.cnt);
	fclose(fp);
}

#endif

urlr_t*
urlr_get(char *key)
{
	urlr_t data;

	memset(&data, 0, sizeof(urlr_t));
	data.url_len = strlen(key);
	memcpy(data.url, key, data.url_len);
    return (urlr_t *)bts_hashtable_lookup(&urlr_table, (void*) (&data));
}

berr
urlr_add(urlr_t *entry)
{
    return bts_hashtable_add(&urlr_table, (void *) entry);
}


void
urlr_clear_func(void *data, void *param)
{
    if (NULL == data)
    {
        return;

    }

    bts_hashtable_del(&urlr_table, data);
}




berr
urlr_del(char *url)
{
    urlr_t *entry = NULL;

    entry = urlr_get(url);
    if (NULL == entry)
    {
        BRET(E_FOUND);
    }

    return bts_hashtable_del(&urlr_table, (void *) entry);
}

berr
urlr_clear(void)
{
	#if 0
    return bts_hashtable_del_all(&urlr_table);
	#else
	bts_hashtable_iter(&urlr_table, urlr_clear_func, NULL);
	return E_SUCCESS;
	#endif
}

void
urlr_stat_clear_func(void *data, void *param)
{
    urlr_t *urlr = NULL;
    if (NULL == data)
    {
        return;

    }
    urlr = (urlr_t *) data;

    bts_atomic64_set(&urlr->acl.cnt, 0); 
	bts_atomic64_set(&urlr->acl.vcnt, 0);
}

berr
urlr_stat_clear(char *url)
{
    urlr_t *urlr = NULL;

    urlr = urlr_get(url);

    if (NULL == urlr)
    {
        BRET(E_FOUND);
    }

    urlr_stat_clear_func(urlr, NULL);

    return E_SUCCESS;
}

berr
urlr_stat_clear_all(void)
{
    bts_hashtable_iter(&urlr_table, urlr_stat_clear_func, NULL);

    return E_SUCCESS;
}

berr
urlr_iter(bts_iter_func func, void *param)
{
    if (NULL == func)
    {
        BRET(E_PARAM);
    }

    bts_hashtable_iter(&urlr_table, func, param); 

    return E_SUCCESS;
}


berr urlr_default_act_set(uint32_t action)
{
	url_default_action = action;
	return E_SUCCESS;
}

berr urlr_default_act_get(uint32_t *action)
{
	*action = url_default_action;
	return E_SUCCESS;
}



/* End of file */
