#include <stdio.h>
#include <unistd.h>

#include "hiredis.h"
#include "zebra.h"
#include "getopt.h"
#include "command.h"

#include "boots.h"
#include "bts_debug.h"

#include "naga_types.h"
#include "naga_util.h"
#include "naga_cmd.h"
#include "product.h"

redisContext* redis = NULL;

redisContext*  redis_init()
{
	redis = redisConnect("127.0.0.1", 6379); 

    if ( redis->err)  
    {  
        redisFree(redis);  
        printf("Connect to redisServer faile\n");  
        return NULL;  
    } 
	return redis;
}

#define CHECK_REDIS_INTER(r) (r->type == REDIS_REPLY_INTEGER)
#define GET_REDIS_INTER_RET(r)  (r->integer)

void  redis_deinit()
{
	redisFree(redis); 
	return ;
}

#define URL_BACKLIST  "/root/ssp_push/blackurl.list"
product_t products[4]
={
		{
			.en = 1,
			.user_push_interval = 5,
			.productname="product_famouns",				
			.white_list_file = "/root/ssp_push/p1_white.conf",	
			.black_list_file = "/root/ssp_push/p1_black.conf",
			.redis_black_set_key = "product_famouns_black_set",
			.redis_white_set_key = "product_famouns_white_set",			
		},		
		{
			.en = 1,
			.user_push_interval = 3,
			.productname="product_normal",	
			.white_list_file = "/root/ssp_push/p2_white.conf",	
			.black_list_file = "/root/ssp_push/p2_black.conf",
			.redis_black_set_key = "product_normal_black_set",
			.redis_white_set_key = "product_normal_white_set",			
		}
};


berr dsp_set_redis_key_set(redisContext* c, char * key, char *filename, uint32_t * size)
{
	FILE * fp = NULL;
	char * p  = NULL;
	char buffer[2048];
	//char newbuffer[2048];
	//char *np = newbuffer;
	
	fp =  fopen(filename, "r+");	
	
	if(fp  == NULL)
	{
		printf("Open File<%s> Failed\n", filename);
		return   E_NULL;					
	}
	else
	{
		printf("Success to Open file %s\n" ,filename);
	}
	
	redisReply* r  = NULL;

	r = (redisReply*)redisCommand(c, "del %s", key);
	
	while( 1)
	{
		if(feof(fp))
			break;

		fgets(buffer, 2048, fp);


		if(strlen(buffer) <= 1)
			break;	
		
		buffer[strlen(buffer)-1] = '\0';
		if(strlen(buffer) == 0)
			break;

		
	
		#if 0
			p = buffer;
		while(1)
		{
			if(iscntrl(*p))
			{
				p++;
				continue;
			}
			else
			{
				*np = *p;
				np++;
				p++;
			}
		}
		#endif
		r = (redisReply*)redisCommand(c, "sadd %s %s", key, buffer); 
		freeReplyObject(r); 
	}


	r = (redisReply*)redisCommand(c, "scard %s", key);
	if(r != NULL)
	{
		if ( CHECK_REDIS_INTER(r))
		{
			*size = GET_REDIS_INTER_RET(r);
		}
	}
	freeReplyObject(r);
	fclose(fp);
	
}

berr dsp_product_init(char * productconf)
{	
#if 0
	FILE *fp = fopen(productconf);

	if(fp == NULL)
	{
		printf("Open product File Failed\n");
		return   E_NULL;
		
	}
	char buffer[2048];
	char *p;
	int i = 0;

	
	while( (p=fgets(buffer, 2048, fp)) != EOF)
	{
					
	}
#endif
	int i;
	redisContext* c = redisConnect("127.0.0.1", 6379); 

    if ( c->err)  
    {  
        redisFree(c);  
        printf("Connect to redisServer faile\n");  
        return ;  
    }  

	int backlistsize;
	dsp_set_redis_key_set(c, "URL_BACKLIST", URL_BACKLIST, &backlistsize);		
	
	
	for(i=0; i < 4; i++)
	{
		if(products[i].en)
		{
			dsp_set_redis_key_set(c, products[i].redis_black_set_key, 
					products[i].black_list_file, &(products[i].black_size));	
			dsp_set_redis_key_set(c, products[i].redis_white_set_key, 
					products[i].white_list_file,  &(products[i].white_size));
		}
	}
	redisFree(c);  
	return E_SUCCESS;
	
}


