#include "boots.h"
#include "bts_hash.h"
#include "bts_hashtable.h"
#include "bts_linklist.h"
#include "bts_util.h"
#include "bts_debug.h"
#include "naga_types.h"


#include "zebra.h"
#include "zebra.h"
#include "getopt.h"
#include "command.h"
#include "product.h"
#include "url_blist.h"

bts_hashtable_t url_blacklist_table;

//user name
uint32_t urllist_hash_func(void *data)
{
    url_blist_t *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }
	
    entry = (url_blist_t *) data; 

    return bts_hash((void *)&(entry->keys), entry->kenlen);
}

int urllist_cmp_func(void *d1, void *d2)
{
    url_blist_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (url_blist_t *) d1;
    e2 = (url_blist_t *) d2;

	if(e1->kenlen != e2->kenlen)
		return 1;
	else		
		return memcmp( (void*) (&e1->keys), (void *) (&e2->keys), e1->kenlen);   
}



int urllist_check(void *d1, void *d2, void*pra)
{
    url_blist_t *e1, *e2;

    e1 = (url_blist_t *) d1;
    e2 = (url_blist_t *) d2;

	e2->count++;
	return 1;
}


berr urllist_init()
{	
    bts_hashtable_init(&url_blacklist_table, 50000, 
			urllist_hash_func, urllist_cmp_func, NULL);
	
	url_blacklist_table.find = NULL;
	url_blacklist_table.check = urllist_check;	
	return E_SUCCESS;
}







berr urllist_lookup(hytag_t *tag)
{

    if( APP_TYPE_HTTP_GET_OR_POST != tag->app_type)
    {
        return E_SUCCESS;
    }
	
	if(ACT_DROP == (tag->acl.actions & ACT_DROP))
	{

        return E_SUCCESS;
	}
    
	url_blist_t data ;

	memset(&data, 0x0, sizeof(url_blist_t));
	
	strncpy( (void *)(data.keys.url), (void *)(tag->url), tag->url_len);
	data.kenlen= tag->url_len;	
	int rv = bts_hashtable_check(&url_blacklist_table, &data, NULL);
	if(rv != 0 )//check the black list
	{
		HYTAG_ACL_SET(tag->acl, ACT_DROP);
	}
	return E_SUCCESS;								
}



int cmd_load_urllist_file(struct vty *vty, const char *file_name)
{
	FILE *fp = NULL;
	char host_line[MAX_HOST_LEN] = {0};
	int rv = 0;
	char *p = NULL;

	fp = fopen(file_name, "r");
	if (NULL == fp)
	{
		printf("Open the file %s failed!\n", file_name);
		return CMD_ERR_NOTHING_TODO;
	}

	while(NULL != fgets(host_line, MAX_URL_LEN, fp))
	{
		if ('#' == host_line[0])
		{
			continue;
		}

		if (NULL != (p = strchr(host_line, '\n')))
		{
            *p = '\0';
		}


		url_blist_t data ;
		
		memset(&data, 0x0, sizeof(url_blist_t));
		strcpy( (void *)(data.keys.url), host_line);
		data.kenlen= strlen(host_line);
		bts_hashtable_add( (&url_blacklist_table), &data);
	}

	fclose(fp);

	return CMD_SUCCESS;
}



void urllist_iter(void *data, void *param)
{
	char buffer[1024];
	url_blist_t * urlb = (url_blist_t *) data;
	FILE * fp = param;

	
	snprintf(buffer, 1024, "%s::%ld\n" , urlb->keys.url, urlb->count);
	fputs(buffer, fp);
	return;
}

berr urlbacklist_assess_save_file(char * filename)
{
	FILE * fp = NULL;
	
	fp = fopen(filename, "w+");

	if(fp == NULL)
	{
		printf("file %s Open  Failed\n", filename);
		return E_FAIL;
	}
	
	bts_hashtable_iter(&url_blacklist_table, urllist_iter, fp);		
	fclose(fp);
	return E_SUCCESS;
}

DEFUN(show_urllist_all,
      show_urllist_all_cmd,
      "urlbacklist save FILENAME", "")
{
	char *filename =(char * ) argv[0];
	vty_out(vty, "Success save %s %s", filename, VTY_NEWLINE);
    return cmd_load_urllist_file(vty, filename);
}

DEFUN(load_urllist,
      load_urllist_cmd,
      "urlbacklist load FILENAME", "")
{
	char *filename =(char * ) argv[0];
	vty_out(vty, "Success load %s %s", filename, VTY_NEWLINE);
    return urlbacklist_assess_save_file(filename);
}


void cmdline_urllist_init(void)
{
	install_element(CMD_NODE, &show_urllist_all_cmd);
	install_element(CMD_NODE, &load_urllist_cmd);
    return ;
}


/* End of file */




