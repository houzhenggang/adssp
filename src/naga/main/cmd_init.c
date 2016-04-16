#include <stdio.h>


void vty_cmd_init()
{
	cmdline_vsr_init();
	cmdline_bts_init();
	cmdline_dmr_init();
	cmdline_domain_init();
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



