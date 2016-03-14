#ifndef __ACR_ACCOUNT_RULE_H__
#define __ACR_ACCOUNT_RULE_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "naga_types.h"
#include "boots.h"


typedef struct {
    uint32_t idx;
	uint32_t account_len;
    uint8_t account[NAGA_ACCOUNT_STR_SZ];
    naga_acl_t acl;
} acr_account_rule_t;

berr acr_account_rule_init(uint32_t number);
acr_account_rule_t* acr_account_rule_lookup(void *account);
berr acr_account_rule_add(acr_account_rule_t *entry);

berr acr_account_rule_del(acr_account_rule_t *entry);


#endif /* !__ACR_ACCOUNT_RULE_H__ */
