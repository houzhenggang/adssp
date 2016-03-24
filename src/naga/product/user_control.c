#include "boots.h"
#include "bts_hash.h"
#include "bts_hashtable.h"
#include "bts_linklist.h"
#include "bts_util.h"
#include "bts_debug.h"
#include "naga_types.h"
#include "user_control.h"

#include "zebra.h"
#include "zebra.h"
#include "getopt.h"
#include "command.h"
#include "product.h"


bts_hashtable_t user_control_table;

//user name
uint32_t user_hash_func(void *data)
{
    useragent_t *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }
	
    entry = (useragent_t *) data; 

    return bts_hash((void *)&(entry->keys), sizeof(userkey_t));
}

int user_cmp_func(void *d1, void *d2)
{
    useragent_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (useragent_t *) d1;
    e2 = (useragent_t *) d2;

	return memcmp( (void*) (&e1->keys), (void *) (&e2->keys), sizeof(e2->keys));   
}

void  user_hash_find_func(void *d1, void *d2)
{
    useragent_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return ;
    }

    e1 = (useragent_t *) d1;
    e2 = (useragent_t *) d2;

	e2->today_push_cnt += e1->today_push_cnt;
	e2->today_access_cnt += e1->today_access_cnt;
	
	free(e1);//use The old
	return ;
}

int user_hash_check_func(void *d1, void *d2, void *program)
{
    useragent_t *e1, *e2;
	long *inteval;
    if ((NULL == d1) || (NULL == d2) ||  (NULL==program))
    {
        return 0;
    }

    e1 = (useragent_t *) d1;
    e2 = (useragent_t *) d2;
	inteval = (long * )program;


	if(e1->last_push_us < e2->last_push_us + *inteval)
	{
		e2->intval_drop ++;
		return 0;							
	}
	else
	{
		e2->last_push_us =  e1->last_push_us;
		e2->today_push_cnt ++;
		return 1;
	}	
}

berr user_init()
{	
    bts_hashtable_init(&user_control_table, 5000000, 
			user_hash_func, user_cmp_func, NULL);
	
	user_control_table.find = user_hash_find_func;
	user_control_table.check = user_hash_check_func;	
	return E_SUCCESS;
}



berr user_push_add(hytag_t *tag)
{	

	time_t rawtime;
	useragent_t *data = malloc(sizeof(useragent_t));

	time(&rawtime);	
	memset(data, 0x0, sizeof(useragent_t));
	
	strncpy( (void *)(data->keys.agent), (void *)(tag->user_agent), tag->user_agent_len);
	data->keys.ip = tag->outer_srcip4;
	
	data->last_push_us = (uint64_t)rawtime;
	data->today_push_cnt = 1; 
	data->today_access_cnt = 0;
	
	bts_hashtable_add(&user_control_table, data);
	return E_SUCCESS;
}


int  user_push_check(hytag_t *tag)
{
	time_t rawtime;
	useragent_t data;
	long  intelval = 0;

	product_t *product = (product_t *)tag->product;
	if(product)
	{
		intelval = product->user_push_interval;
	}
	else
	{
		return 0;
	}

	rawtime = time(NULL);	
	memset(&data, 0x0, sizeof(useragent_t));
	
	strncpy( (void *)(data.keys.agent), (void *)(tag->user_agent), tag->user_agent_len);
	data.keys.ip = tag->outer_srcip4;	
	data.last_push_us = (uint64_t)rawtime;
	
	return bts_hashtable_check(&user_control_table, &data,( void *)(&intelval));

}			





berr user_assess_add(hytag_t *tag)
{

    if( APP_TYPE_HTTP_GET_OR_POST != tag->app_type)
    {
        return E_SUCCESS;
    }

	if(tag->user_agent_len == 0)
		return E_SUCCESS;
	
	useragent_t *data = malloc(sizeof(useragent_t));

	memset(data, 0x0, sizeof(useragent_t));
	
	strncpy( (void *)(data->keys.agent), (void *)(tag->user_agent), tag->user_agent_len);
	data->keys.ip = tag->outer_srcip4;
	
	data->today_access_cnt = 1;		
	bts_hashtable_add(&user_control_table, data);
	return E_SUCCESS;								
}


void user_iter(void *data, void *param)
{
	char buffer[1024];
	useragent_t * user = (useragent_t *) data;
	FILE * fp = param;

	
	snprintf(buffer, 1024, "%d.%d.%d.%d::%s::%ld::%ld::%s\n" , (user->keys.ip>>24)&0xff,
		(user->keys.ip>>16)&0xff,(user->keys.ip>>8)&0xff,(user->keys.ip)&0xff,
		user->keys.agent, user->today_access_cnt, user->today_push_cnt,
		asctime(localtime(&(user->last_push_us))));
	fputs(buffer, fp);
	return;
}

berr user_assess_save_file(char * filename)
{
	FILE * fp = NULL;
	
	fp = fopen(filename, "w+");

	if(fp == NULL)
	{
		printf("file %s Open  Failed\n", filename);
		return E_FAIL;
	}
	
	bts_hashtable_iter(&user_control_table, user_iter, fp);		
	fclose(fp);
	return E_SUCCESS;
}

DEFUN(show_user_all,
      show_user_all_cnt_cmd,
      "userlist save FILENAME", "")
{
	char *filename =(char * ) argv[0];
	vty_out(vty, "Success save %s %s", filename, VTY_NEWLINE);
    return user_assess_save_file(filename);
}



void cmdline_user_init(void)
{
	install_element(CMD_NODE, &show_user_all_cnt_cmd);
    return ;
}


/* End of file */



