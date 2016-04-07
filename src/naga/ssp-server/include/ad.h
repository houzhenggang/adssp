#ifndef __AD_H_H
#define __AD_H_H



typedef struct
{
	int id;
	int  adtype; //1:pc 2:mb	
	int  prio;
	
	char name[256];
	
	uint64_t  push_all_day;
	uint64_t  push_one_day;

	uint64_t push_per_user;
	uint64_t push_user_interval;

	int push_status; //0;挂起  1,投放中， 2,过期
	int apply_status;//0;未审核, 1,已审核

	char * create_time;
	char * update_time;	
	uint32_t    cpm_price;
	uint32_t    sale_mon;
	char cnzz_code[1024]; //just for note;
	char push_url[512]; //just for not	
	
	char domain_white_file[128];		
	char domain_black_file[128];			

	uint64_t starttime;
	uint64_t endtime;
	uint32_t hourmask;	
}ad_struct_t;



typedef struct
{
	ad_struct_t *ad;
	struct dlist_head node;
}ad_list_node_t;



typedef struct 
{
	int prio;
	int size;
	struct dlist_head head;
}ad_list_info_t;	

#endif