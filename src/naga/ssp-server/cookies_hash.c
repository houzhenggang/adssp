#include "boots.h"
#include "bts_hash.h"
#include "bts_hashtable.h"
#include "bts_linklist.h"
#include "bts_util.h"
#include "bts_debug.h"


#include "zebra.h"
#include "zebra.h"
#include "getopt.h"
#include "command.h"


bts_hashtable_t usercookeis_control_table;
char today_time_s[20];
uint64_t  today_end_second;
typedef struct
{
	char cookeis[16];//20160304-no;
	int  cookeislen;
	uint64_t access_success;
	uint64_t push_success;
	uint64_t push_drop;
}usercookeis;
//user name




void today_time_str_init()
{
	time_t times = time(NULL);
	struct tm *tm = localtime(&times);
	snprintf(today_time_s, 20, "%4d%02d%02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
	printf("today_time is %s", today_time_s);

	tm->tm_hour =23;
	tm->tm_min = 59;
	tm->tm_sec = 59;

	today_end_second = mktime(tm);
}

uint32_t usercookeis_hash_func(void *data)
{
    usercookeis *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }
	
    entry = (usercookeis *) data; 

    return bts_hash((void *)&(entry->cookeis),  entry->cookeislen);
}

int usercookeis_cmp_func(void *d1, void *d2)
{
    usercookeis *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (usercookeis *) d1;
    e2 = (usercookeis *) d2;
	printf("e1=%s(%d) e2=%s(%d)\n", e1->cookeis, e1->cookeislen, e2->cookeis, e2->cookeislen);
	
	if(e1->cookeislen == e2->cookeislen)
		return memcmp(e1->cookeis, e2->cookeis, e1->cookeislen);
	else
		return 1;
}

void  usercookeis_hash_find_func(void *d1, void *d2)
{
    usercookeis *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return ;
    }

    e1 = (usercookeis *) d1;
    e2 = (usercookeis *) d2;

	e2->access_success ++;
	free(e1);//use The old
	return ;
}

int usercookeis_hash_check_func(void *d1, void *d2, void *program)
{	
    usercookeis *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return ;
    }

    e1 = (usercookeis *) d1;
    e2 = (usercookeis *) d2;

	uint64_t * pro = (uint64_t *) program;
	
	*pro = e2->push_success; 	
	return 0;
}



berr usercookes_init()
{	
    bts_hashtable_init(&usercookeis_control_table, 10000000, 
			usercookeis_hash_func, usercookeis_cmp_func, NULL);	

	usercookeis_control_table.find = usercookeis_hash_find_func;
	usercookeis_control_table.check = usercookeis_hash_check_func;	
	return E_SUCCESS;
}


berr usercookeis_assess_add(char *cookeis, int *cookeislen)
{

	usercookeis *data = malloc(sizeof(usercookeis));
	memset(data, 0x0, sizeof(usercookeis));

	data->cookeislen
		= snprintf(data->cookeis, 16, 
			"%s-%d", today_time_s,usercookeis_control_table.total_cell +1);

	strncpy(cookeis, data->cookeis, data->cookeislen);
	*cookeislen = data->cookeislen;
		
	berr rv = bts_hashtable_add(&usercookeis_control_table, data);

	return rv;
	
}


int usercookeis_assess_check(char *cookeis, int cookeislen)
{

	usercookeis data = {};
	int rv;
	uint64_t access_times = 0;
	memset(&data, 0x0, sizeof(usercookeis));
	
	strncpy((data.cookeis), (cookeis), cookeislen);
	data.cookeislen = cookeislen;	

	rv = bts_hashtable_check_and_create(&usercookeis_control_table, &data, &access_times);
	if(rv /*find*/)
	{	
		int ret = (int)access_times;
		return ret;
	}
	else 
	{
		return 0;
	}	
}


int  incrss_push_success(void *ndata)
{
    usercookeis *e2;

    if (NULL == ndata)
    {
        return 0;
    }

    e2 = (usercookeis *) ndata;
	e2->push_success++;
	return 1;		
}


int incrss_push_drop(void *ndata)
{
    usercookeis *e2;

    if (NULL == ndata)
    {
        return 0;
    }

    e2 = (usercookeis *) ndata;
	e2->push_drop++;;
	return 1;		
}



int usercookeis_update_success(char *cookeis, int cookeislen)
{

	usercookeis data = {};
	int rv;
	int access_times = 0;


	memset(&data, 0x0, sizeof(usercookeis));
	
	strncpy((data.cookeis), (cookeis), cookeislen);
	data.cookeislen = cookeislen;
	rv = bts_hashtable_diyfunc(&usercookeis_control_table, &data,
		incrss_push_success);

	if(!rv)
	{
	}
	else
	{
		
	}
	return 0 ;						
}


int usercookeis_update_drop(char *cookeis, int cookeislen)
{

	usercookeis data = {};
	int rv;
	int access_times = 0;
		
	memset(&data, 0x0, sizeof(usercookeis));
	data.cookeislen = cookeislen;
	strncpy((data.cookeis), (cookeis), cookeislen);
		
	rv = bts_hashtable_diyfunc(&usercookeis_control_table, &data,
		incrss_push_drop);
	return 0 ;						
}



void usercookeis_iter(void *data, void *param)
{
	char buffer[1024];
	usercookeis * user = (usercookeis *) data;
	FILE * fp = param;

	
	snprintf(buffer, 1024, "%s, %ld, %ld, %ld\n", 
		user->cookeis, user->access_success, user->push_success, user->push_drop);
	fputs(buffer, fp);
	return;
}

berr usercookeis_assess_save_file(char * filename)
{
	FILE * fp = NULL;
	
	fp = fopen(filename, "w+");

	if(fp == NULL)
	{
		printf("file %s Open  Failed\n", filename);
		return E_FAIL;
	}
	
	bts_hashtable_iter(&usercookeis_control_table, usercookeis_iter, fp);		
	fclose(fp);
	return E_SUCCESS;
}

#if 0

DEFUN(show_user_all,
      show_user_all_cnt_cmd,
      "usercookeis save FILENAME", "")
{
	char *filename =(char * ) argv[0];
	vty_out(vty, "Success save %s %s", filename, VTY_NEWLINE);
    return usercookeis_assess_save_file(filename);
}



void cmdline_usercookeis_init(void)
{
	install_element(CMD_NODE, &show_user_all_cnt_cmd);
    return ;
}

/* End of file */
#endif


