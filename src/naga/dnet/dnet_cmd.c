/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/home/shan/work/rose/src/naga/vsr\dnet_cmd.c
#       @date         :2015/09/05 11:53
#       @algorithm    :
=============================================================================*/
#include <string.h>

#include "zebra.h"
#include "version.h"
#include "getopt.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"
#include "dnet.h"
#include "naga_cmd.h"
#include "naga_util.h"
#include "bts_util.h"

#define DNET_STR                     "Net list\n"
#define ADD_STR                     "ADD Operation\n"
#define DEL_STR                     "Del Operation\n"
#define DNET_INDEX_STR               "Vister Record list index\n"
#define DNET_ALL_STR                 "All Net list\n"


//#define DEBUG
#ifdef  DEBUG
#define debug(fmt,args...)  vty_out(vty, "func=%s line=%d:" fmt "%s", __FUNCTION__, __LINE__, ##args, VTY_NEWLINE)
#else
#define debug(fmt,args...)
#endif  /* DEBUG */

#define LITTLE_TO_BIG(_ip) \
       _ip =   ( (_ip & 0xff000000) >> 24) \
             | ( (_ip & 0x00ff0000) >> 8 ) \
             | ( (_ip & 0x0000ff00) << 8 ) \
             | ( (_ip & 0x000000ff) << 24)

