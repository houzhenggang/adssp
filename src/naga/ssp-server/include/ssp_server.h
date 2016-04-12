#ifndef __SSP_SERVER__
#define __SSP_SERVER__




typedef struct
{
	int adtype;
	char useragent[1024];
	char refer[1024];
	char cookies[1024];
	int cookies_len ; 
	char userip[16];
}apply_info_t;

bts_hashtable_t* init_domain_from_file(char *filename);


ad_struct_t * apply_valid_ad(apply_info_t *, int times);


void today_time_str_init();


berr usercookes_init();

berr usercookeis_assess_add(char *cookeis, int cookeislen);

int usercookeis_assess_check(char *cookeis, int cookeislen);


#endif
