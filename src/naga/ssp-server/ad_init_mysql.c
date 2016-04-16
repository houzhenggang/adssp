#include <stdio.h>
#include <linux/types.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "bts_hashtable.h"
#include "ad.h"
#include "ssp_server.h"
#include "init.h"


#define MAX_PRIO  10
int g_max_prio= 10;

ad_struct_t     ad_inuse[4000];
ad_list_info_t  ad_lists[3][MAX_PRIO];

extern uint64_t  today_end_second;


void print_inuse_ad()
{
	int i, k;
	ad_list_node_t * pos = NULL, *next = NULL;

	for(k=1; k<3; k++)
    { 

		for(i=0; i<MAX_PRIO; i++)
		{
			if(ad_lists[k][i].size == 0)
				continue;
			printf("================dev %d prio%d===============\n", k, i);
			list_for_each_entry_safe(pos, next, &(ad_lists[k][i].head), node)
			{

				printf("ID:					%d\n", pos->ad->id);
				printf("adtype:				%d\n", pos->ad->adtype);
				printf("prio:				%d\n", pos->ad->prio);
				printf("push_all_day:		%d\n", pos->ad->push_all_day);
				printf("push_one_day:		%d\n", pos->ad->push_one_day);
				printf("push_per_user:		%d\n", pos->ad->push_per_user);
				printf("push_user_interval:	%d\n", pos->ad->push_user_interval);
				printf("push_status: 		%d\n", pos->ad->push_status);
				printf("apply_status:		%d\n", pos->ad->apply_status);
				printf("cpm_price: 			%d\n", pos->ad->cpm_price);
				printf("push_url:			%s\n", pos->ad->push_url);
				printf("domain_white_file:			%s(%d)\n", pos->ad->domain_white_file
					,ssp_domain_size(pos->ad->domain_white_hashtb));
				printf("domain_black_file:			%s(%d)\n", pos->ad->domain_black_file
					,ssp_domain_size(pos->ad->domain_black_hashtb));
				printf("-----------------------------------\n");							
			}
				
		}
	}
	
}

char *push_status[]={
	"AD_HOLD",
	"AD_RUNNING",
	"AD_OUTOFDATE",
	"AD_TODAY_ENOUGH",
	"AD_ALL_ENOUGH"
};
uint64_t  drop_push_cnt_total = 0;
uint64_t  success_push_cnt_total = 0;

void *loop_check_status(void * param)
{
	
	int i, k;
	ad_list_node_t * pos = NULL, *next = NULL;

	while(1)
	{
		printf("User	: %d\n", usercookes_total_get());
		printf("Success : %lld\n", success_push_cnt_total);
		printf("Drop  	: %lld\n", drop_push_cnt_total);
		printf("ID\tadtype\tstatus\t\tuser_max\ttoday_max\today_cnt\tall_total\n");
		for(k=1; k<3; k++)
		{ 
		
			for(i=0; i<MAX_PRIO; i++)
			{
				if(ad_lists[k][i].size == 0)
					continue;
				list_for_each_entry_safe(pos, next, &(ad_lists[k][i].head), node)
				{
		
					printf("%d\t%s\t%s\t\t%d\t\t%d\t%ld\t\t%ld\n", pos->ad->id, 
						pos->ad->adtype == 1 ? "PC": "MB", 
						push_status[pos->ad->push_status],
						pos->ad->push_per_user,
						pos->ad->push_one_day,
						pos->ad->cnt_push_one_day, pos->ad->cnt_push_all_day);
				}
					
			}
		}
		
		sleep(10);
	}
}
int status_loop_printf()
{
	pthread_t thread;
	pthread_create(&thread, NULL, loop_check_status, NULL);
	pthread_detach(thread);
	return 0;
}

void ad_list_init()
{
	int i, k;

	for(k=1; k<3; k++)
    { 
		for(i=0; i<MAX_PRIO; i++)
		{
				INIT_LIST_HEAD(&(ad_lists[k][i].head));	
		}
	}	
}


