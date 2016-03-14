#include "bts_hash.h"
#include "bts_hashtable.h"
#include "bts_linklist.h"
#include "bts_util.h"
#include "bts_debug.h"

#include "hijack.h"


uint32_t  g_hijack_pkt_interval = 1;
uint32_t  g_hijack_ip_interval = 1;
uint32_t  g_hijack_ip_pkt_interval = 1;
uint32_t  g_hijack_differ_ip_time_interval = 10;
uint32_t  g_hijack_switch_enable = 0;

extern time_t   hijack_timep;

hijack_entry_t *hijack_rule_table = NULL;


berr hijack_enable_set(int status)
{
    g_hijack_switch_enable = status;
    return E_SUCCESS;
}

berr hijack_enable_get(int *status)
{
    *status = g_hijack_switch_enable;
    return E_SUCCESS;
}

berr ip_pkt_set_interval(int interval)
{
    g_hijack_ip_pkt_interval = interval;
    return E_SUCCESS;
}


berr ip_set_interval(int interval)
{
    g_hijack_ip_interval = interval;
    return E_SUCCESS;
}

berr pkt_set_interval(int interval)
{
    g_hijack_pkt_interval = interval;
    return E_SUCCESS;
}

berr ip_time_set_interval(int interval)
{
    g_hijack_differ_ip_time_interval = interval;
    return E_SUCCESS;
}

berr ip_time_interval_get(int *interval)
{
    *interval = g_hijack_differ_ip_time_interval;
    return E_SUCCESS;
}

berr ip_interval_get(int *interval)
{
    *interval = g_hijack_ip_interval;
    return E_SUCCESS;
}

berr ip_pkt_interval_get(int *interval)
{
    *interval = g_hijack_ip_pkt_interval;
    return E_SUCCESS;
}

berr pkt_interval_get(int *interval)
{
    *interval = g_hijack_pkt_interval;
    return E_SUCCESS;
}

bts_hashtable_t hijack_ip_table;

uint32_t 
hijack_ip_hash_func(void *data)
{
    hijack_ip_t *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }

    entry = (hijack_ip_t *) data; 

    return bts_hash(entry->ip_str, entry->ip_str_len);
}

int 
hijack_ip_cmp_func(void *d1, void *d2)
{
    hijack_ip_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (hijack_ip_t *) d1;
    e2 = (hijack_ip_t *) d2;

    return bts_str_cmp((void *)(e1->ip_str), (void *)(e2->ip_str));
}

berr
hijack_ip_init(uint32_t number)
{
    return bts_hashtable_init(&hijack_ip_table, number, hijack_ip_hash_func, hijack_ip_cmp_func, NULL);
}



hijack_ip_t*
hijack_ip_get(char *key)
{
	hijack_ip_t data;

	memset(&data, 0, sizeof(hijack_ip_t));
	data.ip_str_len = strlen(key);
	memcpy(data.ip_str, key, data.ip_str_len);
    return (hijack_ip_t *)bts_hashtable_lookup(&hijack_ip_table, (void*) (&data));
}

berr
hijack_ip_add(hijack_ip_t *entry)
{
    return bts_hashtable_add(&hijack_ip_table, (void *) entry);
}


void
hijack_ip_clear_func(void *data, void *param)
{
    if (NULL == data)
    {
        return;

    }

    bts_hashtable_del(&hijack_ip_table, data);
}




berr
hijack_ip_del(char *host)
{
    hijack_ip_t *entry = NULL;

    entry = hijack_ip_get(host);
    if (NULL == entry)
    {
        BRET(E_FOUND);
    }

    return bts_hashtable_del(&hijack_ip_table, (void *) entry);
}

berr
hijack_ip_clear(void)
{
	#if 0
    return bts_hashtable_del_all(&hijack_ip_table);
	#else
	bts_hashtable_iter(&hijack_ip_table, hijack_ip_clear_func, NULL);
	return E_SUCCESS;
	#endif
}

void
hijack_ip_stat_clear_func(void *data, void *param)
{
    hijack_ip_t *hijack = NULL;
    if (NULL == data)
    {
        return;

    }
    hijack = (hijack_ip_t *) data;

    bts_atomic64_set(&hijack->acl.cnt, 0); 
	bts_atomic64_set(&hijack->acl.vcnt, 0);
}

