/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr.h
#       @date         :2015/08/21 22:56
#       @algorithm    :
=============================================================================*/
#include "stdio.h"
#include "string.h"
#include "boots.h"
#include "pid.h"
#include "rte_atomic.h"
#include "rte_spinlock.h"

#ifndef  __VSR_H__
#define  __VSR_H__

#define VSR_RULE_NUM_MAX  16
#define VSR_URL_NUM_MAX   512
#define VSR_URL_LEN_MAX    512
#define VSR_RULE_EFFECTIVE   1
#define VSR_RULE_UNEFFECTIVE 0
#define VSR_RULE_IP_UNEFFECTIVE 0
#define VSR_RULE_MOBILE_UNEFFECTIVE 0
#define VSR_RULE_URL_NUM_UNEFFECTIVE 0
#define VSR_RULE_URL_EFFECTIVE 1
#define VSR_RULE_URL_UNEFFECTIVE 0
#define VSR_RULE_URL_LEN_UNEFFECTIVE 0
#define VSR_RULE_URL_HASH_UNEFFECTIVE 0
#define VSR_RULE_HOST_UNEFFECTIVE 0


#define __DPDK
#ifdef  __DPDK
#define VSR_STAT_INC(_name) rte_atomic64_inc(&(_name))
#define VSR_STAT_DEC(_name) rte_atomic64_dec(&(_name))
#define VSR_STAT_SET(_name, _val) rte_atomic64_set(&(_name), _val)

#define VSR_SPINLOCK_LOCK(_lock)   rte_spinlock_lock(&(_lock))
#define VSR_SPINLOCK_UNLOCK(_lock) rte_spinlock_unlock(&(_lock))
#define VSR_SPINLOCK_INIT(_lock)   rte_spinlock_init(&(_lock))
#else
#define VSR_STAT_INC(_name)
#define VSR_STAT_DEC(_name)
#define VSR_STAT_SET(_name, _val)

#define VSR_SPINLOCK_LOCK(_lock)
#define VSR_SPINLOCK_UNLOCK(_lock)
#define VSR_SPINLOCK_INIT(_lock)
#endif




typedef rte_spinlock_t  vsr_lock_t;
//typedef uint32_t vsr_lock_t;
typedef rte_atomic64_t vsr_atomic_t;

#define IP_UDP_GTP_IP_URL 34

typedef struct  vsr_url_entry_t {
    uint32_t effective;   /* 0 for not effective; 1 for effective */
    uint32_t len;       /* url length, value from 1-512 */
    uint32_t hash;      /* hash value for url, used for compare*/
    uint8_t  url[VSR_URL_LEN_MAX];
    uint64_t match_pkt; /* matched url  packet number */
} vsr_url_entry_t;

typedef struct vsr_rule_entry_t {
    uint32_t index;
    vsr_lock_t lock;
    uint32_t effective;   /* 0 for not effective; 1 for effective */
    uint32_t ip;
    uint64_t msisdn;
    uint32_t url_num;    /*url number, value from 0-512 */
    uint32_t rsvd;
    vsr_url_entry_t url_entry[VSR_URL_NUM_MAX];
    uint64_t match_pkt; /* matched ip packet number */
}vsr_rule_entry_t;

typedef struct vsr_rule_t {
    vsr_atomic_t ip_num;        /* vsr module ip number from 0-16,for global use*/
    vsr_atomic_t url_num;       /* url entry number,value from 0 to 512 ,for global use */   
    uint32_t encourage_num;
    uint32_t rsvd;
    vsr_rule_entry_t rule[VSR_RULE_NUM_MAX];
}vsr_rule_t;

/* global summary statistics ,for cnt module */
typedef struct vsr_rule_summary_stat_t {
    uint64_t rcv_pkt;   /* vsr module received packets statistics */
    uint64_t ip_match_pkt; /* vsr module matched ip packets statistcs */
    uint64_t ip_unmatch_pkt; /* vsr module unmatched packets statistics */
}vsr_rule_summary_stat;

void vsr_ip_num_dec(void);
void vsr_ip_num_add(void);
void vsr_ip_num_set(uint64_t val);
uint64_t vsr_ip_num_get(void);
void vsr_url_num_dec(void);
void vsr_url_num_add(void);
void vsr_url_num_set(uint64_t val);
uint64_t vsr_url_num_get(void);
void vsr_rule_lock_init(uint32_t index);
void vsr_lock_rule(uint32_t index);
void vsr_unlock_rule(uint32_t index);
void vsr_set_rule_index(uint32_t index);

/*
 *   input  : index,rule index
 *   return : VSR_RULE_EFFECTIVE
 *            VSR_RULE_UNEFFECTIVE
 *
 */
uint32_t vsr_check_rule_effective(uint32_t index);
void vsr_set_rule_effective(uint32_t index, uint32_t effective );
void vsr_set_rule_ip(uint32_t index, uint32_t ip);
uint32_t vsr_get_rule_ip(uint32_t index);
void vsr_set_rule_mobile(uint32_t index, uint64_t mobile);
uint64_t vsr_get_rule_mobile(uint32_t index);
void vsr_set_rule_url_num(uint32_t index, uint32_t num);
void vsr_inc_rule_url_num(uint32_t index);
uint32_t vsr_get_rule_url_num(uint32_t index);
vsr_rule_entry_t *vsr_get_rule_entry(uint32_t index);
void vsr_set_rule_pkt(uint32_t index, uint64_t num);
void vsr_add_rule_pkt(uint32_t index, uint64_t num);
void vsr_inc_rule_pkt(uint32_t index);
/* url operation */
int vsr_check_url_effective(uint32_t index, uint32_t url_index);
void vsr_set_url_effective(uint32_t index, uint32_t url_index, uint32_t effective);
void vsr_set_url_len(uint32_t index, uint32_t url_index,int len);
uint32_t vsr_get_url_len(uint32_t index, uint32_t url_index);
void vsr_set_url_hash(uint32_t index, uint32_t url_index,int hash);
uint32_t vsr_get_url_hash(uint32_t index, uint32_t url_index);
void vsr_set_url_content(uint32_t index, uint32_t url_index, int len, uint8_t * url);
char *vsr_get_url_content(uint32_t index, uint32_t url_index);
void vsr_set_url_pkt(uint32_t index, uint32_t url_index, uint64_t num);
void vsr_add_url_pkt(uint32_t index, uint32_t url_index, uint64_t num);
void vsr_url_pkt_inc(uint32_t index, uint32_t url_index);
void vsr_dec_encourage_num(void);
void vsr_set_encourage_num(uint32_t num);
berr vsr_dp_api_request_data_entry(void);

uint32_t vsr_hash(uint8_t *url, uint32_t len);
berr vsr_request_data_entry(void);

#endif /* end of __VSR_H__ */
