#ifndef __SSP_SERVER__
#define __SSP_SERVER__




bts_hashtable_t* init_domain_from_file(char *filename);


ad_list_node_t* apply_valid_ad();

#endif