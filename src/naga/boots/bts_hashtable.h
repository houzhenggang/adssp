#ifndef __BTS_HASHTABLE_H__
#define __BTS_HASHTABLE_H__

#include "bts_linklist.h"
#include "boots.h"

typedef  uint32_t (*bts_hash_func)(void *data);

typedef struct {
    bts_hash_func hash;
    uint32_t total_bucket;
    uint32_t total_cell;
    bts_list_t *buckets;

} bts_hashtable_t;

extern berr bts_hashtable_init(bts_hashtable_t *tab, uint32_t bucket_number, bts_hash_func hash, bts_list_cmp_func cmp, bts_list_del_func del);

extern void *bts_hashtable_lookup(bts_hashtable_t *tab, void *key);
extern berr bts_hashtable_add(bts_hashtable_t *tab, void *data);
extern berr bts_hashtable_del(bts_hashtable_t *tab, void *data);
extern berr bts_hashtable_del_all(bts_hashtable_t *tab);

//typedef void (*bts_iter_func)(void *data, void *param);

berr bts_hashtable_iter(bts_hashtable_t *tab, bts_iter_func func, void *param);

#endif /* !__BTS_HASHTABLE_H__ */
