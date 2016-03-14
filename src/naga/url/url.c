#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "boots.h"
#include "bts_debug.h"
#include "bts_cnt.h"

#include "naga_types.h"
#include "url.h"
#include "pcre.h"




url_t url_r;

berr url_rule_add(uint32_t id,  char *url,  char * cli_pattern, uint32_t action)
{
    if(id >= MAX_URL_RULE || url == NULL) 
    {
        return E_PARAM;
    }
    int erroffset;
    const char * error= NULL;
    
    struct pcre_s *pcre_n = &(url_r.url_pcre[id]);

    if(pcre_n->used)
    {
        pcre_n->used = 0;
        usleep(300);
        free(pcre_n->pattern);
        pcre_free(pcre_n->cre);
        
        
    }
   
    {
        pcre_n->id = id;
        pcre_n->pattern = strdup(url);
        pcre_n->cli_pattern = strdup(cli_pattern);
        pcre_n->cre =  pcre_compile( pcre_n->pattern , 0, &error, &erroffset, NULL);
		if(pcre_n->cre == NULL)
		{
			free(pcre_n->pattern);
			return E_FAIL;
		}
		pcre_n->acl.actions = action;
		pcre_n->used =1;
		
    }
    if(id >= url_r.inuse)
    {
        url_r.inuse = id +1;
    }
    return E_SUCCESS;
}

struct pcre_s * url_rule_get(uint32_t id)
{
    if(id >= MAX_URL_RULE) 
    {
        return NULL;
    }    

    return &(url_r.url_pcre[id]);   
}


berr url_rule_del(uint32_t id)
{
    if(id >= MAX_URL_RULE) 
    {
        return E_PARAM;
    }
    int i, max_id;
    struct pcre_s *pcre_n = &(url_r.url_pcre[id]);

    if(pcre_n->used)
    {
        pcre_n->used = 0; 
        usleep(300);
        free(pcre_n->pattern); pcre_n->pattern = NULL;
        free(pcre_n->cli_pattern); pcre_n->cli_pattern = NULL;
        pcre_free(pcre_n->cre); pcre_n->cre = NULL;
       
    }
    

   
    for(i=0; i< MAX_URL_RULE; i++)
    {
    	pcre_n = &(url_r.url_pcre[i]);
        if(pcre_n->used)
            max_id = i;                                 
    }
    url_r.inuse = max_id +1;
    return E_SUCCESS;
}


berr  naga_uri(hytag_t *hytag)
{


//#define OVECCOUNT 30

   // const char *tail = "_tTI=tTI";
    //char  *tailptr = NULL;
    int ovector[OVECCOUNT];
    struct pcre_s * urlcre= NULL;
    //if(hytag->uri_len >= 8)    
    //    tailptr = (char *)(&(hytag->uri[hytag->uri_len - 4]));//the last 
    //else
   // tailptr = (char *)(hytag->uri);
    
    uint32_t i; int compare = 0;
    //if(strstr(tailptr,  tail))
    if( APP_TYPE_HTTP_GET_OR_POST != hytag->app_type)
    {
       // CNT_INC(HIJACK_DROP_GET_OR_POST);
        return E_SUCCESS;
    }
#if 0
	if(hytag->uri_len < 8)
	{
	
	}
	else 
	{

		tailptr = hytag->uri + hytag->uri_len - 8;
    	if(!strcmp(tailptr, tail))

    	{
         	CNT_INC(ADP_PUSH_ACK_SUCCESS);
         	hytag->pushed_second_assert = 1;
            hytag->acl.actions |=  ACT_DROP;
            return E_SUCCESS;
    	}
	}
#endif
    if(hytag->uri[0] == '/' && hytag->host_len > 0 && hytag->uri_len > 0)
    {
        hytag->url_len= snprintf(hytag->url, URL_MAX_LEN, "%s%s",
                                                hytag->host, hytag->uri);
    }

        //printf("url is : %s\n", hytag->url);
	
    if(hytag->uri_len == 1 && !strcmp(hytag->uri, "/"))
    {    
    	//hytag->acl.actions |=  ACT_LOG;
        CNT_INC(URL_HOMEPAGE);
		return E_SUCCESS;
    }
    else 
    {
    	//hytag->acl.actions |=  ACT_DROP;
        //CNT_INC(ADP_DROP_BACKSLASH_SUFFIX);
        //return E_SUCCESS;  
        for(i=0; i<url_r.inuse; i ++ )
        {
            urlcre = &(url_r.url_pcre[i]);
            if(urlcre->used && urlcre->cre)
            {
                compare  = pcre_exec(urlcre->cre,
                                NULL, hytag->url, hytag->url_len, 0, 0, ovector, OVECCOUNT);
                
                
                if(compare > 0)
                {
                    if (compare > 1)
                    {
                        memcpy(hytag->reg, (hytag->url + ovector[2]), (ovector[3] - ovector[2])); 
                    }
      
                    ACL_HIT(urlcre->acl); 
                    HYTAG_ACL_MERGE(hytag->acl, urlcre->acl);
				  	//printf("action = 0x%x\n", urlcre->acl.actions);
                   	return E_SUCCESS;
                }
                

            }
        }  
    }

	hytag->acl.actions |=  ACT_DROP;
	CNT_INC(ADP_DROP_BACKSLASH_SUFFIX);
	return E_SUCCESS;  

}


