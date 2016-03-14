#ifndef __NAGA_ADP_H
#define __NAGA_ADP_H

#include "boots.h"
#include "time.h"

berr naga_adp(hytag_t *hytag);

berr adp_set_interval(int interval);

berr adp_dp_init(void);

berr adp_get_interval(int *interval);
berr adp_clear_interval(void);
berr adp_switch_set(int on);
berr adp_switch_get(int *on);


berr adp_switch_template_set(int type, int on);

time_t *adp_get_start_time(void);

berr adp_switch_template_get(int type, int *on);

#endif