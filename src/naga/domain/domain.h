#ifndef __DMR_H__
#define __DMR_H__


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "naga_types.h"
#include "boots.h"

#define DOMAIN_INTERVAL 2

typedef struct {
    uint32_t index;
	uint32_t host_len;
    uint8_t host[MAX_HOST_LEN];
    uint16_t interval;
    naga_acl_t acl;
} dmr_t;

berr domain_init(uint32_t number);
dmr_t *domain_get(char *host);
berr domain_add(dmr_t *entry);
berr domain_del(char *host);
berr domain_clear(void);

berr domain_domain_default_act_set(uint32_t action);

berr domainss_domain_default_act_get(uint32_t *action);

#define api_dmr_get domain_get
#define api_dmr_add domain_add
#define api_dmr_del domain_del
#define api_dmr_clear domain_clear
#define api_dmr_stat_clear domain_stat_clear
#define api_dmr_stat_clear_all  domain_stat_clear_all
#define api_dmr_domain_default_act_set   domain_domain_default_act_set
#define api_dmr_domain_default_act_get domain_domain_default_act_get

#define DOMAIN_TMP_FILE  "domain_tmp.txt"


#endif /* !__HOR_DOMAIN_RULE_H__ */
