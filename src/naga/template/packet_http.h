/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/home/shan/work/rose/src/test-1/template\packet_http.h
#       @date         :2015/09/23 08:48
#       @algorithm    :
=============================================================================*/
#include "naga_types.h"
#include "stdbool.h"
#ifndef __PACKET_HTTP_INCLUDE_H__
#define __PACKET_HTTP_INCLUDE_H__
typedef enum
{
    ADT_SEND_NULL,
    ADT_SEND_SINGLE,
    ADT_SEND_MULTI
}adt_send_em;

#define TEMPLATE_NAME_LEN_MAX   256
#define TEMPLATE_HEAD_LEN_MAX   8196
#define TEMPLATE_URL_LEN_MAX    512
#define TEMPLATE_TAIL_LEN_MAX   8196
typedef struct
{
    char name[TEMPLATE_NAME_LEN_MAX];
    char head[TEMPLATE_HEAD_LEN_MAX];
    int  head_len;
    char url[TEMPLATE_URL_LEN_MAX];
    int tail_len;
    char tail[TEMPLATE_TAIL_LEN_MAX];
}http_body_t;


berr ads_http_302_fill(char *buf, hytag_t *hytag, char *url);


berr ads_http_content_fill(char *buf, hytag_t *hytag);
berr ads_http_ok_head_fill(char *buf, hytag_t *hytag);
berr ads_template_init(void);
http_body_t *adt_get_http_body(void);
berr adt_set(ad_template_em template, const char *dir_str);
berr adt_get_send(uint8_t *send);
berr adt_send_set(adt_send_em send);
bool adt_send_is_single(void);
bool adt_send_is_multi(void);
#endif /* end of __PACKET_HTTP_INCLUDE_H__ */
