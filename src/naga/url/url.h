#ifndef __URL_H_
#define __URL_H_

#include "pcre.h"

#define MAX_URL_RULE   64


struct pcre_s
{
    int id;
    int used;
    char *pattern; 
    char *cli_pattern;
    pcre *cre;
    naga_acl_t acl;
};


typedef struct
{
    uint32_t inuse;
    struct pcre_s url_pcre[MAX_URL_RULE] ;  
}url_t;


berr  naga_uri(hytag_t *hytag);

berr url_rule_add(uint32_t id, char *url,  char *cli_url, uint32_t action);

berr url_rule_del(uint32_t id);
struct pcre_s * url_rule_get(uint32_t id);

#endif
