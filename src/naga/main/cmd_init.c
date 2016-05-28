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
	cmdline_vsr_init();
	cmdline_bts_init();
	cmdline_dmr_init();
//	cmdline_domain_init();
	cmdline_acr_init();
	cmdline_adp_init();
	cmdline_itf_init();
	cmdline_adt_init();
	cmdline_netseg_init();
	cmdline_dnetseg_init();
	cmdline_url_init();
	cmdline_urlr_init();
	cmdline_hijack_init();
	/* Zebra related initialize. */
	//access_list_init ();

	cmdline_user_init();

	cmdline_urllist_init();
}


void vty_cmd_config_write(struct vty  *vty)
{

	vsr_cmd_config_write(vty);
	itf_cmd_config_write(vty);
	adt_cmd_config_write(vty);
	dmr_cmd_config_write(vty);
	acr_cmd_config_write(vty);
	netseg_cmd_config_write(vty);
	adp_cmd_config_write(vty);
	url_cmd_config_write(vty);
	urlr_cmd_config_write(vty);
	hijack_cmd_config_write(vty);
	
}

