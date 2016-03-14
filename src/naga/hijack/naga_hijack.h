#ifndef __NAGA_HIJACK_H
#define __NAGA_HIJACK_H

#include "boots.h"
#include "time.h"

berr naga_hijack(hytag_t *hytag);
void hijack_dp_init(void);

#if 0
berr hijack_set_interval(int interval);

berr hijack_dp_init(void);

berr hijack_get_interval(int *interval);
berr hijack_clear_interval(void);
berr hijack_switch_set(int on);
berr hijack_switch_get(int *on);


berr hijack_switch_template_set(int type, int on);

time_t *hijack_get_start_time(void);

berr hijack_switch_template_get(int type, int *on);
#endif
#endif
