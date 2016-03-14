#include <string.h>
#include <assert.h>

#include "bts_hashtable.h"
#include "bts_debug.h"
#include "bts_linklist.h"

berr 
bts_hashtable_init(bts_hashtable_t *tab, uint32_t bucket_number, bts_hash_func hash, bts_list_cmp_func cmp, bts_list_del_func del)
{
	int i;
    if ((NULL == tab) || (NULL == hash))
    {
        BRET(E_PARAM);
    }

	memset(tab, 0, sizeof(bts_hashtable_t));
    tab->hash = hash;

	tab->total_bucket = bucket_number;
    //tab->buckets = malloc(tab->buckets, 0 , sizeof(bts_hashtable_t) * bucket_number);
    tab->buckets = malloc(sizeof(bts_list_t) * bucket_number);

    if (NULL == tab->buckets)
    {
        BRET(E_MEMORY);
    }


    memset(tab->buckets, 0, sizeof(bts_list_t) * bucket_number);

    if (cmp || del)
    {
        for (i = 0; i < bucket_number; i++)
        {
            tab->buckets[i].cmp = cmp;
            tab->buckets[i].del = del;
            INIT_LIST_HEAD(&(tab->buckets[i].bucket_head));
        }
    }


    BRET(E_SUCCESS);
}

void *
bts_hashtable_lookup(bts_hashtable_t *tab, void *key)
{
    uint32_t hash, idx;
    bts_list_t *bucket = NULL;
    bts_listnode_t *node = NULL;

    if ((NULL == tab) || (NULL == key))
    {
        return NULL;
    }

    if ((NULL == tab->hash) || (0 == tab->total_bucket) || (NULL == tab->buckets))
    {
        return NULL;
    }

    hash = tab->hash(key);

    idx = hash % tab->total_bucket;

    bucket = &tab->buckets[idx];

    node = bts_listnode_lookup_by_key(bucket, key);

    
    if (node)
    {
        return node->data;
    }

    return NULL;
}


void *
bts_hashtable_safe_lookup_and_create
			(bts_hashtable_t *tab, void *key, int size)
{
    uint32_t hash, idx;
    bts_list_t *bucket = NULL;
    bts_listnode_t *node = NULL;
	void *data = NULL;
#if 0
    if ((NULL == tab) || (NULL == key))
    {
        return NULL;
    }

    if ((NULL == tab->hash) || (0 == tab->total_bucket) || (NULL == tab->buckets))
    {
        return NULL;
    }

    hash = tab->hash(key);

    idx = hash % tab->total_bucket;

    bucket = &tab->buckets[idx];

	pthread_mutex_lock(&bucket->mutex);
    node = bts_listnode_lookup_by_key(bucket, key);
	if(node == NULL)
	{
		data = (void *)malloc(size);
		if(data == NULL)
		{
			printf("Failed to malloc listnode\n");
			return NULL;	
		}
		memcpy(data, key, size);    
		list_add();
	}
	pthread_mutex_unlock(&bucket->mutex);

    
    if (node)
    {
        return node->data;
    }
#endif

    return NULL;
}



berr
bts_hashtable_add(bts_hashtable_t *tab, void *data)
{
    uint32_t hash, idx;
    bts_list_t *bucket = NULL;
    bts_listnode_t *node = NULL;

    if ((NULL == tab) || (NULL == data))
    {
        BRET(E_PARAM);
    }

    if ((NULL == tab->hash) || (0 == tab->total_bucket) || (NULL == tab->buckets))
    {
        BRET(E_INIT);
    }

    hash = tab->hash(data);

    idx = hash % tab->total_bucket;

    bucket = &tab->buckets[idx];

    bts_listnode_add(bucket, data);
    

    BRET(E_SUCCESS);
}


berr
bts_hashtable_del(bts_hashtable_t *tab, void *data)
{
    uint32_t key, idx;
    bts_list_t *bucket = NULL;
    bts_listnode_t *node = NULL;

    if ((NULL == tab) || (NULL == data))
    {
        BRET(E_PARAM);
    }

    if ((NULL == tab->hash) || (0 == tab->total_bucket) || (NULL == tab->buckets))
    {
        BRET(E_INIT);
    }
    
    key = tab->hash(data);

    idx = key % tab->total_bucket;

    bucket = &tab->buckets[idx];

    bts_listnode_delete_by_key(bucket, data);

    BRET(E_SUCCESS);
}

berr
bts_hashtable_del_by_key(bts_hashtable_t *tab, void *data)
{
    uint32_t key, idx;
    bts_list_t *bucket = NULL;
    bts_listnode_t *node = NULL;

    if ((NULL == tab) || (NULL == data))
    {
        BRET(E_PARAM);
    }

    if ((NULL == tab->hash) || (0 == tab->total_bucket) || (NULL == tab->buckets))
    {
        BRET(E_INIT);
    }
    
    key = tab->hash(data);

    idx = key % tab->total_bucket;

    bucket = &tab->buckets[idx];

    node = bts_listnode_lookup_by_key(bucket, &key);

    if (node)
    {
        bts_list_delete_node(bucket, node);
    }

    BRET(E_SUCCESS);
}

berr
bts_hashtable_del_all(bts_hashtable_t *tab)
{
	int i = 0;
	if (NULL == tab)
	{
		BRET(E_PARAM); 
	}

    for(i = 0; i < tab->total_bucket; i ++)
    {
		bts_list_delete_all_node(&tab->buckets[i]);
    }

    return E_SUCCESS;
}

berr
bts_hashtable_iter(bts_hashtable_t *tab, bts_iter_func func, void *param)
{
    bts_list_t *bucket = NULL;
    bts_listnode_t *node = NULL;
    void *entry = NULL;

    int i = 0;

    for(i = 0; i < tab->total_bucket; i++)
    {
        bucket = &tab->buckets[i];
#if 0
        for (ALL_BTS_LIST_ELEMENTS_RO(bucket, node, entry))
        {
            (*func)(entry, param);
        }
		bts_list_all_element_lo()
#endif
        bts_list_all_element_lo(bucket, func, param);
    }

    return E_SUCCESS;
}

/* End of file */