static int dnet_cmd_add(struct vty *vty, const char *index_str, const char *ip_str, const char *act_str)
{
    int ret = 0;
    uint32_t index = 0;
    uint32_t ip    = 0;
    uint32_t u_mask    = 0;
    struct prefix p;
    struct in_addr mask;
    dnet_t net;

    memset(&net, 0 ,sizeof(dnet_t));

    index = atoi(index_str);
    debug("index:%d", index);

    ret = str2prefix (ip_str, &p);
    if (ret <= 0)
    {
        vty_out (vty, "%% Malformed address%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    ip =((struct prefix_ipv4 *) &p)->prefix.s_addr;
    debug("ip: %d.%d.%d.%d", (ip >> 24) & 0xff,
                             (ip >> 16) & 0xff,
                             (ip >> 8 ) & 0xff,
                             (ip >> 0 ) & 0xff);
    LITTLE_TO_BIG(ip);
    debug("ip: %d.%d.%d.%d", (ip >> 24) & 0xff,
                             (ip >> 16) & 0xff,
                             (ip >> 8 ) & 0xff,
                             (ip >> 0 ) & 0xff);

    masklen2ip(p.prefixlen, &mask);
    u_mask = mask.s_addr;
    debug("mask: %d.%d.%d.%d", (u_mask >> 24) & 0xff,
                               (u_mask >> 16) & 0xff,
                               (u_mask >> 8 ) & 0xff,
                               (u_mask >> 0 ) & 0xff);

    net.index = index;
    net.ip    = ip;
    net.mask  = ntohl(mask.s_addr);

    if(naga_action_parse((char *)act_str, &net.acl.actions))
    {
        return CMD_ERR_NO_MATCH;
    }

    ret = api_dnet_add(&net);
    if (ret)
    {
        vty_out(vty, "net add error, index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(dnet_add,
      dnet_add_cmd,
      "dnet add <0-149> A.B.C.D/M ACT",
      DNET_STR
      ADD_STR
      DNET_INDEX_STR
      "IP information, like A.B.C.D/M\n"
      ACTION_STR
      )
{
    return dnet_cmd_add(vty, argv[0], argv[1], argv[2]);
}

static int dnet_cmd_del(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;

    index = atoi(index_str);
    debug("index:%d", index);

    ret = api_dnet_del(index);
    if (ret)
    {
        vty_out(vty, "net del error,index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(dnet_del,
      dnet_del_cmd,
      "dnet del <0-99>",
      DNET_STR
      DEL_STR
      DNET_INDEX_STR)
{
    return dnet_cmd_del(vty, argv[0]);;
}

static int dnet_cmd_del_all(struct vty *vty)
{
    int ret = 0;
    int i;
    for ( i = 0; i < DNETSEG__RULE_NUM_MAX; i++ )
    {
        ret = api_dnet_del((uint32_t)i);
        if (ret)
        {
            vty_out(vty, "net del error,index(%d) ret(%d) %s", i, ret, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(dnet_del_all, 
      dnet_del_all_cmd,
      "dnet del all",
      DNET_STR
      DEL_STR
      DNET_ALL_STR)
{
    return dnet_cmd_del_all(vty);
}

static void dnet_dump(struct vty *vty, dnet_t *net)
{
    char action_str[NAGA_ACTION_STR_SZ] = {0};
    struct in_addr netmask;

    if ((NULL == vty) || (NULL == net))
    {
        return;
    }

    naga_action_string(&net->acl.actions, action_str);
    netmask.s_addr = htonl(net->mask);
    vty_out(vty, "%-8d %d.%d.%d.%d/%-15d	%-16s %-16lu %-16lu %-16lu%s", net->index,
            (net->ip >> 24) & 0xff,
            (net->ip >> 16) & 0xff,
            (net->ip >>  8) & 0xff,
            (net->ip >>  0) & 0xff,
            ip_masklen (netmask), action_str,
            (uint64_t) net->acl.cnt.cnt, (uint64_t) net->acl.vcnt.cnt,
            (uint64_t) net->acl.pushed_cnt.cnt, VTY_NEWLINE);
}

static int dnet_cmd_show(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;
    dnet_t net;
	uint8_t effect;
	
    if (index_str)
    {
        index = atoi(index_str);
        debug("index:%d", index);
    }

	memset(&net, 0, sizeof(dnet_t));

    ret = api_dnet_get(index, &net, &effect);
    if (ret)
    {
        vty_out(vty, "dnet dump fail, index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

	vty_out(vty, "%-8s %-23s 	%-16s %-16s %-20s %-20s %s","index", "ip/mask","action", "cnt", "none-drop", "pushed",VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------------------%s", VTY_NEWLINE);

	if (DNETSEG__RULE_EFFECTIVE == effect)
	{
    	dnet_dump(vty, &net);
	}
	else
	{
		vty_out(vty, "dnet %d does not exist!%s", index, VTY_NEWLINE);
	}
    return CMD_SUCCESS;
}


static int dnet_cmd_show_all(struct vty *vty)
{
    int ret = 0;
    int i;
    dnet_t net;
	uint8_t effect;

	memset(&net, 0, sizeof(dnet_t));
	vty_out(vty, "%-8s %-23s	%-16s %-16s %-20s %-20s %s","index", "ip/mask","action", "cnt", "none-drop", "pushed",VTY_NEWLINE);
    vty_out(vty, "----------------------------------------------------------------%s", VTY_NEWLINE);
    for ( i = 0; i < DNETSEG__RULE_NUM_MAX; i++ )
    {
 
        ret = api_dnet_get(i, &net, &effect);
        if (ret)
        {
            vty_out(vty, "dnet dump fail, index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
            continue;
        }
		if (DNETSEG__RULE_EFFECTIVE == effect)
		{
			dnet_dump(vty, &net);
		}
	}

    return CMD_SUCCESS;
}

DEFUN(dnet_show_by_index, 
      dnet_show_by_index_cmd,
      "show dnet <0-149>",
      SHOW_STR
      DNET_STR
      DNET_INDEX_STR)
{
    return dnet_cmd_show(vty, argv[0]);
}

DEFUN(dnet_show_all, 
      dnet_show_all_cmd,
      "show dnet all",
      SHOW_STR
      DNET_STR
      DNET_ALL_STR)
{
    return dnet_cmd_show_all(vty);
}


static int dnet_cmd_clear_statistics(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;

    index = atoi(index_str);
    debug("index:%d", index);

    ret = api_dnet_clear_statistics(index);
    if (ret)
    {
        vty_out(vty, "dnet clear statistics error,index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}
DEFUN(dnet_clear_statistics, 
      dnet_clear_statistics_cmd,
      "clear dnet stat <0-149>",
      CLEAR_STR
      DNET_STR
      STAT_STR
      DNET_INDEX_STR)
{
    return dnet_cmd_clear_statistics(vty, argv[0]);
}

static int dnet_cmd_clear_statistics_all(struct vty *vty)
{
    int ret = 0;
    int i;
    for ( i = 0; i < DNETSEG__RULE_NUM_MAX; i++ )
    {
        ret = api_dnet_clear_statistics((uint32_t)i);
        if (ret)
        {
            vty_out(vty, "dnet clear statistics error,index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}
DEFUN(dnet_clear_statistics_all,
      dnet_clear_statistics_all_cmd,
      "clear dnet stat all",
      CLEAR_STR
      DNET_STR
      STAT_STR
      DNET_ALL_STR)
{
    return dnet_cmd_clear_statistics_all(vty);
}


static int cmd_dnetseg_default_act_set(struct vty *vty, const char *act_str)
{
	int ret = 0;
	uint32_t action = 0;
	
	if(naga_action_parse((char *)act_str, &action))
    {
        return CMD_ERR_NO_MATCH;
    }
	ret = api_dnetseg_default_act_set(action);
	if (ret)
    {
        vty_out(vty, "dnetseg set default action fail:(%s)%s", berr_msg(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}



DEFUN(dnetseg_default_act_set,
      dnetseg_default_act_set_cmd,
      "dnet default ACT",
      DNET_STR
      DEFAULT_STR
      ACTION_STR)
{
    return cmd_dnetseg_default_act_set(vty, argv[0]);
}







void dnetseg_cmd_config_write(struct vty *vty)
{
    int ret = 0;
    char action_str[NAGA_ACTION_STR_SZ] = {0};
    struct in_addr netmask;
    dnet_t net;
	uint8_t effect = 0;
    int i;
	uint32_t action = 0;

	ret = api_dnetseg_default_act_get(&action);
	if (ret)
    {
        vty_out(vty, "dnetseg get default action fail:(%s)%s", berr_msg(ret), VTY_NEWLINE);
        return;
    }

	if (0 != action)
	{
		naga_action_string(&action, action_str);
		vty_out(vty, "net default %s%s", action_str, VTY_NEWLINE);
	}
    for ( i = 0; i < DNETSEG__RULE_NUM_MAX; i++ )
    {

        ret = api_dnet_get(i, &net, &effect);
        if (ret)
        {
            vty_out(vty, "net dump fail, index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
            continue;
        }
        else
        {
            naga_action_string(&net.acl.actions, action_str);
            netmask.s_addr = htonl(net.mask);
			
			if (DNETSEG__RULE_EFFECTIVE == effect)
			{
	            vty_out(vty, "dnet add %d %d.%d.%d.%d/%d %s %s", i, 
	                    (net.ip >> 24) & 0xff,
	                    (net.ip >> 16) & 0xff,
	                    (net.ip >>  8) & 0xff,
	                    (net.ip >>  0) & 0xff,
	                    ip_masklen (netmask), action_str,
	                    VTY_NEWLINE);
			}
		}

    }
}

/*
 * net module cmdline register and init 
 *
 * */
void cmdline_dnetseg_init(void)
{

    install_element(CMD_NODE, &dnet_add_cmd);
    install_element(CMD_NODE, &dnet_del_cmd);
    install_element(CMD_NODE, &dnet_del_all_cmd);
    install_element(CMD_NODE, &dnet_show_by_index_cmd);
    install_element(CMD_NODE, &dnet_show_all_cmd);
    install_element(CMD_NODE, &dnet_clear_statistics_cmd);
    install_element(CMD_NODE, &dnet_clear_statistics_all_cmd);
	install_element(CMD_NODE, &dnetseg_default_act_set_cmd);

    return ;
}
