#include <stdio.h>
#include "zebra.h"
#include "version.h"
#include "getopt.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"
#include "vsr_file.h"

void vty_cmd_init()
{
	cmdline_usercookeis_init();
}

void vty_cmd_config_write(struct vty  *vty)
{
	return ;
}

