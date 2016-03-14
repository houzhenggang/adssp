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

#include "dnet.h"
#include "stdlib.h"
#include "bts_debug.h"

dnetseg_t *dnetseg = NULL;
uint32_t dnetseg_default_action = 0;


#define __DPDK
#ifdef  __DPDK
#define DNETSEG__STAT_INC(_index)         rte_atomic64_inc(&(dnetseg[_index].net.acl.cnt))
#define DNETSEG__STAT_DEC(_index)         rte_atomic64_dec(&(dnetseg[_index].net.acl.cnt))
#define DNETSEG__STAT_SET(_index, _val)   rte_atomic64_set(&(dnetseg[_index].net.acl.cnt), _val)
#define DNETSEG__SPINLOCK_LOCK(_lock)     rte_spinlock_lock(&(_lock))
#define DNETSEG__SPINLOCK_UNLOCK(_lock)   rte_spinlock_unlock(&(_lock))
#define DNETSEG__SPINLOCK_INIT(_lock)     rte_spinlock_init(&(_lock))
#define DNETSEG__RULE_LOCK(_index)        rte_spinlock_lock(&(dnetseg[_index].lock))
#define DNETSEG__RULE_UNLOCK(_index)      rte_spinlock_unlock(&(dnetseg[_index].lock))
#else
#define DNETSEG__STAT_INC(_name)
#define DNETSEG__STAT_DEC(_name)
#define DNETSEG__STAT_SET(_name, _val)
#define DNETSEG__SPINLOCK_LOCK(_lock)
#define DNETSEG__SPINLOCK_UNLOCK(_lock)
#define DNETSEG__SPINLOCK_INIT(_lock)
#endif


berr api_dnet_add(dnet_t *net)
{
    uint32_t index;
    if (NULL == net)
    {
        return E_PARAM;
    }
    index = net->index;

    DNETSEG__RULE_LOCK(index);
    memcpy(&dnetseg[index].net, net, sizeof(dnet_t));
    dnetseg[index].effective = DNETSEG__RULE_EFFECTIVE;

    DBG_INFO(MOD_NET, "dnetseg[%d].effective(%d)\n", index, dnetseg[index].effective);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.index(0x%x)\n", index, dnetseg[index].net.index);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.ip(0x%x)\n", index, dnetseg[index].net.ip);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.mask(0x%x)\n", index, dnetseg[index].net.mask);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.acl.action(0x%x)\n", index, dnetseg[index].net.acl.actions);
    DNETSEG__RULE_UNLOCK(index);
    return E_SUCCESS;
}

berr api_dnet_del(uint32_t index)
{
    if (index >= DNETSEG__RULE_NUM_MAX)
    {
        return E_PARAM;
    }

    DNETSEG__RULE_LOCK(index);
    memset(&dnetseg[index].net, 0, sizeof(dnet_t));
    dnetseg[index].effective = DNETSEG__RULE_UNEFFECTIVE;

    DBG_INFO(MOD_NET, "dnetseg[%d].effective(%d)\n", index, dnetseg[index].effective);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.index(0x%x)\n", index, dnetseg[index].net.index);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.ip(0x%x)\n", index, dnetseg[index].net.ip);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.mask(0x%x)\n", index, dnetseg[index].net.mask);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.acl.action(0x%x)\n", index, dnetseg[index].net.acl.actions);
    DNETSEG__RULE_UNLOCK(index);
    return E_SUCCESS;
}

berr  api_dnet_get(uint32_t index, dnet_t *net, uint8_t *effect)
{
    if (NULL == net)
    {
        return E_PARAM;
    }

    if (index >= DNETSEG__RULE_NUM_MAX)
    {
        return E_PARAM;
    }

    DNETSEG__RULE_LOCK(index);
    memcpy(net, &dnetseg[index].net, sizeof(dnet_t));
	*effect = dnetseg[index].effective;

    DBG_INFO(MOD_NET, "dnetseg[%d].effective(%d)\n", index, dnetseg[index].effective);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.index(0x%x)\n", index, dnetseg[index].net.index);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.ip(0x%x)\n", index, dnetseg[index].net.ip);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.mask(0x%x)\n", index, dnetseg[index].net.mask);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.acl.action(0x%x)\n", index, dnetseg[index].net.acl.actions);
	DBG_INFO(MOD_NET, "dnetseg[%d].effective(%d)\n", index, dnetseg[index].effective);
    DNETSEG__RULE_UNLOCK(index);
	return E_SUCCESS;
}

berr api_dnet_clear_statistics(uint32_t index)
{
    if (index >= DNETSEG__RULE_NUM_MAX)
    {
        return E_PARAM;
    }

    DNETSEG__RULE_LOCK(index);
    bts_atomic64_set(&(dnetseg[index].net.acl.cnt), 0); 
	bts_atomic64_set(&(dnetseg[index].net.acl.vcnt), 0);
    DBG_INFO(MOD_NET, "dnetseg[%d].effective(%d)\n", index, dnetseg[index].effective);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.index(0x%x)\n", index, dnetseg[index].net.index);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.ip(0x%x)\n", index, dnetseg[index].net.ip);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.mask(0x%x)\n", index, dnetseg[index].net.mask);
    DBG_INFO(MOD_NET, "dnetseg[%d].net.acl.action(0x%x)\n", index, dnetseg[index].net.acl.actions);
    DNETSEG__RULE_UNLOCK(index);
	return E_SUCCESS;
}


berr api_get_dnetseg_effect(uint32_t index, uint8_t *effect)
{
	if (index >= DNETSEG__RULE_NUM_MAX)
    {
        return E_PARAM;
    }

	*effect = dnetseg[index].effective;
}

dnetseg_t *api_get_dnetseg_ptr(void)
{
	if (NULL == dnetseg)
	{
		printf("Netseg is NULL!!!!!!!!!!!!!\n");
		return NULL;
	}
	return dnetseg;
}





/* dp use */
berr api_dnet_dp_match(uint32_t index, uint32_t ip)
{
    int rv = E_SUCCESS;
    /* lock */
    DNETSEG__RULE_LOCK(index);

    /* check rule effective */
    if(dnetseg[index].effective == DNETSEG__RULE_UNEFFECTIVE)
    {
        /* unlock */
        DNETSEG__RULE_UNLOCK(index);
        return rv;
    }

    /* check ip */
    if ( ip & dnetseg[index].net.mask != dnetseg[index].net.ip & dnetseg[index].net.mask )
    {
        /* unlock */
        DNETSEG__RULE_UNLOCK(index);
        return rv;
    }

    /* unlock */
    DNETSEG__RULE_UNLOCK(index);

    return rv;
}



berr api_dnetseg_default_act_set(uint32_t action)
{
	dnetseg_default_action = action;
	return E_SUCCESS;
}

berr api_dnetseg_default_act_get(uint32_t *action)
{
	*action = dnetseg_default_action;
	return E_SUCCESS;
}



void dnetseg_init(void)
{
    int i;
	
    dnetseg = malloc(sizeof(dnetseg_t) * DNETSEG__RULE_NUM_MAX);
    if ( NULL == dnetseg )
    {
        printf("%s %d setseg malloc failed\n", __func__, __LINE__);
        return;
    }

    memset(dnetseg, 0, sizeof(dnetseg_t) * DNETSEG__RULE_NUM_MAX);
    for( i = 0; i < DNETSEG__RULE_NUM_MAX; i++)
    {
        dnetseg[i].effective = DNETSEG__RULE_UNEFFECTIVE;
        DNETSEG__SPINLOCK_INIT(dnetseg[i].lock);
    }

    return;

}
