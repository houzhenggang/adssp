#include "stdio.h"
#include "boots.h"
#include "bts_debug.h"
#include "naga_types.h"

int
bts_ipaddr_cmp(void *val1, void *val2)
{
    bts_ipaddr_t *ip1, *ip2;

    ip1 = (bts_ipaddr_t *) val1;
    ip2 = (bts_ipaddr_t *) val2;

    if ((NULL== ip1) || (NULL == ip2))
    {
        return -2;
    }

    if (*ip1 > *ip2)
    {
        return 1;
    }

    if (*ip1 < *ip2)
    {
        return -1;
    }

    return 0;
}

int
bts_str_cmp(void *val1, void *val2)
{
    if ((NULL== val1) || (NULL == val2))
    {
        return -2;
    }

    return strcmp(val1, val2);
}



void bts_ip_string(uint32_t ip, char *str)
{
	if (NULL == str)
	{
		return;
	}
    sprintf(str, "%d.%d.%d.%d",
            ((ip >> 24) & 0xff),
            ((ip >> 16) & 0xff),
            ((ip >> 8) & 0xff),
            (ip & 0xff));

}

berr 
bswt_parse(char *str, bswt *val)
{
    if ((NULL == str) || (NULL == val))
    {
        BRET(E_PARAM);
    }

    if (!strncmp(str, "on"))
    {
        *val = ON;
    } 
    else if (!strncmp(str, "off"))
    {
        *val = OFF;
    } 
    else if (!strncmp(str, "bas"))
    {
        *val = BAS;
    }
    else
    {
        BRET(E_FORMAT);
    }

    return E_SUCCESS;
}

void
bswt_string(bswt *val, char *str)
{
    if ((NULL == str) || (NULL == val))
    {
        return;
    }

    if (ON == *val)
    {
        sprintf(str, "on");
    }
    else if (OFF == *val)
    {
        sprintf(str, "off");
    }
    else if (BAS == *val)
    {
        sprintf(str, "bas");
    }
    else 
    {
        sprintf(str, "na");
    }
}



/* End of file */