char *  jsfile_string_format(char *src)
{
	char buffer[2048];
	int l = strlen(src);
	int i;
	int offset = 0;
	char *new = NULL;

	
	for(i=0; i<l ; i++)
	{
		switch(src[i])
		{
			case '\'':
				buffer[offset++] = '\\';
				buffer[offset++] =  '\"';
				break;
			case '\"':
				buffer[offset++] = '\\';
				buffer[offset++] =  '\"';				
				break;
			default:
				buffer[offset++] = src[i];
					break;
		}
	}
	buffer[offset] = '\0';
	new = strndup(buffer, offset);
	return new;
}


int main(int argc, char *argv[])
{
   MYSQL  mysql ,*sock;
   char query[1024];
   int i, k;
   int l = 0;
   MYSQL_ROW row;


   init_process();
   
   if( (sock = mysql_init(&mysql)) == NULL)  
   {
	printf("mysql init failed\n");
	return 0;
   }	

  if( mysql_real_connect(&mysql,"127.0.0.1", "root","", "addsp", 0, NULL, 0 ) == NULL)
  {
	printf("mysql connect Failed\n");
	return 0;
  } 
	
  int index=0;
  ad_struct_t * struct_ptr = NULL;


  index = 0;

  for(k=1; k<3; k++)
  {
  	for(i=0; i<g_max_prio; i++)
  	{  	 	
     	snprintf(query, 1024, "select * from ad where prio=%d and adtype=%d and push_status=1\n", i, k);
	 
		pthread_mutex_init(&(ad_lists[k][i].mutex), NULL);
     	if(mysql_query(&mysql, query) == 0)
		{
		
				MYSQL_RES *result = mysql_store_result(&mysql);
				if(result == NULL)
				{
					
					continue;
				}
				ad_lists[k][i].prio = i;
				
	    		while (row = mysql_fetch_row(result)) {
					
				struct_ptr = &(ad_inuse[index++]);
				struct_ptr->id = atoi(row[0]);  //id 
				struct_ptr->adtype = atoi(row[1]); //adtype	
				struct_ptr->prio =  i;
				struct_ptr->push_all_day =  atoi(row[4]);
				struct_ptr->push_one_day =  atoi(row[5]);
				struct_ptr->push_per_user  = atoi(row[6]);
				struct_ptr->push_user_interval = atoi(row[7]);			
				struct_ptr->push_status  =  atoi(row[9]);
				struct_ptr->apply_status  =  atoi(row[10]);
				
				strncpy(struct_ptr->push_url,  row[15], sizeof(struct_ptr->push_url));

				if(row[16] != NULL)
				{
					struct_ptr->domain_white_file = strdup(row[16]);
					
					struct_ptr->domain_white_hashtb 
						= init_domain_from_file(struct_ptr->domain_white_file);
					
				}
				if(row[17] != NULL)
				{
					struct_ptr->domain_black_file = strdup(row[17]);
					struct_ptr->domain_black_hashtb 
						= (bts_hashtable_t *)init_domain_from_file(struct_ptr->domain_black_file);				
				}

				if(row[18] == NULL)
				{
					struct_ptr->showtime = DEFAULT_SHOW_TIME;
				}
				else
				{
					struct_ptr->showtime =  atoi(row[18]);
					if(struct_ptr->showtime  == 0)
						struct_ptr->showtime = DEFAULT_SHOW_TIME;
				}

				

				if(row[19] != NULL)
				{
					struct_ptr->jscode  = strdup(row[19]);
					//
					//jsfile_string_format(row[19]);
				}
				
				if(row[20] != NULL)
					struct_ptr->typejs	=  atoi(row[20]);

				
				ad_list_node_t * lnode = (ad_list_node_t *)malloc(sizeof(ad_list_node_t));
				
				if(lnode == NULL)
				{
					printf("Failed to malloc for lnode \n");
					continue;
				}
				
				INIT_LIST_HEAD(&(lnode->node));
				lnode->ad = struct_ptr;
				dlist_add_tail( &(lnode->node), &(ad_lists[k][i].head));
				ad_lists[k][i].size ++;
				ad_lists[k][i].current = &(ad_lists[k][i].head);
	    		}
		}
		else
		{
			continue;
		}
 		}
  	}	
    print_inuse_ad();
	status_loop_printf();
	zmq_server_init();

}

