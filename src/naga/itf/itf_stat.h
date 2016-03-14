/**************************************************************
* Copyright (C) 2014-2015 All rights reserved.
* @Version: 1.0
* @Created: 2015-10-09 09:25
* @Author: SamLiu - shan3275@gmail.com
* @Description: 
*
* @History: 
**************************************************************/
#include "stdbool.h"
#include "boots.h"
#ifndef __ITF_STAT_H_INCLUDE__
#define __ITF_STAT_H_INCLUDE__
#define ITF_TX   (1 << 0)
#define ITF_RX   (1 << 1)
#define ITF_ENABLE  1
#define ITF_DISABLE 0
#define INTERFACE_NUM_MAX 2

typedef struct
{
    uint8_t direction;
    uint8_t enable;
}itf_stat;

typedef struct
{
    uint8_t port_id;
    uint8_t enable;
}port_stat;

berr interface_stat_set( port_stat *stat);
berr interface_stat_get( port_stat *stat);
bool interface_is_enable(uint8_t port_id);

berr itf_stat_set( itf_stat *stat);
berr itf_stat_get( itf_stat *stat);
bool itf_tx_is_enable(void);
bool itf_rx_is_enable(void);



#endif /* end of __ITF_STAT_H_INCLUDE__ */
