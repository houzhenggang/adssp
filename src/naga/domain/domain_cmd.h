
#ifndef __DMR_CMD_H__
#define __DMR_CMD_H__


void cmdline_dmr_init(void);
void dmr_cmd_config_write(struct vty *vty);

#define  FLAG_SHOW_ALL              0
#define  FLAG_SHOW_PUSHED_NONZERO   1
#define  FLAG_SHOW_FAILED           2
#define  FLAG_SHOW_SUM              3


typedef struct
{
    void    *vty;
    int     flag;
    //int     print_single;
    uint64_t cnt_total;
    uint64_t non_drop;
    uint64_t pushed;
}dmr_param_t;

#endif
