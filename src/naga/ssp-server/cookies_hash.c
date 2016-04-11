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


bts_hashtable_t usercookeis_control_table;


typedef struct
{
	char cookeis[16];//20160304-no;
	int  cookeislen;
	uint64_t access_success;
	uint64_t push_success;
	uint64_t push_drop;
}usercookeis;
//user name
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

	if(e1.cookeislen == e2.cookeislen)
		return memcmp(e1->cookeis, e2->cookeis, e1.cookeislen);
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
	return 0;
}



berr usercookes_init()
{	
    bts_hashtable_init(&usercookeis_control_table, 10000000, 
			usercookeis_hash_func, usercookeis_cmp_func, NULL);	

	usercookeis_control_table.find = usercookeis_hash_find_func;
	
	return E_SUCCESS;
}


berr usercookeis_assess_add(char *cookeis, int cookeislen)
{

	usercookeis *data = malloc(sizeof(usercookeis));
	memset(data, 0x0, sizeof(usercookeis));
	
	strncpy((data->cookeis), (cookeis), cookeislen);
	
	bts_hashtable_add(&usercookeis_control_table, data);
	
	return E_SUCCESS;								
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



