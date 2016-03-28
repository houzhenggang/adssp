#ifndef __URL_BLIST_H
#define __URL_BLIST_H

typedef struct 
{
	unsigned char url[256];
}urlkey_t;


typedef struct
{	
	urlkey_t  keys;
	int       kenlen;
	uint64_t  count;
}url_blist_t;


berr urllist_lookup(hytag_t *tag);

berr urllist_init();
#endif