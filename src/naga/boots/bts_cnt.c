#include "boots_custom.h"
#include "bts_cnt.h"
#include "bts_debug.h"

cnt_t cnt_array[CNT_MAX] =
{
    CNT_DEF(ITF0_IPKTS),
    CNT_DEF(ITF1_IPKTS),
    CNT_DEF(ITF_IPKTS),
    CNT_DEF(ITF_IBYTS),
    CNT_DEF(ITF_OPKTS),
    CNT_DEF(ITF_OBYTS),
    CNT_DEF(VLAN_PKTS),
	CNT_DEF(OUTERL3_HD),   /*Head Err, length not compare or infomation err*/
	CNT_DEF(OUTERL3_IPV4),
	CNT_DEF(OUTERL3_IPV6),
	CNT_DEF(OUTERL3_NOIP),

	CNT_DEF(OUTERL4_HD),
	CNT_DEF(OUTERL4_TCP),
	CNT_DEF(OUTERL4_UDP),
	CNT_DEF(OUTERL4_SCTP),

	CNT_DEF(OUTERL4_OTHER),
	CNT_DEF(INNERL3_HD),
	CNT_DEF(INNERL3_IPV4),
	CNT_DEF(INNERL3_IPV6),

	CNT_DEF(INNERL3_NOIP),
	CNT_DEF(INNERL4_HD),
	CNT_DEF(INNERL4_TCP),
	CNT_DEF(INNERL4_UDP),

	CNT_DEF(INNERL4_SCTP),
	CNT_DEF(INNERL4_OTHER),
	CNT_DEF(GTP_U_HD),
	CNT_DEF(GTP_U),

	CNT_DEF(GTP_U_NOPAYLOAD),
	CNT_DEF(GTPC_HD),
	CNT_DEF(GTPV1_C),
	CNT_DEF(GTPV2_C),

	CNT_DEF(APP_HTTP),
	CNT_DEF(APP_OTHER),
	CNT_DEF(HTTP_METHOD_EXCEED),
	CNT_DEF(APP_HTTP_GET),
	CNT_DEF(APP_HTTP_OTHER),
	CNT_DEF(HTTP_URL_EXCEED),
	CNT_DEF(ERR_PACKET),
	CNT_DEF(HEADER_UNEXPECT),
#ifdef CNT_DEF_CUSTOM
    CNT_DEF_CUSTOM,
#endif
};

berr cnt_add(cnte idx, uint64_t value)
{
    if (idx >= CNT_MAX) 
    {
        BRET(E_EXCEED);
    }

    CNT_ADD(idx, value);

    BRET(E_SUCCESS);
}

berr cnt_inc(cnte idx)
{
    if (idx >= CNT_MAX)
    {
        BRET(E_EXCEED);
    }

    CNT_INC(idx);

    BRET(E_SUCCESS);
}

berr cnt_dec(cnte idx)
{
    if (idx >= CNT_MAX)
    {
        BRET(E_EXCEED);
    }

    CNT_DEC(idx); 

    BRET(E_SUCCESS);
}

berr cnt_get(cnte idx, uint32_t number, cnt_t *vals, uint32_t *total)
{
    uint32_t i  = 0, idxc = 0;

    if ((idx + number) > CNT_MAX)
    {
        BRET(E_EXCEED);
    }

    if ((NULL == vals) || (NULL == total))
    {
        BRET(E_PARAM);
    }

    *total = 0;

    for (i = 0; i < number; i++)
    {
        idxc = idx + i;
        vals[i] = cnt_array[idxc];
        *total += 1;
    }

    BRET(E_SUCCESS);
}

berr cnt_clear(cnte idx, uint32_t number, uint32_t *total)
{
    uint32_t i  = 0, idxc = 0;

    if ((idx + number) > CNT_MAX)
    {
        BRET(E_EXCEED);
    }

    if (NULL == total)
    {
        BRET(E_PARAM);
    }

    *total = 0;

    for (i = 0; i < number; i++)
    {
        idxc = idx + i;
        CNT_SET(idxc, 0); 
        *total += 1;
    }

    BRET(E_SUCCESS);
}

berr cnt_int()
{
    uint32_t i = 0;

    for (i = 0; i < CNT_MAX; i++)
    {
        CNT_SET(i, 0);
    }

    BRET(E_SUCCESS);
}

/* End of file */
