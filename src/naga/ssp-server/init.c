#include <stdio.h>
#include "bts_hashtable.h"
#include "ad.h"

#include "ssp_server.h"


void * user_cmd_init(void *pp)
{
	cmdline (0, NULL);   
	return NULL;
}
int init_process()
{

	
	usercookes_init();
	ad_list_init();
	today_time_str_init();


	pthread_t thread;
	pthread_create(&thread, NULL, user_cmd_init, NULL);
	pthread_detach(thread);
	
}
