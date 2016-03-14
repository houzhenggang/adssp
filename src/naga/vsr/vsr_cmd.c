/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/home/shan/work/rose/src/naga/vsr\vsr_cmd.c
#       @date         :2015/09/05 11:53
#       @algorithm    :
=============================================================================*/
#include <string.h>

#include "vsr_cmd_api.h"
#include "zebra.h"
#include "version.h"
#include "getopt.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"
#include "vsr_file.h"

#define RULE_STR                    "Access control list\n"
#define VSR_STR                     "Vister Record list\n"
#define ADD_STR                     "ADD Operation\n"
#define DEL_STR                     "Del Operation\n"
//#define SHOW_STR                    "Show entry content\n"
#define VSR_INDEX_STR               "Vister Record list index\n"
#define VSR_ALL_STR                 "All Vister Record list\n"
//#define IP_STR                      "IP address\n"
#define MOBILE_STR                  "Mobile phone number\n"
#define FLUSH_STR                   "Flush url\n"
//#define CLEAR_STR                   "Clear Operation\n"
#define STAT_STR                    "Statistics Operation\n"
#define ENCOURAGE_STR               "Encourage Operation, for test\n" 
#define TOTAL_STR                   "Summary Display\n"


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

static int vsr_cmd_add(struct vty *vty, const char *index_str, const char *ip_str, const char *mobile_str)
{
    int ret = 0;
    uint32_t index = 0;
    uint32_t ip    = 0;
    uint64_t mobile = 0;
    struct prefix p;

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

    if ( mobile_str )
    {
        mobile = (ULL) strtol(mobile_str, NULL, 10);
        debug("mobile: %llu", (ULL) mobile);
    }

    ret = rule_vsr_cmd_add(index, ip, mobile);
    if (ret)
    {
        vty_out(vty, "vsr add error, index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(vsr_add, 
      vsr_add_cmd,
      "rule vsr add <0-15> A.B.C.D",
      RULE_STR
      VSR_STR
      ADD_STR
      VSR_INDEX_STR
      IP_STR)
{
    return vsr_cmd_add(vty, argv[0], argv[1], NULL);
}

DEFUN(vsr_add_mobile,
      vsr_add_mobile_cmd,
      "rule vsr add <0-15> A.B.C.D MOBILE",
      RULE_STR
      VSR_STR
      ADD_STR
      VSR_INDEX_STR
      IP_STR
      MOBILE_STR)
{
    return vsr_cmd_add(vty, argv[0], argv[1], argv[2]);
}

static int vsr_cmd_del(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;

    index = atoi(index_str);
    debug("index:%d", index);

    ret = rule_vsr_cmd_del(index);
    if (ret)
    {
        vty_out(vty, "vsr del error,index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(vsr_del,
      vsr_del_cmd,
      "rule vsr del <0-15>",
      RULE_STR
      VSR_STR
      DEL_STR
      VSR_INDEX_STR)
{
    return vsr_cmd_del(vty, argv[0]);;
}

static int vsr_cmd_del_all(struct vty *vty)
{
    int ret = 0;
    int i;
    for ( i = 0; i < VSR_RULE_NUM_MAX; i++ )
    {
        ret = rule_vsr_cmd_del((uint32_t)i);
        if (ret)
        {
            vty_out(vty, "vsr del error,index(%d) ret(%d) %s", i, ret, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(vsr_del_all, 
      vsr_del_all_cmd,
      "rule vsr del all",
      RULE_STR
      VSR_STR
      DEL_STR
      VSR_ALL_STR)
{
    return vsr_cmd_del_all(vty);
}

static int vsr_cmd_show(struct vty *vty, const char *index_str, const char *ip_str, const char *mobile_str)
{
    int ret = 0;
    uint32_t index = 0;
    uint32_t ip    = 0;
    uint64_t mobile = 0;
    struct prefix p;
    uint8_t buff[VSR_URL_NUM_MAX * (VSR_URL_LEN_MAX * 2)];

    if (index_str)
    {
        index = atoi(index_str);
        debug("index:%d", index);
    }

    if (ip_str)
    {
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
        ret = rule_vsr_cmd_get_index_by_ip(ip, &index);
        if (ret)
        {
            vty_out(vty, "get index by ip fail, ip(%d.%d.%d.%d), ret(%d)%s", (ip >> 24) & 0xff,
                                                                             (ip >> 16) & 0xff,
                                                                             (ip >>  8) & 0xff,
                                                                             (ip >>  0) & 0xff,
                                                                             ret, VTY_NEWLINE);
            return CMD_WARNING;

        }
        debug("index:%d", index);
    }

    if ( mobile_str )
    {
        mobile = (ULL) strtol(mobile_str, NULL, 10);
        debug("mobile: %llu", (ULL) mobile);
        ret = rule_vsr_cmd_get_index_by_mobile(mobile, &index);
        if (ret)
        {
            vty_out(vty, "get index by mobile fail, mobile(%llu), ret(%d)%s", (ULL)mobile,
                                                                              ret, VTY_NEWLINE);
            return CMD_WARNING;
        }
        debug("index:%d", index);
    }

    ret = rule_vsr_cmd_dump(index, buff,sizeof(buff));
    if (ret)
    {
        vty_out(vty, "vsr dump fail, index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    vty_out(vty, "%s%s", buff, VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(vsr_show_by_index, 
      vsr_show_by_index_cmd,
      "show rule vsr index <0-15>",
      SHOW_STR
      RULE_STR
      VSR_STR
      VSR_INDEX_STR
      VSR_INDEX_STR)
{
    return vsr_cmd_show(vty, argv[0], NULL, NULL);
}

DEFUN(vsr_show_by_ip, 
      vsr_show_by_ip_cmd,
      "show rule vsr ip A.B.C.D",
      SHOW_STR
      RULE_STR
      VSR_STR
      IP_STR
      IP_STR)
{
    return vsr_cmd_show(vty, NULL, argv[0], NULL);
}

DEFUN(vsr_show_by_mobile, 
      vsr_show_by_mobile_cmd,
      "show rule vsr mobile MOBILE",
      SHOW_STR
      RULE_STR
      VSR_STR
      MOBILE_STR
      MOBILE_STR)
{
    return vsr_cmd_show(vty, NULL, NULL, argv[0]);
}

static int vsr_cmd_show_all(struct vty *vty)
{
    int ret = 0;
    int i;
    uint32_t index = 0;
    uint8_t buff[VSR_URL_NUM_MAX * (VSR_URL_LEN_MAX * 2)];

    for ( i = 0 ; i < VSR_RULE_NUM_MAX; i++ )
    {
        ret = rule_vsr_cmd_dump((uint32_t)i , buff, sizeof(buff));
        if (ret)
        {
            continue;
            //vty_out(vty, "vsr dump fail, index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
            //return CMD_WARNING;
        }

        vty_out(vty, "%s%s", buff, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}
DEFUN(vsr_show_all, 
      vsr_show_all_cmd,
      "show rule vsr all",
      SHOW_STR
      RULE_STR
      VSR_STR
      VSR_ALL_STR)
{
    return vsr_cmd_show_all(vty);
}

static int vsr_cmd_show_total(struct vty *vty)
{
    int ret = 0;
    int i;
    uint8_t buff[VSR_URL_NUM_MAX * (VSR_URL_LEN_MAX * 2)];

    ret = rule_vsr_cmd_total_dump((char *)buff, sizeof(buff));
    if (ret)
    {
        vty_out(vty, "vsr dump fail, ret(%d)%s", ret, VTY_NEWLINE);
        //return CMD_WARNING;
    }

    vty_out(vty, "%s%s", buff, VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(vsr_show_total,
      vsr_show_total_cmd,
      "show rule vsr total",
      SHOW_STR
      RULE_STR
      VSR_STR
      TOTAL_STR)
{
    return vsr_cmd_show_total(vty);
}
static int vsr_cmd_flush_url(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;

    index = atoi(index_str);
    debug("index:%d", index);

    ret = rule_vsr_cmd_flush_url(index);
    if (ret)
    {
        vty_out(vty, "vsr flush url error,index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}
DEFUN(vsr_flush_url, 
      vsr_flush_url_cmd,
      "rule vsr flush <0-15>",
      RULE_STR
      VSR_STR
      FLUSH_STR
      VSR_INDEX_STR)
{
    return vsr_cmd_flush_url(vty, argv[0]);
}

static int vsr_cmd_flush_url_all(struct vty *vty)
{
    int ret = 0;
    int i;
    for ( i = 0; i < VSR_RULE_NUM_MAX; i++ )
    {
        ret = rule_vsr_cmd_flush_url((uint32_t)i);
        if (ret)
        {
            vty_out(vty, "vsr flush url error,index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}
DEFUN(vsr_flush_url_all, 
      vsr_flush_url_all_cmd,
      "rule vsr flush all",
      RULE_STR
      VSR_STR
      FLUSH_STR
      VSR_ALL_STR)
{
    return vsr_cmd_flush_url_all(vty);
}

static int vsr_cmd_clear_statistics(struct vty *vty, const char *index_str)
{
    int ret = 0;
    uint32_t index = 0;

    index = atoi(index_str);
    debug("index:%d", index);

    ret = rule_vsr_cmd_clear_statistics(index);
    if (ret)
    {
        vty_out(vty, "vsr clear statistics error,index(%d) ret(%d)%s", index, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}
DEFUN(vsr_clear_statistics, 
      vsr_clear_statistics_cmd,
      "clear stat rule vsr <0-15>",
      CLEAR_STR
      STAT_STR
      RULE_STR
      VSR_STR
      VSR_INDEX_STR)
{
    return vsr_cmd_clear_statistics(vty, argv[0]);
}

static int vsr_cmd_clear_statistics_all(struct vty *vty)
{
    int ret = 0;
    int i;
    for ( i = 0; i < VSR_RULE_NUM_MAX; i++ )
    {
        ret = rule_vsr_cmd_clear_statistics((uint32_t)i);
        if (ret)
        {
            vty_out(vty, "vsr clear statistics error,index(%d) ret(%d)%s", i, ret, VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}
DEFUN(vsr_clear_statistics_all,
      vsr_clear_statistics_all_cmd,
      "clear stat rule vsr all",
      CLEAR_STR
      STAT_STR
      RULE_STR
      VSR_STR
      FLUSH_STR
      VSR_ALL_STR)
{
    return vsr_cmd_clear_statistics_all(vty);
}

static int vsr_cmd_encourage(struct vty *vty, const char *num_str)
{
    int ret = 0;
    uint32_t num = 0;

    num = atoi(num_str);
    debug("num:%d", num);

    ret = rule_vsr_cmd_encourage(num);
    if (ret)
    {
        vty_out(vty, "vsr encourage error, num(%d) ret(%d)%s", num, ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}
DEFUN(vsr_encourage, 
      vsr_encourage_cmd,
      "rule vsr encourage <0-1000>",
      RULE_STR
      VSR_STR
      ENCOURAGE_STR
     "Test packet number\n" 
      )
{
    return vsr_cmd_encourage(vty, argv[0]);
}


static int vsr_cmd_file(struct vty *vty)
{
    vsr_tab_file();
    return CMD_SUCCESS;
}

DEFUN(vsr_file,
      vsr_file_cmd,
      "rule vsr entry file",
      RULE_STR
      VSR_STR
      "Table Entry\n"
      "Write File\n"
      )
{
    return vsr_cmd_file(vty);
}

void vsr_cmd_config_write(struct vty *vty)
{
    int ret = 0;
    int i;
    uint32_t ip;
    uint64_t mobile;
    for ( i = 0; i < VSR_RULE_NUM_MAX; i++ )
    {
        ret = rule_vsr_cmd_get_ip((uint32_t) i, &ip);
        if (ret)
        {
            continue;
        }

        if ( VSR_RULE_IP_UNEFFECTIVE == ip )
        {
            continue;
        }

        ret =  rule_vsr_cmd_get_mobile((uint32_t) i, &mobile);
        if (ret)
        {
            vty_out(vty, "rule vsr add %d %d.%d.%d.%d%s", i, (ip >> 24) & 0xff,
                                                             (ip >> 16) & 0xff,
                                                             (ip >> 8 ) & 0xff,
                                                             (ip >> 0 ) & 0xff, VTY_NEWLINE);
            continue;
        }

        if ( VSR_RULE_MOBILE_UNEFFECTIVE != mobile )
        {
            vty_out(vty, "rule vsr add %d %d.%d.%d.%d %llu%s", i, (ip >> 24) & 0xff,
                                                                  (ip >> 16) & 0xff,
                                                                  (ip >> 8 ) & 0xff,
                                                                  (ip >> 0 ) & 0xff, (ULL)mobile, VTY_NEWLINE);
            continue;
        }
        else
        {
            vty_out(vty, "rule vsr add %d %d.%d.%d.%d%s", i, (ip >> 24) & 0xff,
                                                             (ip >> 16) & 0xff,
                                                             (ip >> 8 ) & 0xff,
                                                             (ip >> 0 ) & 0xff, VTY_NEWLINE);
            continue;
        }

    }
}












/*
 * vsr module cmdline register and init 
 *
 * */
void cmdline_vsr_init(void)
{

    install_element(CMD_NODE, &vsr_add_cmd);
    install_element(CMD_NODE, &vsr_add_mobile_cmd);

    install_element(CMD_NODE, &vsr_del_cmd);
    install_element(CMD_NODE, &vsr_del_all_cmd);

    install_element(CMD_NODE, &vsr_show_by_index_cmd);
    install_element(CMD_NODE, &vsr_show_all_cmd);
    install_element(CMD_NODE, &vsr_show_total_cmd);
    install_element(CMD_NODE, &vsr_show_by_ip_cmd);
    install_element(CMD_NODE, &vsr_show_by_mobile_cmd);

    install_element(CMD_NODE, &vsr_flush_url_cmd);
    install_element(CMD_NODE, &vsr_flush_url_all_cmd);

    install_element(CMD_NODE, &vsr_clear_statistics_cmd);
    install_element(CMD_NODE, &vsr_clear_statistics_all_cmd);

    install_element(CMD_NODE, &vsr_encourage_cmd);
    install_element(CMD_NODE, &vsr_file_cmd);

    return ;
}
