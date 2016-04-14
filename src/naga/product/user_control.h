#ifndef __USER_AGENT
#define __USER_AGENT





typedef struct userkey
{
	uint32_t ip;
	unsigned char agent[256];
}userkey_t;


typedef struct
{	
	userkey_t keys;
	uint64_t last_push_us;
	uint64_t today_push_cnt;
	uint64_t today_access_cnt;
	uint64_t intval_drop;
}useragent_t;


berr naga_ssp_url_backlist(hytag_t *hytag);


int  user_push_check(hytag_t *tag);


int get_user_interval();

#endif

