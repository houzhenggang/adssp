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
				printf("domain_white_file:			%s\n", pos->ad->domain_white_file);
				printf("domain_black_file:			%s\n", pos->ad->domain_black_file);			
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

void *loop_check_status(void * param)
{
	
	int i, k;
	ad_list_node_t * pos = NULL, *next = NULL;

	while(1)
	{

		printf("ID\tadtype\tstatus\ttoday_cnt\tall_total\n")
		for(k=1; k<3; k++)
		{ 
		
			for(i=0; i<MAX_PRIO; i++)
			{
				if(ad_lists[k][i].size == 0)
					continue;
				list_for_each_entry_safe(pos, next, &(ad_lists[k][i].head), node)
				{
		
					printf("%d\t%s\t%s\t%ld\t%ld\n", pos->ad->id, 
						pos->ad->adtype == 1 ? "PC": "MB", 
						push_status[pos->ad->push_status], 
						pos->ad->cnt_push_one_day, pos->ad->cnt_push_all_day);
				}
					
			}
		}
		
		sleep(60);
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
     	snprintf(query, 1024, "select * from ad where prio=%d and adtype=%d\n", i, k);
	 
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
					
					//bts_hashtable_t * hashtable = 
					//	init_domain_from_file(struct_ptr->domain_white_file);
					
				}
				if(row[17] != NULL)
				{
					struct_ptr->domain_black_file = strdup(row[17]);
					//struct_ptr->domain_black_hashtb 
						//= (bts_hashtable_t *)init_domain_from_file(struct_ptr->domain_black_file);				
				}
				
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



ad_struct_t * apply_valid_ad (apply_info_t * info, int times)
{
	int i;
	ad_list_node_t * pos = NULL, *next = NULL;
	ad_struct_t * ad = NULL;
	struct dlist_head *cnode = NULL;
	int adtype = info->adtype;
	
	for(i=0; i<MAX_PRIO; i++)
	{
		
		if(ad_lists[adtype][i].size == 0)
			continue;

		pthread_mutex_lock( &ad_lists[adtype][i].mutex);
		
		list_for_each_entry_safe(pos, next,  &(ad_lists[adtype][i].head), node )
		{
			if(1)
			{
				if( pos->ad->push_per_user && pos->ad->push_per_user <= times)
				{
					/*one user push times != 0 && times < ad */
					continue;
				}
				
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
				
				ad = pos->ad;
				dlist_move_tail( &(pos->node), &(ad_lists[adtype][i].head));
				break;
			}	
		}
		pthread_mutex_unlock(&ad_lists[adtype][i].mutex);		
		
		if(ad != NULL)
		{
			//printf("ad type = %d, id= %d\n", adtype, ad->id);
			return ad;
		}	
	}
	
	return NULL;	
}


