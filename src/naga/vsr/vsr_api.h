/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr_api.h
#       @date         :2015/08/21 22:53
#       @algorithm    :
=============================================================================*/
//#include "bts_cnt.h"
#include "vsr.h"

#ifndef __VSR_API_H__
#define __VSR_API_H__


uint32_t vsr_api_get_ip_by_index(uint32_t index);
uint64_t vsr_api_get_mobile_by_index(uint32_t index);
berr vsr_api_add_rule(uint32_t index, uint32_t ip, uint64_t msisdn);
berr vsr_api_del_rule_by_index(uint32_t index);
berr vsr_api_get_rule_entry(uint32_t index, vsr_rule_entry_t *rule);
berr vsr_api_clear_statistics(uint32_t index);
berr vsr_api_flush_url(uint32_t index);
void vsr_api_encourage_dec();
berr vsr_api_encourage_set(uint32_t num);
uint32_t  vsr_api_encourage_get(void);
uint64_t  vsr_api_ip_num_get(void);
uint64_t  vsr_api_url_num_get(void);

/* dp use */
berr vsr_dp_api_match(uint32_t index, uint32_t ip, char* url, uint16_t len);
berr vsr_dp_api_init(uint32_t index);
berr vsr_dp_api_request_data_entry(void);


#endif
