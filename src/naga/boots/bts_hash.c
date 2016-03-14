#include "rte_hash_crc.h"

#define BTS_HASH_INIT_VAL 0
uint32_t bts_hash(const void *data, uint32_t data_len)
{
    return rte_hash_crc(data, data_len, BTS_HASH_INIT_VAL);
}
