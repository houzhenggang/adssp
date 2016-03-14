/**************************************************************
* Copyright (C) 2014-2015 All rights reserved.
* @Version: 1.0
* @Created: 2015-10-11 11:44
* @Author: SamLiu - shan3275@gmail.com
* @Description: 
*
* @History: 
**************************************************************/
#include <string.h>
#include "zebra.h"
#include "version.h"
#include "getopt.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"

#include "packet_http.h"

#define AD_STR       "Advertisement Operator\n"
#define TEMPLATE_STR "Template Operator\n"

static int adt_cmd_show(struct vty *vty)
{
    int i,j;
    http_body_t *http_body = NULL;

    http_body = adt_get_http_body();
    if ( NULL == http_body )
    {
        vty_out(vty, "ad template NULL %s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    for( i = 0; i < AD_TEMPLATE_MAX; i++ )
    {
        vty_out(vty,"Template(%d):%s", i, VTY_NEWLINE);
        vty_out(vty,"name(%d):%s%s",   (int)strlen(http_body[i].name), http_body[i].name, VTY_NEWLINE);
        vty_out(vty,"head(%d):%s", (int)strlen(http_body[i].head), VTY_NEWLINE);
        for ( j = 0; j < (int) strlen(http_body[i].head); j++ )
        {
            if ( '\n' == http_body[i].head[j])
            {
                vty_out(vty, "%s", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%c", http_body[i].head[j]);
            }
        }
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty,"url(%d):%s",  (int)strlen(http_body[i].url), VTY_NEWLINE);
        for ( j = 0; j < (int) strlen(http_body[i].url); j++ )
        {
            if ( '\n' == http_body[i].url[j])
            {
                vty_out(vty, "%s", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%c", http_body[i].url[j]);
            }
        }
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty,"tail(%d):%s", (int)strlen(http_body[i].tail), VTY_NEWLINE);
        for ( j = 0; j < (int) strlen(http_body[i].tail); j++ )
        {
            if ( '\n' == http_body[i].tail[j])
            {
                vty_out(vty, "%s", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%c", http_body[i].tail[j]);
            }
        }
    }
    vty_out(vty, "%s", VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN(adt_show, 
      adt_show_cmd,
      "show ad template",
      SHOW_STR
      AD_STR
      TEMPLATE_STR
      )
{
    return adt_cmd_show(vty);
}


static int adt_cmd_load(struct vty *vty, const char * dir_str)
{
    int ret = 0;
    ad_template_em template;

    if ( strstr(dir_str, "pc.template"))
    {
        template = AD_TEMPLATE_PC;
    }
    else
    if ( strstr(dir_str, "mobile.template"))
    {
        template = AD_TEMPLATE_MOBILE;
    }
    else
    {
        vty_out(vty, "ad template error, must pc.template or mobile.template %s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    ret = adt_set(template, dir_str);
    if ( ret )
    {
        vty_out(vty, "adt_set error ret(%d)%s", ret, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(adt_load,
      adt_load_cmd,
      "ad template load FILENAME",
      AD_STR
      TEMPLATE_STR
      "Load Operator\n"
      "File Name, must named pc.template or mobile.template\n"
      )
{
    return adt_cmd_load(vty, argv[0]);
}

static int adt_cmd_send(struct vty *vty, const char * dir_str)
{
    int ret = 0;
    adt_send_em send;

    if ( dir_str[0] == 's')
    {
        send = ADT_SEND_SINGLE;
    }
    else
    if ( dir_str[0] == 'm')
    {
        send = ADT_SEND_MULTI;
    }
    else
    {
        vty_out(vty, "ad template send parameter error %s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    ret = adt_send_set(send);
    if ( ret )
    {
        vty_out(vty, "adt_send error ret(%d)%s", ret, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(adt_send,
      adt_send_cmd,
      "ad template send (single|multi)",
      AD_STR
      TEMPLATE_STR
      "Send Operator\n"
      "Single or Multi packets\n"
      )
{
    return adt_cmd_send(vty, argv[0]);
}

static int adt_cmd_send_show(struct vty *vty)
{
    int rv;
    uint8_t send;

    rv = adt_get_send(&send);
    if ( rv )
    {
        vty_out(vty, "ad template get_sned fail rv(%d) %s", rv, VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    vty_out(vty, "%s%s", send == ADT_SEND_SINGLE ?"single":"multi", VTY_NEWLINE);
    return CMD_SUCCESS;
}

DEFUN(adt_send_show, 
      adt_send_show_cmd,
      "show ad template send",
      SHOW_STR
      AD_STR
      TEMPLATE_STR
      "Send Operator\n"
      )
{
    return adt_cmd_send_show(vty);
}
void adt_cmd_config_write(struct vty *vty)
{
    int i;
    int rv;
    http_body_t *http_body = NULL;
    uint8_t send;

    http_body = adt_get_http_body();
    if ( NULL == http_body )
    {
        vty_out(vty, "ad template NULL %s", VTY_NEWLINE);
        return ;
    }

    for( i = 0; i < AD_TEMPLATE_MAX; i++ )
    {
        if ( strcmp(http_body[i].name, "pc.html")
                && strcmp(http_body[i].name, "mobile.html"))
        {
            vty_out(vty, "ad template load %s%s", http_body[i].name, VTY_NEWLINE);
        }
    }

    rv = adt_get_send(&send);
    if ( rv )
    {
        vty_out(vty, "ad template get_sned fail rv(%d) %s", rv, VTY_NEWLINE);
        return ;
    }
    vty_out(vty, "ad template send %s%s", send == ADT_SEND_SINGLE ?"single":"multi", VTY_NEWLINE);
}


/*
 * vsr module cmdline register and init 
 *
 * */
void cmdline_adt_init(void)
{
    install_element(CMD_NODE, &adt_load_cmd);
    install_element(CMD_NODE, &adt_show_cmd);

    install_element(CMD_NODE, &adt_send_cmd);
    install_element(CMD_NODE, &adt_send_show_cmd);
    return ;
}


