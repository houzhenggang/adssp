#ifndef __ACR_H__
#define __ACR_H__


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "naga_types.h"
#include "boots.h"

typedef struct {
    uint32_t index;
	uint32_t account_len;
    uint8_t account[MAX_ACCOUNT_LEN];
    naga_acl_t acl;
} acr_t;

berr acr_init(uint32_t number);
acr_t *acr_get(char *account);
berr acr_add(acr_t *entry);
berr acr_del(char *account);
berr acr_clear(void);
berr acr_account_default_act_set(uint32_t action);


#define api_acr_get acr_get
#define api_acr_add acr_add
#define api_acr_del acr_del
#define api_acr_clear acr_clear
#define api_acr_stat_clear acr_stat_clear
#define api_acr_stat_clear_all  acr_stat_clear_all
#define api_acr_account_default_act_set acr_account_default_act_set



#endif /* !__HOR_ACCOUNT_RULE_H__ */
