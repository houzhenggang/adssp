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
}useragent_t;

#endif

