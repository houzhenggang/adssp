/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr_api.c
#       @date         :2015-08-22 21:35
#       @algorithm    :
=============================================================================*/
#include "vsr.h"
#include "vsr_api.h"
#include "bts_cnt.h"
#include "bts_hash.h"


#define DEBUG
#ifdef  DEBUG   
#define debug(fmt,args...)  printf ("func(%s), line(%d)"fmt"\n" ,__FUNCTION__, __line__, ##args)
#else   
#define debug(fmt,args...)   
#endif  /* DEBUG */ 

#define VSR_LOCK
#ifdef VSR_LOCK
#define VSR_RULE_LOCK(_index)   vsr_lock_rule(_index)
#define VSR_RULE_UNLOCK(_index) vsr_unlock_rule(_index)
#else
#define VSR_RULE_LOCK(_index)
#define VSR_RULE_UNLOCK(_index)
#endif

berr vsr_api_del_rule_by_index(uint32_t index)
{
    uint32_t url_num = 0;
    int i;
    VSR_RULE_LOCK(index);
    /* check effective */
    if(vsr_check_rule_effective(index)== VSR_RULE_UNEFFECTIVE)
    {
        VSR_RULE_UNLOCK(index);
        return E_SUCCESS;
    }

    /*clear ip, mobile, effective */
    vsr_set_rule_ip(index, VSR_RULE_IP_UNEFFECTIVE);
    vsr_set_rule_mobile(index, VSR_RULE_MOBILE_UNEFFECTIVE);
    vsr_set_rule_effective(index, VSR_RULE_UNEFFECTIVE);

    /* loop url_num*/
    url_num = vsr_get_rule_url_num(index);
    for(i = 0; i < url_num; i++)
    {
        /*check url effective */
        if(vsr_check_url_effective(index, i) == VSR_RULE_URL_UNEFFECTIVE )
        {
            /* abnormal deal */
            /* log */
           printf("rule vsr index(%d) url(%d) uneffective ,abnormal!!!\n", index, i); 
           VSR_RULE_UNLOCK(index);
           BRET(E_EFFECTIVE);
        }

        vsr_set_url_len(index, i, VSR_RULE_URL_LEN_UNEFFECTIVE);
        vsr_set_url_hash(index, i, VSR_RULE_URL_HASH_UNEFFECTIVE);
        vsr_set_url_content(index, i, 0, NULL);
        vsr_set_url_pkt(index, i, 0);
        vsr_set_url_effective(index, i, VSR_RULE_URL_UNEFFECTIVE);
    }

    /* clear url_num */
    vsr_set_rule_url_num (index, VSR_RULE_URL_NUM_UNEFFECTIVE);

    vsr_set_rule_pkt(index, 0);


    /* ip num --*/
    vsr_ip_num_dec();
    VSR_RULE_UNLOCK(index);
    return E_SUCCESS;
}


berr vsr_api_add_rule(uint32_t index, uint32_t ip, uint64_t msisdn)
{
    VSR_RULE_LOCK(index);

    /* set ip, mobile, and effective */
    vsr_set_rule_ip(index, ip);
    vsr_set_rule_mobile(index, msisdn);
    vsr_set_rule_effective(index, VSR_RULE_EFFECTIVE);

    /* ip_num ++*/
    vsr_ip_num_add();
    VSR_RULE_UNLOCK(index);

    return E_SUCCESS;
}



/*
 *  * input: index, value form 0-15
 *   * return : ip
 *    */
uint32_t vsr_api_get_ip_by_index(uint32_t index)
{
    uint32_t ip = 0;
    /* lock */
    VSR_RULE_LOCK(index);

    ip = vsr_get_rule_ip(index);
    /* unlock */
    VSR_RULE_UNLOCK(index);

    return ip;
}

/*
 *  * input: index, value form 0-15
 *   * return : mobile
 *    */
