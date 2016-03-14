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
#include "rte_atomic.h"
#include "rte_spinlock.h"
#include "naga_types.h"

#ifndef  __DNETSEG__H__
#define  __DNETSEG__H__

#define DNETSEG__RULE_NUM_MAX     150
#define DNETSEG__RULE_EFFECTIVE   1
#define DNETSEG__RULE_UNEFFECTIVE 0


typedef struct {
    uint32_t index;
	uint32_t ip;
	uint32_t mask;
    naga_acl_t acl;
} dnet_t;

typedef rte_spinlock_t  dnet_lock_t;

typedef struct {
    dnet_t net;
    dnet_lock_t lock;
    uint8_t effective;
} dnetseg_t;


//#define CHECK_DNET_EFFECTTIV(_i)   ()


berr api_dnet_add(dnet_t *net);
berr api_dnet_del(uint32_t index);
berr  api_dnet_get(uint32_t index, dnet_t *net, uint8_t *effect);
berr api_dnet_clear_statistics(uint32_t index);

berr api_get_dnetseg_effect(uint32_t index, uint8_t *effect);

berr api_dnetseg_default_act_set(uint32_t action);

berr api_dnetseg_default_act_get(uint32_t *action);

/* dp use */
berr api_dnet_dp_match(uint32_t index, uint32_t ip);
dnetseg_t *api_get_dnetseg_ptr(void);

void dnetseg_init(void);

#endif /* end of __DNET_H__ */
