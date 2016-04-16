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


typedef struct 
{
    uint8_t   host[MAX_HOST_LEN];
}domain_key_t;

typedef struct {
	domain_key_t keys;
	uint32_t  host_len;
	uint64_t  stat;
} ssp_domain_t;


uint32_t ssp_domain_hash_func(void *data)
{
    ssp_domain_t *entry = NULL;

    if (NULL == data)
    {
        return 0;
    }
	
    entry = (ssp_domain_t *) data; 

    return bts_hash((void *)&(entry->keys), sizeof(domain_key_t));
}

int ssp_domain_cmp_func(void *d1, void *d2)
{
    ssp_domain_t *e1, *e2;

    if ((NULL == d1) || (NULL == d2))
    {
        return -2;
    }

    e1 = (ssp_domain_t *) d1;
    e2 = (ssp_domain_t *) d2;

	return memcmp( (void*) (&e1->keys), (void *) (&e2->keys), sizeof(e2->keys));   
}

void  ssp_domain_hash_find_func(void *d1, void *d2)
{

	return ;
}

int ssp_domain_hash_check_func(void *d1, void *d2, void *program)
{
	return 0;	
}


berr ssp_domain_push_add(char *host, bts_hashtable_t* hashtable)
{	

	ssp_domain_t *data = (ssp_domain_t *)malloc(sizeof(ssp_domain_t));

	strncpy(data->keys.host, host, MAX_HOST_LEN);
	data->host_len = strlen(host);
	bts_hashtable_add(hashtable, data);
	return E_SUCCESS;
}


uint32_t ssp_domain_size(bts_hashtable_t* hashtable)
{	
	return hashtable->total_cell;
}




bts_hashtable_t* init_domain_from_file(char *filename)
{
	if(filename == NULL)
	{
		return NULL;
	}

	FILE *fp = NULL;
	char host_line[MAX_HOST_LEN] = {0};
	berr rv = 0;
	char *p = NULL;
	int size = 0;
	
	fp = fopen(filename, "r");
	if (NULL == fp)
	{
		printf("open <%s> Failed Failed\n", filename);
		return NULL;
	}

	bts_hashtable_t * hash_table = (bts_hashtable_t *)malloc(sizeof(bts_hashtable_t));
	
    bts_hashtable_init(hash_table, 10000, 
			ssp_domain_hash_func, ssp_domain_cmp_func, NULL);

	while(NULL != fgets(host_line, MAX_HOST_LEN, fp))
	{
		if ('#' == host_line[0])
		{
			continue;
		}

		if (NULL != (p = strchr(host_line, '\n')))
		{
			*p = '\0';
		}

		rv = ssp_domain_push_add(host_line, hash_table);	
	}
	
	fclose(fp);
	return hash_table;
}




int ssp_domain_push_lookup(char *host, bts_hashtable_t* hashtable)
{	

	ssp_domain_t data={};
	
	strncpy(data.keys.host, host, MAX_HOST_LEN);
	data.host_len = strlen(host);
	
	return bts_hashtable_check(hashtable, data, NULL);
}

