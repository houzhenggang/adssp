/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr_dp.c
#       @date         :2015-08-22 20:53
#       @algorithm    :
=============================================================================*/


berr vsr_dp_match(uint32_t ip, char*url, uint16_t url_len)
{
    int i;
    int rv = E_SUCCESS;
    for ( i = 0; i < VSR_RULE_NUM_MAX; i++)
    {
        /* match success */
        rv = vsr_dp_api_match(i,ip, usr, url_len);
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
    CNT_INC(VSR_PKTS);

    /*check protocol type*/
    if ( hytag->protocol_type != IP_UDP_GTP_IP_URL)
    {
        /* add statistics */
        CNT_INC(VSR_UNURLPKTS);
        return E_SUCCESS;
    }

    /* add IP_UDP_GTP_IP_URL packet statistics */
    CNT_INC(VSR_URLPKTS);

    /* IP_UDP_GTP_IP_URL packet process */
    rv = vsr_dp_match(hytag->inner_srcip, hytag->url, hytag->url_len);
    if(rv)
    {
        /*add not match statistics */
        CNT_INC(VSR_UNMATCHPKTS);
    }
    else
    {
        /* add match statistics */
        CNT_INC(VSR_MATCHPKTS);
    }
    return E_SUCCESS; 
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
            printf("dp init index(%d) fail\n", index);
            return rv;
        }
    }
    printf("vsr dataplane init success!\n");
    return rv;
}

