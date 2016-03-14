#ifndef __URLR_H__
#define __URLR_H__


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "naga_types.h"
#include "boots.h"

#define URL_INTERVAL 2
#define MAX_URL_LENGTH  512

typedef struct {
    uint32_t index;
	uint32_t url_len;
    uint16_t interval;
    uint8_t url[MAX_URL_LENGTH];
    naga_acl_t acl;
} urlr_t;

berr urlr_init(uint32_t number);
urlr_t *urlr_get(char *url);
berr urlr_add(urlr_t *entry);
berr urlr_del(char *url);
berr urlr_clear(void);

berr urlr_default_act_set(uint32_t action);

berr urlr_default_act_get(uint32_t *action);

#define api_urlr_get urlr_get
#define api_urlr_add urlr_add
#define api_urlr_del urlr_del
#define api_urlr_clear urlr_clear
#define api_urlr_stat_clear urlr_stat_clear
#define api_urlr_stat_clear_all  urlr_stat_clear_all
#define api_urlr_default_act_set   urlr_default_act_set
#define api_urlr_default_act_get urlr_default_act_get

#define URL_TMP_FILE  "url_tmp.txt"


#endif /* !__HOR_URL_RULE_H__ */