uint64_t vsr_api_get_mobile_by_index(uint32_t index)
{
    uint64_t mobile = 0;
    /* lock */
    VSR_RULE_LOCK(index);

    mobile = vsr_get_rule_mobile(index);
    /* unlock */
    VSR_RULE_UNLOCK(index);

    return mobile;
}

berr vsr_api_get_rule_entry(uint32_t index, vsr_rule_entry_t *rule)
{
    /*lock */
    VSR_RULE_LOCK(index);

    /*check rule whether effective or not */
    if(vsr_check_rule_effective(index)== VSR_RULE_UNEFFECTIVE)
    {
        VSR_RULE_UNLOCK(index);
        BRET(E_NULL);
    }

    /*copy to */
    memcpy(rule, vsr_get_rule_entry(index), sizeof(vsr_rule_entry_t));

    /*unlock */
    VSR_RULE_UNLOCK(index);

    return E_SUCCESS;
}

berr vsr_api_flush_url(uint32_t index)
{
    uint32_t url_num = 0;
    int i;
    /* lock */
    VSR_RULE_LOCK(index);

    /*check rule effective */
    if(vsr_check_rule_effective(index)== VSR_RULE_UNEFFECTIVE)
    {
        VSR_RULE_UNLOCK(index);
        return E_SUCCESS;
    }

    /* get rule url num */
    url_num = vsr_get_rule_url_num(index);

    /* loop clear url */
    for(i = 0; i < url_num; i++)
    {
        /*check url effective */
        if(vsr_check_url_effective(index, i) == VSR_RULE_URL_UNEFFECTIVE )
        {
            /* abnormal deal */
            /* log */
           printf("rule vsr index(%d) url(%d) uneffective ,abnormal!!!\n", index, i); 
           VSR_RULE_UNLOCK(index);
           BRET(E_EFFECTIVE);
        }

        vsr_set_url_len(index, i, VSR_RULE_URL_LEN_UNEFFECTIVE);
        vsr_set_url_hash(index, i, VSR_RULE_URL_HASH_UNEFFECTIVE);
        vsr_set_url_content(index, i, 0, NULL);
        vsr_set_url_pkt(index, i, 0);
        vsr_set_url_effective(index, i, VSR_RULE_URL_UNEFFECTIVE);
    }

    /* clear url_num */
    vsr_set_rule_url_num (index, VSR_RULE_URL_NUM_UNEFFECTIVE);

    /* unlock */
    VSR_RULE_UNLOCK(index);
    return E_SUCCESS;

}

berr vsr_api_clear_statistics(uint32_t index)
{
    uint32_t url_num = 0;
    int i;
    /*lock */
    VSR_RULE_LOCK(index);

    /*clear statistics*/
    /*clear rule ip match pkt*/
    vsr_set_rule_pkt(index, 0);

    /* loop clear url match pkt*/
    url_num = vsr_get_rule_url_num(index);
    for(i = 0; i < url_num; i++)
    {
        /*check url effective */
        if(vsr_check_url_effective(index, i) == VSR_RULE_URL_UNEFFECTIVE )
        {
            /* abnormal deal */
            /* log */
           printf("rule vsr index(%d) url(%d) uneffective ,abnormal!!!\n", index, i); 
           VSR_RULE_UNLOCK(index);
           BRET(E_EFFECTIVE);
        }

        vsr_set_url_pkt(index, i, 0);
    }

    /*unlock */
    VSR_RULE_UNLOCK(index);
    return E_SUCCESS;
}

void vsr_api_encourage_dec(void)
{
    vsr_dec_encourage_num();
}

berr vsr_api_encourage_set(uint32_t num)
{
    vsr_set_encourage_num(num);
    return E_SUCCESS;
}

uint32_t  vsr_api_encourage_get(void)
{
    return vsr_get_encourage_num();
}
uint64_t  vsr_api_ip_num_get(void)
{
    return vsr_ip_num_get();
}
uint64_t  vsr_api_url_num_get(void)
{
    return vsr_url_num_get();
}


