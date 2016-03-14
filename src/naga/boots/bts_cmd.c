/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/naga/boots\bts_cmd.c
#       @date         :2015-09-03 13:32
#       @algorithm    :
=============================================================================*/
#include "zebra.h"
#include "version.h"
#include "getopt.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"

#include "bts_cnt.h"
#include "bts_debug.h"
#include "bts_util.h"

#define STAT_STR        "Statistics Operation\n"
#define MODULE_STR      "module of this system\n"
#define MODULE_NAME_STR "name of module\n"
#define MATCH_STR_STR   "string match in stat name\n"

#define BTS_DEBUG_STR           "debug function\n"
#define DEBUG_OPTION_STR        "debug option\n"
#define DEBUG_OPTION_NAME_STR   "name of debug option\n"

static int cmd_clear_stat(struct vty *vty, const char *str)
{
    int i;
    uint32_t total = 0;

    for (i = 0; i < CNT_MAX; i++) 
    {
        if (str)  
        {
            if (NULL == strstr(cnt_array[i].name, str))
            {
                continue;
            }
        }

        cnt_clear(i, 1, &total);
    }

    return CMD_SUCCESS;
}

DEFUN(clear_stat,
      clear_stat_cmd,
      "clear stat [STR]",
      CLEAR_STR
      STAT_STR
      MATCH_STR_STR
      )
{
    return cmd_clear_stat(vty, argv[0]);
}

static int cmd_show_stat(struct vty *vty, const char *str)
{

    int i;
    uint32_t total = 0;
    cnt_t cnt_array[CNT_MAX];
	
    if (E_SUCCESS != cnt_get(0, CNT_MAX, cnt_array, &total))
    {
    	vty_out(vty, "Fail To Get cnt_get %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    for ( i = 0; i < CNT_MAX; i++)
    {
        if (str)  
        {
            if (NULL == strstr(cnt_array[i].name, str))
            {
                continue;
            }
        }

        vty_out(vty, "%-40s: %lld%s", cnt_array[i].name, (ULL)cnt_array[i].val.cnt, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(show_stat,
      show_stat_cmd,
      "show stat [STR]",
      SHOW_STR
      STAT_STR
      MATCH_STR_STR
      )
{
	if(argc == 1)		
    	return cmd_show_stat(vty, argv[0]);
	else
		return cmd_show_stat(vty, NULL);
}

void 
vty_module_dump(struct vty *vty, mod_t *module)
{
    char enable_str[BSWT_STR_SZ];
    char debug_str[BSWT_STR_SZ];

    if ((NULL == vty) || (NULL == module))
    {
        return;
    }

    bswt_string(&module->enable, enable_str);
    bswt_string(&module->debug, debug_str);

    vty_out(vty, "(%d)%3s    %-3s    %-3s    %s%s", 
            module->mod, module->abbr, enable_str, 
            debug_str, module->desc, VTY_NEWLINE);
}

static int
cmd_show_module(struct vty *vty, const char *name)
{
    mod_t *module = NULL;
    if (NULL == name)
    {
        int i;
        for (i = 0; i < MOD_MAX; i++)
        {
            module = &mod_array[i];
            vty_module_dump(vty, module);
        }
    }
    else
    {
        module = mod_lookup((char *)name);
        if (NULL == module)
        {
            vty_out(vty, "%%module %s not found!%s", name, VTY_NEWLINE);
            return CMD_WARNING;
        }
        vty_module_dump(vty, module); 
    }

    return CMD_SUCCESS;
}

DEFUN(show_module,
      show_module_cmd,
      "show module [NAME]",
      SHOW_STR
      MODULE_STR
      MODULE_NAME_STR
      )
{
    return cmd_show_module(vty, argv[0]);
}

static int 
cmd_module(struct vty *vty, const char *name, const char *enable_str, const char *debug_str)
{
    bswt enable = 0, debug = 0;
    berr e;

    if (bswt_parse((char *)enable_str, &enable))
    {
        vty_out(vty, "%%invald options %s%s", enable_str, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (bswt_parse((char *)debug_str, &debug))
    {
        vty_out(vty, "%%invald options %s%s", debug_str, VTY_NEWLINE);
        return CMD_WARNING;
    }

    e = mod_modify((char *)name, enable, debug);
    if (E_FOUND == e)
    {
        vty_out(vty, "%%module %s not found!%s", name, VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (E_SUCCESS != e)
    {
        vty_out(vty, "%%modify module %s fail!%s", name, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(module,
      module_cmd,
      "module NAME (on|off) (on|off)",
      MODULE_STR
      MODULE_NAME_STR
      "enable of this module\n""disable of this module\n"
      "turn on debug info\n""turn off debug info\n")
{
    return cmd_module(vty, argv[0], argv[1], argv[2]);
}

/*
 *  debug option command
 *
 */
void 
vty_dopt_dump(struct vty *vty, dopt_t *dopt)
{
    char enable_str[BSWT_STR_SZ];

    if ((NULL == vty) || (NULL == dopt))
    {
        return;
    }

    bswt_string(&dopt->enable, enable_str);

    vty_out(vty, "(%d)%-10s    %-3s    %s%s", 
            dopt->idx, dopt->name, enable_str, 
            dopt->desc, VTY_NEWLINE);
}

static int
cmd_show_dopt(struct vty *vty, const char *name)
{
    dopt_t *dopt = NULL;
    if (NULL == name)
    {
        int i;
        for (i = 0; i < DOPT_MAX; i++)
        {
            dopt = &dopt_array[i];
            vty_dopt_dump(vty, dopt);
        }
    }
    else
    {
        dopt = dopt_lookup((char *)name);
        if (NULL == dopt)
        {
            vty_out(vty, "%%dopt %s not found!%s", name, VTY_NEWLINE);
            return CMD_WARNING;
        }
        vty_dopt_dump(vty, dopt); 
    }

    return CMD_SUCCESS;
}

DEFUN(show_debug_option,
      show_debug_option_cmd,
      "show debug option [NAME]",
      SHOW_STR
      BTS_DEBUG_STR
      DEBUG_OPTION_STR
      DEBUG_OPTION_NAME_STR
      )
{
    return cmd_show_dopt(vty, argv[0]);
}

static int 
cmd_dopt(struct vty *vty, const char *name, const char *enable_str)
{
    bswt enable = 0;
    berr e;

    if (bswt_parse((char *)enable_str, &enable))
    {
        vty_out(vty, "%%invald options %s%s", enable_str, VTY_NEWLINE);
        return CMD_WARNING;
    }

    e = dopt_modify((char *)name, enable);
    if (E_FOUND == e)
    {
        vty_out(vty, "%%dopt %s not found!%s", name, VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (E_SUCCESS != e)
    {
        vty_out(vty, "%%doptify dopt %s fail!%s", name, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(debug_option,
      debug_option_cmd,
      "debug option NAME (on|off)",
      BTS_DEBUG_STR
      DEBUG_OPTION_STR
      DEBUG_OPTION_NAME_STR
      "enable of this dopt\n""disable of this debug option\n")
{
    return cmd_dopt(vty, argv[0], argv[1]);
}

/*
 * bts_cnt  module cmdline register and init 
 *
 * */
void cmdline_bts_init(void)
{
    install_element(CMD_NODE, &show_stat_cmd);
    install_element(CMD_NODE, &clear_stat_cmd);

    install_element(CMD_NODE, &module_cmd);
    install_element(CMD_NODE, &show_module_cmd);

    install_element(CMD_NODE, &debug_option_cmd);
    install_element(CMD_NODE, &show_debug_option_cmd);

    return ;
}

/* End of file */
