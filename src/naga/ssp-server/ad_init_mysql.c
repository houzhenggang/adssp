#include <stdio.h>
#include <linux/types.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "list.h"

#include "ad.h"



#define MAX_PRIO  10
int g_max_prio= 10;

ad_struct_t     ad_inuse[MAX_PRIO][200];
ad_list_info_t  ad_lists[MAX_PRIO];

void print_inuse_ad()
{
	int i;
	ad_list_node_t * pos = NULL, *next = NULL;

	for(i=0; i<MAX_PRIO; i++)
	{
		if(ad_lists[i].size == 0)
			continue;
		printf("================prio%d===============\n", i);
		list_for_each_entry_safe(pos, next, &(ad_lists[i].head), node)
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
			printf("-----------------------------------\n");							
		}
	
					
	}
	
}

void ad_list_init()
{
	int i;
	for(i=0; i<MAX_PRIO; i++)
	{
			INIT_LIST_HEAD(&(ad_lists[i].head));	
	}
}

int main()
{
   MYSQL  mysql ,*sock;
   char query[1024];
   int i;
   int l = 0;
   MYSQL_ROW row;


   ad_list_init();
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
  for(i=0; i<g_max_prio; i++)
  {

  	 index = 0;
     snprintf(query, 1024, "select * from ad where prio=%d\n", i);
	 

     if(mysql_query(&mysql, query) == 0)
	{
		
		MYSQL_RES *result = mysql_store_result(&mysql);
		if(result == NULL)
		{
			continue;
		}

		ad_lists[i].prio = i;
    	while (row = mysql_fetch_row(result)) {
			struct_ptr = &(ad_inuse[i][index++]);
			struct_ptr->id = atoi(row[0]);  //id 
			struct_ptr->adtype = atoi(row[1]); //adtype	
			struct_ptr->prio =  i;
			struct_ptr->push_all_day =  atoi(row[4]);
			struct_ptr->push_one_day =  atoi(row[5]);
			struct_ptr->push_per_user  = atoi(row[6]);
			struct_ptr->push_user_interval = atoi(row[7]);			
			struct_ptr->push_status  =  atoi(row[9]);
			struct_ptr->apply_status  =  atoi(row[10]);
			
			ad_list_node_t * lnode = (ad_list_node_t *)malloc(sizeof(ad_list_node_t));
			
			if(lnode == NULL)
			{
				printf("Failed to malloc for lnode \n");
				continue;
			}
			INIT_LIST_HEAD(&(lnode->node));
			lnode->ad = struct_ptr;
			dlist_add_tail( &(lnode->node), &(ad_lists[i].head));
			ad_lists[i].size ++;
    	}
	}
	else
	{
		continue;
	}
  }



  print_inuse_ad();
}