/* dp use */
berr vsr_dp_api_match(uint32_t index, uint32_t ip, char* url, uint16_t len)
{
    int rv = E_SUCCESS;
    uint32_t hash = 0;
    int i;
    uint32_t url_num = 0;
    /* lock */
    VSR_RULE_LOCK(index);

    /* check rule effective */
    if(vsr_check_rule_effective(index)== VSR_RULE_UNEFFECTIVE)
    {
        /* unlock */
        VSR_RULE_UNLOCK(index);
        BRET(E_NULL);
    }

    /* check ip */
    if ( ip != vsr_get_rule_ip(index))
    {
        /* unlock */
        VSR_RULE_UNLOCK(index);
        BRET(E_MATCH);
    }

    /* update rule statistics */
    /* caluate the hash value of url */
    hash = bts_hash((void *)url, len);
    vsr_inc_rule_pkt(index);

    /* search url */
    url_num = vsr_get_rule_url_num(index);
    for( i = 0; i < url_num; i++)
    {
        /*compare hash */
        if(hash != vsr_get_url_hash(index,i))
        {
            continue;
        }
        /* compare len*/
        if(len != vsr_get_url_len(index, i))
        {
            continue;
        }
        /* compare url */
        if(strncmp(url, vsr_get_url_content(index,i), len))
        {
            continue;
        }
        /* all matched, so break*/
        /* add match ip statistics */
        cnt_inc(VSR_RULE_MATCHPKTS);
        break;
    }

    /* match, just update the statistics */
    if(url_num)
    {
        if(i < url_num)
        {
            vsr_url_pkt_inc(index, i);
        }
    }
    /* not match, insert the url content */
    if ( i == url_num)
    {
        if (url_num == VSR_URL_NUM_MAX)
        {
            cnt_inc(VSR_RULE_URL_OVERFLOW);
            /* unlock */
            VSR_RULE_UNLOCK(index);
            BRET(E_EXCEED);
        }
        /* update hash*/
         vsr_set_url_hash(index, i,hash);
        /* update content */
         vsr_set_url_content(index, i, len, (uint8_t *)url);
         /* set url len */
         vsr_set_url_len(index, i,len);
         /* set url effective */
         vsr_set_url_effective(index, i, VSR_RULE_URL_EFFECTIVE);
        /* update the statitics */
         vsr_url_pkt_inc(index, i);
        /* url_num ++*/
         vsr_inc_rule_url_num(index);
         cnt_inc(VSR_RULE_NEW_URL);
    }

    /* unlock */
    VSR_RULE_UNLOCK(index);

    return rv;
}

berr vsr_dp_api_init(uint32_t index)
{
    int i;
    /* lock init */
    vsr_rule_lock_init(index);    
    /*index init*/
    vsr_set_rule_index(index);
    /* effective  init */
    vsr_set_rule_effective(index, VSR_RULE_UNEFFECTIVE);
    /* ip init */
    vsr_set_rule_ip(index, VSR_RULE_IP_UNEFFECTIVE);
    /* msisdn init*/
    vsr_set_rule_mobile(index, 0);
    /* url_num init */
    vsr_set_rule_url_num(index, 0);
    /* match pact init */
    vsr_set_rule_pkt(index, 0);
    /* loop url init */
    for (i=0; i< VSR_URL_NUM_MAX; i++)
    {
        /* effective init */
        vsr_set_url_effective(index, i, VSR_RULE_URL_UNEFFECTIVE);
        /* len init*/
        vsr_set_url_len(index, i, 0);
        /* hash init */
        vsr_get_url_hash(index, i);
        /* url init */
        vsr_set_url_content(index, i, 0, NULL);
        /* match_pkt init */ 
        vsr_set_url_pkt(index, i, 0);
    }

    return E_SUCCESS;

}

berr vsr_dp_api_request_data_entry(void)
{
    int rv;
    rv = vsr_request_data_entry();
    return rv;
}
