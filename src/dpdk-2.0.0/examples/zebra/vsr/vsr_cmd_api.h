/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr_cmd_api.h
#       @date         :2015/08/21 22:54
#       @algorithm    :
=============================================================================*/

#ifndef __VSR_CMD_API_H__
#define __VSR_CMD_API_H__

berr rule_vsr_cmd_add(uint32_t index, uint32_t ip, uint32_t msisdn);
berr rule_vsr_cmd_del(uint32_t index);
berr rule_vsr_cmd_get_index_by_ip(uint32_t ip, uint32_t *index);
berr_rule_vsr_cmd_get_index_by_mobile(uint32_t mobile, uint32_t *index);
berr rule_vsr_cmd_dump(uint32_t index, uint8_t *buff);
berr rule_vsr_cmd_flush_url(uint32_t index); /* clear url */
berr rule_vsr_cmd_clear_statistics(uint32_t index);

#endif __VSR_CMD_API_H__
