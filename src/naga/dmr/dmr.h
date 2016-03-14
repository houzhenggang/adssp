#ifndef __DMR_H__
#define __DMR_H__


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "naga_types.h"
#include "boots.h"

#define DOMAIN_INTERVAL 1

typedef struct {
    uint32_t index;
	uint32_t host_len;
    uint8_t host[MAX_HOST_LEN];
    uint16_t interval;
    naga_acl_t acl;
} dmr_t;

berr dmr_init(uint32_t number);
dmr_t *dmr_get(char *host);
berr dmr_add(dmr_t *entry);
berr dmr_del(char *host);
berr dmr_clear(void);

berr dmr_domain_default_act_set(uint32_t action);

berr dmr_domain_default_act_get(uint32_t *action);

#define api_dmr_get dmr_get
#define api_dmr_add dmr_add
#define api_dmr_del dmr_del
#define api_dmr_clear dmr_clear
#define api_dmr_stat_clear dmr_stat_clear
#define api_dmr_stat_clear_all  dmr_stat_clear_all
#define api_dmr_domain_default_act_set   dmr_domain_default_act_set
#define api_dmr_domain_default_act_get dmr_domain_default_act_get

#define DOMAIN_TMP_FILE  "domain_tmp.txt"


#endif /* !__HOR_DOMAIN_RULE_H__ */
