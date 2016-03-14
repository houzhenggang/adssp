#ifndef __NAGA_UTIL_H__
#define __NAGA_UTIL_H__

#include "boots.h"

#define NAGA_ACTION_STR_SZ  32

berr naga_action_parse(char *str, uint32_t *actions);
void naga_action_string(uint32_t *action, char *str);

#endif /* !__NAGA_UTIL_H__ */
