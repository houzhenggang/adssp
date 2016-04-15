#ifndef __SSP_SERVER__
#define __SSP_SERVER__

#include "ad.h"


typedef struct
{
	int adtype;
	char useragent[1024];
	char refer[1024];
	char cookies[1024];
	int cookies_len ; 
	char userip[16];
	time_t ntime;
}apply_info_t;


#define DEFAULT_SHOW_TIME  20


typedef struct
{
	int 	 ad_id;
	time_t   last_push_time;
	uint32_t push_times;
	struct  dlist_head node;
}push_info_t;




typedef struct
{
	char cookeis[16];//20160304-no;
	int  cookeislen;
	uint64_t access_success;
	uint64_t push_success;
	uint64_t push_drop;
	struct  dlist_head head;
}usercookeis;



bts_hashtable_t* init_domain_from_file(char *filename);


ad_struct_t * apply_valid_ad(apply_info_t *, int times);

uint32_t usercookes_total_get();

void today_time_str_init();


berr usercookes_init();

berr usercookeis_assess_add(char *cookeis, int *cookeislen);

int usercookeis_assess_check(char *cookeis, int cookeislen);

usercookeis* usercookeis_get_user_ptr(char *cookeis, int cookeislen);


usercookeis  * usercookeis_assess_new(char *cookeis, int cookeislen);


push_info_t * get_user_times_by_id(usercookeis *cookied, int id);


#endif
