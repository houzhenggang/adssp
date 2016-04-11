#ifndef __SSP_SERVER__
#define __SSP_SERVER__




bts_hashtable_t* init_domain_from_file(char *filename);


ad_struct_t * apply_valid_ad(int adtype);


berr usercookes_init();

berr usercookeis_assess_add(char *cookeis, int cookeislen);

#endif