berr naga_ssp_url_backlist(hytag_t *hytag)
{
	redisContext* c= redis;

	redisReply* r  = NULL;	
	
	if(ACT_DROP == (hytag->acl.actions & ACT_DROP))
	{
        return E_SUCCESS;
	}
    
    if( APP_TYPE_HTTP_GET_OR_POST != hytag->app_type)
    {
        return E_SUCCESS;
    }


	if(hytag->url_append == DISABLE)
	{
		if(hytag->uri[0] == '/' && hytag->host_len > 0 && hytag->uri_len > 0)
		{
			hytag->url_len= snprintf(hytag->url, URL_MAX_LEN, "%s%s",
												hytag->host, hytag->uri);
			hytag->url_append = ENABLE;
		}
	}

	if( hytag->url_len < 150)
		r = (redisReply*)redisCommand(c, "SISMEMBER %s %s", "URL_BACKLIST" , hytag->url);	

	if(r != NULL)
	{
		if(CHECK_REDIS_INTER(r) && GET_REDIS_INTER_RET(r) )
		{
			HYTAG_ACL_SET(hytag->acl, ACT_DROP);
		}
		else
		{
			//printf("set %d\n", r->integer);
		}
	}
	
	return E_SUCCESS;
		
}

berr naga_ssp_counter(hytag_t *hytag)
{
	redisContext* c= redis;

	redisReply* r  = NULL;	
	
	
    if( APP_TYPE_HTTP_GET_OR_POST != hytag->app_type)
    {
        return E_SUCCESS;
    }

	if(hytag->host_len)
	{
	
		r = (redisReply*)redisCommand(c, "HINCRBY ssp_hash_domain_counter %s 1",  hytag->host);

		if(r != NULL)
		{

			if(CHECK_REDIS_INTER(r))
			{
				;
			}
			else
			{
				;
			}	
		}
		else
		{
			printf("Set redis Failed\n");
		}
		freeReplyObject(r);
	}
	
	uint32_t userip = hytag->outer_srcip4;

	if(hytag->user_agent_len)
	{
		r = (redisReply*)redisCommand(c, "HINCRBY ssp_hash_user_counter %d.%d.%d.%d::%s 1", 
										(userip>>24)&0xff, (userip>>16)&0xff,
										(userip>>8)&0xff, (userip)&0xff,
										hytag->user_agent);	

		if(r != NULL)
		{
		
			if(CHECK_REDIS_INTER(r))
			{
				;
			}
			else
			{
				;
			}	
			freeReplyObject(r);
		}

	}
	return E_SUCCESS;
}


berr naga_ssp_product(hytag_t *hytag)

{
	if(redis == NULL)								
		return E_SUCCESS;

	redisContext* c= redis;

	redisReply* r  = NULL;
	
	int i;
	uint32_t action = 0;


    if( APP_TYPE_HTTP_GET_OR_POST != hytag->app_type)
    {
        return E_SUCCESS;
    }

	
	for(i=0; i < 4; i++)
	{
		if(!products[i].en)
			continue;
		
		action = 0;

		if(products[i].black_size)
		{
	
			r = (redisReply*)redisCommand(c, "SISMEMBER %s %s", products[i].redis_black_set_key, hytag->host);

			if(r != NULL)
			{

				if(r->type == REDIS_REPLY_INTEGER && r->integer > 0)
				{
					freeReplyObject(r);
					continue;
				}
				else
				{
					freeReplyObject(r);
				}
			}
			
		}
		if(products[i].white_size)
		{
			r = (redisReply*)redisCommand(c, "SISMEMBER %s %s", products[i].redis_white_set_key, hytag->host);		
			if(r != NULL)
			{
				if( r->type == REDIS_REPLY_INTEGER && r->integer > 0)
				{
					action =   ACT_PUSH;
					hytag->product = &(products[i]);
					freeReplyObject(r);
					break;
				}
				
			}
		}
		else
		{
			hytag->product = &(products[i]);
			action =   ACT_PUSH;  
			break;
		}
	}

	if(action == 0)/*max use one product*/
	{
		HYTAG_ACL_SET(hytag->acl, ACT_DROP);
	}
	else 
	{
		HYTAG_ACL_SET(hytag->acl, ACT_PUSH);
	}
	
	return E_SUCCESS;
	
}


