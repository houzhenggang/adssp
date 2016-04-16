#include <stdio.h>
#include "bts_hashtable.h"
#include "ad.h"

#include "ssp_server.h"


int init_process()
{

	
	usercookes_init();
	ad_list_init();
	today_time_str_init();


	cmdline (0, NULL);    
	
}
