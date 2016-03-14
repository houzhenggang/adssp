/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/naga/vsr\vsr_dp.c
#       @date         :2015-08-30 13:25
#       @algorithm    :
=============================================================================*/
#include "vsr_api.h"
#include "bts_cnt.h"
#include "log.h"
berr vsr_dp_match(uint32_t ip, char*url, uint16_t url_len)
{
    int i;
    int rv = E_SUCCESS;
    for ( i = 0; i < VSR_RULE_NUM_MAX; i++)
    {
        /* match success */
        rv = vsr_dp_api_match(i,ip, url, url_len);
        if(rv == E_SUCCESS)
        {
            break;
        }
    }
    return rv;
}

berr vsr_dp_process(hytag_t *hytag)
{
    int rv;
    /*add recv statistics */
    cnt_inc(VSR_PKTS);

    /*check protocol type*/
    if ( hytag->app_type != APP_TYPE_HTTP_GET_OR_POST)
    {
        /* add statistics */
        cnt_inc(VSR_UNURLPKTS);
        return E_SUCCESS;
    }

    /*check url len */
    if ( hytag->url_len == 0 || hytag->url_len > VSR_URL_LEN_MAX)
    {
        /* add statistics */
        cnt_inc(VSR_RULE_LEN_OVERFLOW);
        return E_SUCCESS;
    }
    /* add IP_UDP_GTP_IP_URL packet statistics */
    cnt_inc(VSR_URLPKTS);

    /* IP_UDP_GTP_IP_URL packet process */
    rv = vsr_dp_match(hytag->inner_srcip4, hytag->url, hytag->url_len);
    if(rv)
    {
        /*add not match statistics */
        cnt_inc(VSR_UNMATCHPKTS);
        hytag->match &= 0xfffe;
    }
    else
    {
        /* add match statistics */
        cnt_inc(VSR_MATCHPKTS);
        hytag->match |= 1;
    }
    zlog (NULL, LOG_INFO, "ip(%d.%d.%d.%d), match(%d), len(%d), url(%s)",
                           (hytag->inner_srcip4 >> 24) &0xff,
                           (hytag->inner_srcip4 >> 16) &0xff,
                           (hytag->inner_srcip4 >>  8) &0xff,
                           (hytag->inner_srcip4 >>  0) &0xff,
                           hytag->match, hytag->url_len, hytag->url);
    return E_SUCCESS; 
}

/* encourage */
uint32_t srcip[VSR_RULE_NUM_MAX] =
{
    0x01010101,
    0x02020202,
    0x03030403,
    0x04040404,
    0x05050505,
    0x06060606,
    0x07070707,
    0x08080808,
    0x09090909,
    0x0a0a0a0a,
    0x0b0b0b0b,
    0x0c0c0c0c,
    0x0d0d0d0d,
    0x0e0e0e0e,
    0x0f0f0f0f,
    0x10101010,
};

char *url[VSR_RULE_NUM_MAX] =
{
    "www.hao123.com    ",
    "www.baidu.com     ",
    "www.qq.com        ",
    "www.163.com       ",
    "www.youku.com     ",
    "www.tudou.com     ",
    "www.sohu.com      ",
    "www.sina.com      ",
    "www.google.com    ",
    "www.bing.com      ",
    "www.xintansuo,com ",
    "www.evernote.com  ",
    "www.ubuntu.com    ",
    "www.opensuse.com  ",
    "www.slackware.com ",
    "www.slickedit.com ",
};

berr vsr_dp_encourage_test(hytag_t *hytag)
{

    if (NULL == hytag)
    {
        BRET(E_PARAM);
    }

    if(vsr_api_encourage_get())
    {

        hytag->url_len = 18;
        hytag->inner_srcip4 = srcip[vsr_api_encourage_get() % VSR_RULE_NUM_MAX];
        strncpy(hytag->url, url[vsr_api_encourage_get() % VSR_RULE_NUM_MAX], 18);
        hytag->app_type = APP_TYPE_HTTP_GET_OR_POST;
        printf("url_len(%d), url(%s), srcip(0x%x) app_type(%d)\n", hytag->url_len, hytag->url,hytag->inner_srcip4, hytag->app_type);

        vsr_api_encourage_dec();
        return E_SUCCESS; 
    }

    BRET(E_NULL);
}

void vsr_dp_test(void)
{
    hytag_t hytag;
    int  rv;
    while(1)
    {
        rv = vsr_dp_encourage_test(&hytag);
        if (rv)
        {
            continue;
        }
        vsr_dp_process(&hytag);
    }
}

berr vsr_dp_init(void)
{
    int i;
    int rv = E_SUCCESS;
    rv = vsr_dp_api_request_data_entry();
    if (rv)
    {
        printf("dp request data table fail!\n");
        return rv;
    }

    for ( i = 0; i < VSR_RULE_NUM_MAX; i++)
    {
        /* match success */
        rv = vsr_dp_api_init(i);
        if(rv)
        {
            printf("dp init index(%u) fail\n", i);
            return rv;
        }
    }
    printf("vsr dataplane init success!\n");
    return rv;
}