berr
hijack_ip_stat_clear(char *ip_str)
{
    hijack_ip_t *hijack = NULL;

    hijack = hijack_ip_get(ip_str);

    if (NULL == hijack)
    {
        BRET(E_FOUND);
    }

    hijack_ip_stat_clear_func(hijack, NULL);

    return E_SUCCESS;
}

berr
hijack_ip_stat_clear_all(void)
{
    bts_hashtable_iter(&hijack_ip_table, hijack_ip_stat_clear_func, NULL);

    return E_SUCCESS;
}

berr
hijack_ip_iter(bts_iter_func func, void *param)
{
    if (NULL == func)
    {
        BRET(E_PARAM);
    }

    bts_hashtable_iter(&hijack_ip_table, func, param); 

    return E_SUCCESS;
}



berr hijack_get(uint32_t index, hijack_rule_t *hijack, uint8_t *effect)
{
    if (NULL == hijack)
    {
        return E_PARAM;
    }

    if (index >= HIJACK_RULE_NUM_MAX)
    {
        return E_PARAM;
    }
 
	*effect = hijack_rule_table[index].effective;
    memcpy(hijack, &hijack_rule_table[index].hijack, sizeof(hijack_rule_t)); 

   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].effective(%d)\n", index, hijack_rule_table[%d].effective);
   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.index(%d)\n", index, hijack_rule_table[%d].hijack.index);
   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.host: %s\n", index, hijack_rule_table[%d].hijack.host);
   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.key: %s\n", index, hijack_rule_table[%d].hijack.key);
   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.locate: %s\n", index, hijack_rule_table[%d].hijack.locate);

	return E_SUCCESS;
}



berr hijack_del(uint32_t index)
{
    if (index >= HIJACK_RULE_NUM_MAX)
    {
        return E_PARAM;
    }


    memset(&hijack_rule_table[index].hijack, 0, sizeof(hijack_rule_t));
    hijack_rule_table[index].effective = HIJACK_RULE_UNEFFECTIVE;

    //DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].effective(%d)\n", index, hijack_rule_table[%d].effective);
    //DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.index(%d)\n", index, hijack_rule_table[%d].hijack.index);
    //DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.host: %s\n", index, hijack_rule_table[%d].hijack.host);
    //DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.key: %s\n", index, hijack_rule_table[%d].hijack.key);
    //DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.locate: %s\n", index, hijack_rule_table[%d].hijack.locate);

    return E_SUCCESS;
}



berr hijack_add(hijack_rule_t *hijack)
{
    uint32_t index;
    if (NULL == hijack)
    {
        return E_PARAM;
    }
    index = hijack->index;

    memcpy(&hijack_rule_table[index].hijack, hijack, sizeof(hijack_rule_t));
    hijack_rule_table[index].effective = HIJACK_RULE_EFFECTIVE;

    //DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].effective(%d)\n", index, hijack_rule_table[%d].effective);
   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.index(%d)\n", index, hijack_rule_table[%d].hijack.index);
   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.host: %s\n", index, hijack_rule_table[%d].hijack.host);
   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.key: %s\n", index, hijack_rule_table[%d].hijack.key);
   // DBG_INFO(MOD_HIJACK, "hijack_rule_table[%d].hijack.locate: %s\n", index, hijack_rule_table[%d].hijack.locate);

    return E_SUCCESS;
}


hijack_entry_t *get_hijack_table_ptr(void)
{
	if (NULL == hijack_rule_table)
	{
		printf("hijack_rule_table is NULL!!!!!!!!!!!!!\n");
		return NULL;
	}
	return hijack_rule_table;
}


time_t *hijack_get_start_time(void)
{
    return  &hijack_timep;
}


void hijack_init(void)
{
    int i;
	
    hijack_rule_table = malloc(sizeof(hijack_entry_t) * HIJACK_RULE_NUM_MAX);
    if ( NULL == hijack_rule_table )
    {
        printf("%s %d hijack rule table malloc failed\n", __func__, __LINE__);
        return;
    }

    memset(hijack_rule_table, 0, sizeof(hijack_entry_t) * HIJACK_RULE_NUM_MAX);

    return;

}
