/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/home/shan/work/rose/src/test-1/template\packet.h
#       @date         :2015/09/23 08:50
#       @algorithm    :
=============================================================================*/

#ifndef __PACKET_INCLUDE_H__
#define __PACKET_INCLUDE_H__
#include "naga_types.h"


berr
redirect_302_response_generator(void *ptr, hytag_t *hytag, char *url);


berr
ads_response_head_generator(void *ptr, hytag_t *hytag);
berr
ads_response_content_generator(void *ptr, hytag_t *hytag);


berr ads_mac_get(int dst_or_src, int *custom, uint8_t * mac);



berr ads_mac_set(int dst_or_src, int custom, uint8_t* mac );


#endif
