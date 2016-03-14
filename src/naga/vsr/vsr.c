/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr.c
#       @date         :2015/08/21 22:53
#       @algorithm    :
=============================================================================*/

#include "vsr.h"
#include "stdlib.h"



vsr_rule_t *vsr_rule;

void vsr_ip_num_dec(void)
{
    VSR_STAT_DEC(vsr_rule->ip_num);
}

void vsr_ip_num_add(void)
{
    VSR_STAT_INC(vsr_rule->ip_num);
}

void vsr_ip_num_set(uint64_t val)
{
    VSR_STAT_SET(vsr_rule->ip_num, val);
}

uint64_t vsr_ip_num_get(void)
{
    return (uint64_t)vsr_rule->ip_num.cnt;
}

void vsr_url_num_dec(void)
{
    VSR_STAT_DEC(vsr_rule->url_num);
}

void vsr_url_num_add(void)
{
    VSR_STAT_INC(vsr_rule->url_num);
}

void vsr_url_num_set(uint64_t val)
{
    VSR_STAT_SET(vsr_rule->url_num, val);
}

uint64_t vsr_url_num_get(void)
{
    return (uint64_t)vsr_rule->url_num.cnt;
}


void vsr_rule_lock_init(uint32_t index)
{
    VSR_SPINLOCK_INIT(vsr_rule->rule[index].lock);
}

void vsr_lock_rule(uint32_t index)
{

    VSR_SPINLOCK_LOCK(vsr_rule->rule[index].lock);
}

void vsr_unlock_rule(uint32_t index)
{

    VSR_SPINLOCK_UNLOCK(vsr_rule->rule[index].lock);
}

void vsr_set_rule_index(uint32_t index)
{
    vsr_rule->rule[index].index= index;
}

/*
 *   input  : index,rule index
 *   return : VSR_RULE_EFFECTIVE
 *            VSR_RULE_UNEFFECTIVE
 *
 */
uint32_t vsr_check_rule_effective(uint32_t index)
{
    return vsr_rule->rule[index].effective;
}

void vsr_set_rule_effective(uint32_t index, uint32_t effective )
{
    vsr_rule->rule[index].effective = effective;
}

void vsr_set_rule_ip(uint32_t index, uint32_t ip)
{
    vsr_rule->rule[index].ip = ip;
}

uint32_t vsr_get_rule_ip(uint32_t index)
{

    return vsr_rule->rule[index].ip;
}

void vsr_set_rule_mobile(uint32_t index, uint64_t mobile)
{
    vsr_rule->rule[index].msisdn = mobile;
}

uint64_t vsr_get_rule_mobile(uint32_t index)
{
    return vsr_rule->rule[index].msisdn;
}

void vsr_set_rule_url_num(uint32_t index, uint32_t num)
{
    vsr_rule->rule[index].url_num  = num;

}
void vsr_inc_rule_url_num(uint32_t index)
{
    vsr_rule->rule[index].url_num++;
}

uint32_t vsr_get_rule_url_num(uint32_t index)
{
    return vsr_rule->rule[index].url_num;
}


vsr_rule_entry_t *vsr_get_rule_entry(uint32_t index)
{
    return &vsr_rule->rule[index];
}

void vsr_set_rule_pkt(uint32_t index, uint64_t num)
{
    vsr_rule->rule[index].match_pkt = num;
}

void vsr_add_rule_pkt(uint32_t index, uint64_t num)
{
    vsr_rule->rule[index].match_pkt += num;
}

void vsr_inc_rule_pkt(uint32_t index)
{
    vsr_rule->rule[index].match_pkt ++;
}

/* url operation */
int vsr_check_url_effective(uint32_t index, uint32_t url_index)
{
    return vsr_rule->rule[index].url_entry[url_index].effective;
}

void vsr_set_url_effective(uint32_t index, uint32_t url_index, uint32_t effective)
{
    vsr_rule->rule[index].url_entry[url_index].effective = effective;
}

void vsr_set_url_len(uint32_t index, uint32_t url_index,int len)
{
    vsr_rule->rule[index].url_entry[url_index].len = len;
}

uint32_t vsr_get_url_len(uint32_t index, uint32_t url_index)
{
    return vsr_rule->rule[index].url_entry[url_index].len;
}

void vsr_set_url_hash(uint32_t index, uint32_t url_index,int hash)
{
    vsr_rule->rule[index].url_entry[url_index].hash = hash;
}

uint32_t vsr_get_url_hash(uint32_t index, uint32_t url_index)
{
    return vsr_rule->rule[index].url_entry[url_index].hash;
}

void vsr_set_url_content(uint32_t index, uint32_t url_index, int len, uint8_t * url)
{
    if ( len )
    {
        strncpy((char *)vsr_rule->rule[index].url_entry[url_index].url, (char *)url, len);
    }
    else
    {
        memset((char *)vsr_rule->rule[index].url_entry[url_index].url, 0, VSR_URL_LEN_MAX);
    }
}

char *vsr_get_url_content(uint32_t index, uint32_t url_index)
{
    return (char *)vsr_rule->rule[index].url_entry[url_index].url;
}

void vsr_set_url_pkt(uint32_t index, uint32_t url_index, uint64_t num)
{
    vsr_rule->rule[index].url_entry[url_index].match_pkt = num;
}

void vsr_add_url_pkt(uint32_t index, uint32_t url_index, uint64_t num)
{
    vsr_rule->rule[index].url_entry[url_index].match_pkt += num;
}

void vsr_url_pkt_inc(uint32_t index, uint32_t url_index)
{
    vsr_rule->rule[index].url_entry[url_index].match_pkt ++;
}

void vsr_dec_encourage_num(void)
{
    if (vsr_rule->encourage_num)
    {
        vsr_rule->encourage_num -= 1;
    }
}

void vsr_set_encourage_num(uint32_t num)
{
    vsr_rule->encourage_num = num;
}

uint32_t vsr_get_encourage_num(void)
{
    return vsr_rule->encourage_num;
}

berr vsr_request_data_entry(void)
{
    vsr_rule = (vsr_rule_t *)malloc(sizeof(vsr_rule_t));
    if (vsr_rule == NULL)
    {
        printf("request vsr data entry failed!\n");
        BRET(E_MEMORY);
    }

    vsr_ip_num_set(0);
    vsr_url_num_set(0);
    vsr_set_encourage_num(0);
    return E_SUCCESS;
}

uint32_t vsr_hash(uint8_t *url, uint32_t len)
{
    uint32_t val = 0;
    return  val;
}