berr  fromat_refer_to_domain(apply_info_t * info)
{
	if(NULL == info)
		return E_FAIL;
	int j  =0;

	
	// skip "http://"
	char * ptr = info->refer + 7;
	char * domain = info->domain;
	int end = 0;
	
	while(1)
	{
		switch(  ptr[j] )
		{
			case '\/':
				domain[j] = '\0';
				end = 1;	
				break;
			default:
				domain[j] = ptr[j];
				break;
		}
		if(end)
			break;
		else
			j++;
	}
	return E_SUCCESS;
}

ad_struct_t * apply_valid_ad (apply_info_t * info, int times)
{
	int i;
	ad_list_node_t * pos = NULL, *next = NULL;
	ad_struct_t * ad = NULL;
	struct dlist_head *cnode = NULL;
	int adtype = info->adtype;
	push_info_t *adinpush = NULL;


	usercookeis* user = 
		usercookeis_get_user_ptr(info->cookies, info->cookies_len);
	
	
	if(user == NULL)
	{
		//new it;
		user = usercookeis_assess_new(info->cookies, info->cookies_len);

		if(user == NULL)
		{	
			return NULL;
		}
	}
	
	for(i=0; i<MAX_PRIO; i++)
	{
		
		if(ad_lists[adtype][i].size == 0)
			continue;

		pthread_mutex_lock( &ad_lists[adtype][i].mutex);
		
		list_for_each_entry_safe(pos, next,  &(ad_lists[adtype][i].head), node )
		{
			if(1)
			{
				if( pos->ad->push_one_day
					&&  pos->ad->cnt_push_one_day >= pos->ad->push_one_day)
				{
					pos->ad->push_status = AD_TODAY_ENOUGH;
					continue;
				}
				
				if( pos->ad->push_all_day 
					&&  pos->ad->cnt_push_all_day >= pos->ad->push_all_day)
				{
					pos->ad->push_status = AD_ALL_ENOUGH;
					continue;
				}


				adinpush = get_user_times_by_id(user, pos->ad->id);
				if(adinpush != NULL)
				{
					if( pos->ad->push_per_user && pos->ad->push_per_user 
						<= adinpush->push_times)
					{
						/*one user push times != 0 && times < ad */
						continue;
					}
				}
				else
				{
					continue;
				}

				
				if( pos->ad->push_user_interval   
					&&( 
					(info->ntime - adinpush->last_push_time)
					< pos->ad->push_user_interval ))
				{
					continue;
				}


				
				fromat_refer_to_domain(info);

				int ret = 0;

				if(ssp_domain_size(pos->ad->domain_black_hashtb))
				{
					ret = ssp_domain_push_lookup(info.domain, pos->ad->domain_black_hashtb);
					if(ret)
						continue;
				}
				if(ssp_domain_size(pos->ad->domain_white_hashtb))
				{
					ret = ssp_domain_push_lookup(info.domain, pos->ad->domain_white_hashtb);
					if(!ret)
						continue;
				}	
				
				ad = pos->ad;
				dlist_move_tail( &(pos->node), &(ad_lists[adtype][i].head));
				break;
			}	
		}
		pthread_mutex_unlock(&ad_lists[adtype][i].mutex);		
		
		if(ad != NULL)
		{
			adinpush->last_push_time = info->ntime;
			adinpush->push_times ++;
			return ad;
		}	
	}
	
	return NULL;	
}


