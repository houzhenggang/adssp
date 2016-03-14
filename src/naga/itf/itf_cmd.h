#ifndef __ITF_CMD_H
#define __ITF_CMD_H

void itf_cmd_config_write(struct vty *vty);

/*
 * vsr module cmdline register and init 
 *
 * */
void cmdline_itf_init(void);


#endif
