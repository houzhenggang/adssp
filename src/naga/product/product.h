#ifndef __PRODUCT_H_
#define __PRODUCT_H_

#include "hiredis.h"

typedef struct 
{
	int id;
	int en;
	long  user_push_interval; //us
	char * productname;
	char * white_list_file;
	char * black_list_file;
	char * redis_black_set_key;
	char * redis_white_set_key;
	uint32_t black_size;
	uint32_t white_size;
}product_t;


berr dsp_product_init(char * productconf);


berr naga_ssp_counter(hytag_t *hytag);

redisContext*  redis_init();

extern redisContext* redis ;
berr naga_ssp_product(hytag_t *hytag);

#endif