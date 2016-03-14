/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/home/shan/work/rose/src/naga/vsr\net_cmd.c
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
#include "netseg.h"
#include "naga_cmd.h"
#include "naga_util.h"
#include "bts_util.h"

#define NET_STR                     "Net list\n"
#define ADD_STR                     "ADD Operation\n"
#define DEL_STR                     "Del Operation\n"
#define NET_INDEX_STR               "Vister Record list index\n"
#define NET_ALL_STR                 "All Net list\n"


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

static int net_cmd_add(struct vty *vty, const char *index_str, const char *ip_str, const char *act_str)
{
    int ret = 0;
    uint32_t index = 0;
    uint32_t ip    = 0;
    uint32_t u_mask    = 0;
    struct prefix p;
    struct in_addr mask;
    net_t net;

    memset(&net, 0 ,sizeof(net_t));

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

    ret = api_net_add(&net);
    if (ret)
    {
        vty_out(vty, "net add error, index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(net_add,
      net_add_cmd,
      "snet add <0-149> A.B.C.D/M ACT",
      NET_STR
      ADD_STR
      NET_INDEX_STR
      "IP information, like A.B.C.D/M\n"
      ACTION_STR
      )
{
    return net_cmd_add(vty, argv[0], argv[1], argv[2]);
}

static int net_cmd_del(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;

    index = atoi(index_str);
    debug("index:%d", index);

    ret = api_net_del(index);
    if (ret)
    {
        vty_out(vty, "net del error,index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(net_del,
      net_del_cmd,
      "snet del <0-149>",
      NET_STR
      DEL_STR
      NET_INDEX_STR)
{
    return net_cmd_del(vty, argv[0]);;
}

static int net_cmd_del_all(struct vty *vty)
{
    int ret = 0;
    int i;
    for ( i = 0; i < NETSEG_RULE_NUM_MAX; i++ )
    {
        ret = api_net_del((uint32_t)i);
        if (ret)
        {
            vty_out(vty, "net del error,index(%d) ret(%d) %s", i, ret, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(net_del_all, 
      net_del_all_cmd,
      "snet del all",
      NET_STR
      DEL_STR
      NET_ALL_STR)
{
    return net_cmd_del_all(vty);
}

static void net_dump(struct vty *vty, net_t *net)
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

static int net_cmd_show(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;
    net_t net;
	uint8_t effect;
	
    if (index_str)
    {
        index = atoi(index_str);
        debug("index:%d", index);
    }

	memset(&net, 0, sizeof(net_t));

    ret = api_net_get(index, &net, &effect);
    if (ret)
    {
        vty_out(vty, "net dump fail, index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

	vty_out(vty, "%-8s %-23s 	%-16s %-16s %-20s %-20s %s","index", "ip/mask","action", "cnt", "none-drop", "pushed",VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------------------%s", VTY_NEWLINE);

	if (NETSEG_RULE_EFFECTIVE == effect)
	{
    	net_dump(vty, &net);
	}
	else
	{
		vty_out(vty, "Snet %d does not exist!%s", index, VTY_NEWLINE);
	}
    return CMD_SUCCESS;
}


static int net_cmd_show_all(struct vty *vty)
{
    int ret = 0;
    int i;
    net_t net;
	uint8_t effect;
    
    uint64_t cnt_t=0;
    uint64_t none_drop_t= 0;
    uint64_t pushed_t = 0;
    

    memset(&net, 0, sizeof(net_t));
	vty_out(vty, "%-8s %-23s	%-16s %-16s %-20s %-20s %s","index", "ip/mask","action", "cnt", "none-drop", "pushed",VTY_NEWLINE);
    vty_out(vty, "----------------------------------------------------------------%s", VTY_NEWLINE);
    for ( i = 0; i < NETSEG_RULE_NUM_MAX; i++ )
    {
 
        ret = api_net_get(i, &net, &effect);
        if (ret)
        {
            vty_out(vty, "net dump fail, index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
            continue;
        }
		if (NETSEG_RULE_EFFECTIVE == effect)
		{
			net_dump(vty, &net);
            cnt_t += net.acl.cnt.cnt;
            none_drop_t += net.acl.vcnt.cnt;
            pushed_t += net.acl.pushed_cnt.cnt;
		}
	}
    vty_out(vty, "===================================================================================%s", VTY_NEWLINE);
    vty_out(vty, "%-8s %-23s	%-16s %-16lu %-16lu %-16lu%s","Total", "NULL", "NULL",
                                            cnt_t, none_drop_t, pushed_t, VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN(net_show_by_index, 
      net_show_by_index_cmd,
      "show snet <0-99>",
      SHOW_STR
      NET_STR
      NET_INDEX_STR)
{
    return net_cmd_show(vty, argv[0]);
}

DEFUN(net_show_all, 
      net_show_all_cmd,
      "show snet all",
      SHOW_STR
      NET_STR
      NET_ALL_STR)
{
    return net_cmd_show_all(vty);
}


static int net_cmd_clear_statistics(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;

    index = atoi(index_str);
    debug("index:%d", index);

    ret = api_net_clear_statistics(index);
    if (ret)
    {
        vty_out(vty, "net clear statistics error,index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}
DEFUN(net_clear_statistics, 
      net_clear_statistics_cmd,
      "clear snet stat <0-149>",
      CLEAR_STR
      NET_STR
      STAT_STR
      NET_INDEX_STR)
{
    return net_cmd_clear_statistics(vty, argv[0]);
}

static int net_cmd_clear_statistics_all(struct vty *vty)
{
    int ret = 0;
    int i;
    for ( i = 0; i < NETSEG_RULE_NUM_MAX; i++ )
    {
        ret = api_net_clear_statistics((uint32_t)i);
        if (ret)
        {
            vty_out(vty, "net clear statistics error,index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}
DEFUN(net_clear_statistics_all,
      net_clear_statistics_all_cmd,
      "clear snet stat all",
      CLEAR_STR
      NET_STR
      STAT_STR
      NET_ALL_STR)
{
    return net_cmd_clear_statistics_all(vty);
}


static int cmd_netseg_default_act_set(struct vty *vty, const char *act_str)
{
	int ret = 0;
	uint32_t action = 0;
	
	if(naga_action_parse((char *)act_str, &action))
    {
        return CMD_ERR_NO_MATCH;
    }
	ret = api_netseg_default_act_set(action);
	if (ret)
    {
        vty_out(vty, "netseg set default action fail:(%s)%s", berr_msg(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}



DEFUN(netseg_default_act_set,
      netseg_default_act_set_cmd,
      "snet default ACT",
      NET_STR
      DEFAULT_STR
      ACTION_STR)
{
    return cmd_netseg_default_act_set(vty, argv[0]);
}







static int cmd_write_snet_file(struct vty *vty, const char *file_name)
{
	FILE *fp = NULL;
	int ret = 0;
	int i;
	net_t net;
	uint8_t effect = 0;
	char action_str[NAGA_ACTION_STR_SZ] = {0};
    struct in_addr netmask;

	if (NULL == file_name)
	{
		return CMD_ERR_NOTHING_TODO;
	}
   
	fp = fopen(file_name, "w+");
	if (NULL == fp)
	{
		vty_out(vty, "Open the file %s failed!%s\n", file_name, VTY_NEWLINE);
		return CMD_ERR_NOTHING_TODO;
	}
	
	memset(&net, 0, sizeof(net_t));
	fprintf(fp, "%-8s %-23s	%-16s %-16s %-20s %-20s\n","index", "ip/mask","action", "cnt", "none-drop", "pushed");
	
	for ( i = 0; i < NETSEG_RULE_NUM_MAX; i++ )
	{
	
		ret = api_net_get(i, &net, &effect);
		if (ret)
		{
			vty_out(vty, "net dump fail, index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
			continue;
		}
		if (NETSEG_RULE_EFFECTIVE == effect)
		{
    		naga_action_string(&net.acl.actions, action_str);
    		netmask.s_addr = htonl(net.mask);
   			fprintf(fp, "%-8d %d.%d.%d.%d/%-15d	%-16s %-16lu %-16lu %-16lu\n", net.index,
            (net.ip >> 24) & 0xff,
            (net.ip >> 16) & 0xff,
            (net.ip >>  8) & 0xff,
            (net.ip >>  0) & 0xff,
            ip_masklen (netmask), action_str,
            (uint64_t) net.acl.cnt.cnt, (uint64_t) net.acl.vcnt.cnt,
            (uint64_t) net.acl.pushed_cnt.cnt);
		}
	}

	fclose(fp);

	return CMD_SUCCESS;
}




DEFUN(write_snet,
      write_snet_cmd,
      "write snet FILE",
      WRITE_STR
      NET_STR
      FILE_STR)
{
    return cmd_write_snet_file(vty, argv[0]);
}







void netseg_cmd_config_write(struct vty *vty)
{
    int ret = 0;
    char action_str[NAGA_ACTION_STR_SZ] = {0};
    struct in_addr netmask;
    net_t net;
	uint8_t effect = 0;
    int i;
	uint32_t action = 0;

	ret = api_netseg_default_act_get(&action);
	if (ret)
    {
        vty_out(vty, "netseg get default action fail:(%s)%s", berr_msg(ret), VTY_NEWLINE);
        return;
    }

	if (0 != action)
	{
		naga_action_string(&action, action_str);
		vty_out(vty, "snet default %s%s", action_str, VTY_NEWLINE);
	}
    for ( i = 0; i < NETSEG_RULE_NUM_MAX; i++ )
    {

        ret = api_net_get(i, &net, &effect);
        if (ret)
        {
            vty_out(vty, "snet dump fail, index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
            continue;
        }
        else
        {
            naga_action_string(&net.acl.actions, action_str);
            netmask.s_addr = htonl(net.mask);
			
			if (NETSEG_RULE_EFFECTIVE == effect)
			{
	            vty_out(vty, "snet add %d %d.%d.%d.%d/%d %s %s", i, 
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
void cmdline_netseg_init(void)
{

    install_element(CMD_NODE, &net_add_cmd);
    install_element(CMD_NODE, &net_del_cmd);
    install_element(CMD_NODE, &net_del_all_cmd);
    install_element(CMD_NODE, &net_show_by_index_cmd);
    install_element(CMD_NODE, &net_show_all_cmd);
    install_element(CMD_NODE, &net_clear_statistics_cmd);
    install_element(CMD_NODE, &net_clear_statistics_all_cmd);
	install_element(CMD_NODE, &netseg_default_act_set_cmd);
	install_element(CMD_NODE, &write_snet_cmd);

    return ;
}
