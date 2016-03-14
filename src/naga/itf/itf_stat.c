/**************************************************************
* Copyright (C) 2014-2015 All rights reserved.
* @Version: 1.0
* @Created: 2015-10-09 09:23
* @Author: SamLiu - shan3275@gmail.com
* @Description: 
*
* @History: 
**************************************************************/
#include "itf.h"
#include "itf_stat.h"
#include "bts_debug.h"

uint8_t itf_tx_enable =  ITF_ENABLE;
uint8_t itf_rx_enable =  ITF_ENABLE;
uint8_t if_enable[INTERFACE_NUM_MAX] = {ITF_ENABLE, ITF_ENABLE};

berr interface_stat_set( port_stat *stat)
{
    if ( NULL == stat)
    {
        return E_PARAM;
    }

    if ( stat->port_id >= INTERFACE_NUM_MAX )
    {
        return E_EFFECTIVE;
    }

    if_enable[stat->port_id] = stat->enable;

    return E_SUCCESS;
}

berr interface_stat_get( port_stat *stat)
{
    if ( NULL == stat)
    {
        return E_PARAM;
    }

    if ( stat->port_id >= INTERFACE_NUM_MAX )
    {
        return E_EFFECTIVE;
    }

    stat->enable = if_enable[stat->port_id];

    return E_SUCCESS;
}

bool interface_is_enable(uint8_t port_id)
{
    if(ITF_ENABLE == if_enable[port_id] )
    {
        return true;
    }

    return false;
}

berr itf_stat_set( itf_stat *stat)
{
    if ( NULL == stat)
    {
        return E_PARAM;
    }

    if ( ITF_TX ==  stat->direction )
    {
        itf_tx_enable = stat->enable;
    }
    else
    if ( ITF_RX ==  stat->direction )
    {
        itf_rx_enable = stat->enable;
    }
    else
    {
        return E_EFFECTIVE;
    }

    return E_SUCCESS;
}

berr itf_stat_get( itf_stat *stat)
{
    if ( NULL == stat)
    {
        return E_PARAM;
    }

    if ( ITF_TX ==  stat->direction )
    {
        stat->enable = itf_tx_enable;
    }
    else
    if ( ITF_RX ==  stat->direction )
    {
        stat->enable = itf_rx_enable;
    }
    else
    {
        return E_EFFECTIVE;
    }

    return E_SUCCESS;
}


bool itf_tx_is_enable(void)
{
    if(ITF_ENABLE == itf_tx_enable )
    {
        return true;
    }

    return false;
}

bool itf_rx_is_enable(void)
{
    if(ITF_ENABLE == itf_rx_enable )
    {
        return true;
    }

    return false;
}

