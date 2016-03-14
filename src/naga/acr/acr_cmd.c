/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :gongjianhua
#       @name         :Sam Liu
#       @file         :/home/shan/work/rose/src/naga/vsr\vsr_cmd.c
#       @date         :2015/09/05 11:53
#       @algorithm    :
=============================================================================*/
#include <string.h>

#include "zebra.h"
#include "getopt.h"
#include "command.h"

#include "boots.h"
#include "bts_debug.h"

#include "naga_types.h"
#include "naga_util.h"
#include "naga_cmd.h"

#include "acr.h"

#include "version.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"

#define ACCOUNT_STR					"Account rule\n"
#define ACCOUNT_NAME_STR             "Name of account\n" 
#define ACCOUNT_ALL_STR             "All the account\n"


#define ACR_DEBUG
#ifdef  ACR_DEBUG
#define acr_debug(fmt,args...)  vty_out(vty, "func=%s line=%d:" fmt "%s", __FUNCTION__, __LINE__, ##args, VTY_NEWLINE)
#else
#define acr_debug(fmt,args...)
#endif  /* DEBUG */



static acr_t *acr_entry_new(void)
{
	acr_t *entry = NULL;

	entry = malloc(sizeof(acr_t));
	if (NULL != entry)
	{
		memset(entry, 0, sizeof(acr_t));
	}

	return entry;
}

void
acr_dump_vty(void *data, void *param)
{
    acr_t *entry = NULL;
    struct vty *vty = NULL;

    char action_str[NAGA_ACTION_STR_SZ];

    if ((NULL == param) || (NULL == data))
    {
        return;
    }

    entry = (acr_t *) data;
    vty   = (struct vty *) param;

    naga_action_string(&entry->acl.actions, action_str);

    vty_out(vty, "%-32s %-32s %-16ld %s", entry->account, action_str, 
            (uint64_t) entry->acl.cnt.cnt, VTY_NEWLINE);
}

static int cmd_acr_show(struct vty *vty, const char *account)
{
	int ret = 0;

	acr_t *entry = NULL;

	if (NULL == account)
	{
        return CMD_ERR_NOTHING_TODO;
	}

    entry = api_acr_get((char *)account);
    if (NULL == entry)
    {
        vty_out(vty, "acr account<%s> empty%s", account, VTY_NEWLINE);
        return CMD_WARNING;
    }
	vty_out(vty, "%-32s %-32s %-16s %s","account","action", "cnt",VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------%s", VTY_NEWLINE);

    acr_dump_vty((void *)entry, (void *)vty);
   
    return CMD_SUCCESS;
}


static int cmd_acr_show_all(struct vty *vty)
{
	int ret = 0;

	vty_out(vty, "%-32s %-32s %-16s %s","account","action", "cnt",VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------%s", VTY_NEWLINE);
	
    acr_iter(acr_dump_vty, vty);

   

    return CMD_SUCCESS;
}




DEFUN(show_account,
      show_account_cmd,
      "show account NAME",
      SHOW_STR
      ACCOUNT_STR
      ACCOUNT_NAME_STR)
{
    return cmd_acr_show(vty, argv[0]);
}


DEFUN(show_account_all,
      show_account_all_cmd,
      "show account all",
      SHOW_STR
      ACCOUNT_STR
      ACCOUNT_ALL_STR)
{
    return cmd_acr_show_all(vty);
}



static int cmd_acr_del(struct vty *vty, const char *account)
{
    int ret = 0;
	
	if (NULL == account)
	{
        return CMD_ERR_NOTHING_TODO;
	}

    ret = api_acr_del((char *)account);

    if (ret)
    {
        vty_out(vty, "acr del  %s fail:(%s)%s", account, berr_msg(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


static int cmd_acr_del_all(struct vty *vty)
{
	int ret = 0;
	ret = api_acr_clear();
	if (ret)
    {
        vty_out(vty, "acr del all account fail:(%s)%s", berr_msg(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(remove_account, 
      remove_account_cmd,
      "remove account NAME",
      REMOVE_STR
      ACCOUNT_STR
      ACCOUNT_NAME_STR)
{
    return cmd_acr_del(vty, argv[0]);
}

DEFUN(remove_account_all, 
      remove_account_all_cmd,
      "remove account all",
      REMOVE_STR
      ACCOUNT_STR
      ACCOUNT_ALL_STR)
{
    return cmd_acr_del_all(vty);
}


static int cmd_acr_add(struct vty *vty, const char *account, const char *action_str)
{
    berr ret = 0;
	char action_arry[NAGA_ACTION_STR_SZ]= {0};
	acr_t *entry = NULL;
		
	if ((NULL == account) || (NULL == action_str))
	{
        return CMD_ERR_NO_MATCH;
	}

    entry = acr_entry_new();
	if (NULL == entry)
	{
        return CMD_ERR_NO_MATCH;
	}
	sprintf(action_arry, "%s", action_str);
	if(naga_action_parse((char *)action_arry, &entry->acl.actions))
    {
        return CMD_ERR_NO_MATCH;
    }
	
	entry->account_len = strlen(account);
	memcpy(entry->account, account, entry->account_len);

    ret = api_acr_add(entry);
    if (ret)
    {
        vty_out(vty, "acr add account fail: ret(%s)%s", berr_msg(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

/*account operation*/

DEFUN(account, 
      account_cmd,
      "account NAME ACT",
      ACCOUNT_STR
      ACCOUNT_NAME_STR
      ACTION_STR)
{
    return cmd_acr_add(vty, argv[0], argv[1]);
}

static int cmd_acr_load(struct vty *vty, const char *file_name, const char *action_str)
{
	FILE *fp = NULL;
	char account_line[MAX_ACCOUNT_LEN] = {0};
	int rv = 0;
	char *p = NULL;

	fp = fopen(file_name, "r");
	if (NULL == fp)
	{
		acr_debug("Open the file %s failed!\n", file_name);
		return CMD_ERR_NOTHING_TODO;
	}

	while(NULL != fgets(account_line, MAX_ACCOUNT_LEN, fp))
	{
		if ('#' == account_line[0])
		{
			continue;
		}

		if (NULL != (p = strchr(account_line, '\n')))
		{
            *p = '\0';
		}	

		rv = cmd_acr_add(vty, account_line, action_str);
		if (CMD_SUCCESS != rv)
		{
			acr_debug("Add account %s rule failed!\n", account_line);
			return rv;
		}
	}

	fclose(fp);

	return CMD_SUCCESS;
}


DEFUN(load_account,
      load_account_cmd,
      "load account FILE",
      LOAD_STR
      ACCOUNT_STR
      FILE_STR)
{
    return cmd_acr_load(vty, argv[0], argv[1]);
}

static int cmd_acr_clear_stat(struct vty *vty, const char *account)
{
	int ret = 0;

    if (NULL == account)
    {
        return CMD_ERR_NOTHING_TODO;
    }
	
    ret = api_acr_stat_clear((char *)account); 
	if (ret)
    {
        vty_out(vty, "acr clear account %s rule stat fail:(%s)%s", account, berr_msg(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;

}


static int cmd_acr_clear_stat_all(struct vty *vty)
{
	int ret = 0;
	ret = api_acr_stat_clear_all();
	if (ret)
    {
        vty_out(vty, "acr clear all account rule stat fail:(%s)%s", berr_msg(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


DEFUN(clear_account_stat,
      clear_account_stat_cmd,
      "clear account stat ACCOUNT",
      CLEAR_STR
      ACCOUNT_STR
      STAT_STR
      ACCOUNT_STR)
{
    return cmd_acr_clear_stat(vty, argv[0]);
}

DEFUN(clear_account_stat_all,
      clear_account_stat_all_cmd,
      "clear account stat all",
      CLEAR_STR
      ACCOUNT_STR
      STAT_STR
      ACCOUNT_ALL_STR)
{
    return cmd_acr_clear_stat_all(vty);
}


static int cmd_acr_account_default_act_set(struct vty *vty, const char *act_str)
{
	int ret = 0;
	uint32_t action = 0;
	
	if(naga_action_parse((char *)act_str, &action))
    {
        return CMD_ERR_NO_MATCH;
    }
	ret = api_acr_account_default_act_set(action);
	if (ret)
    {
        vty_out(vty, "account set default action fail:(%s)%s", berr_msg(ret), VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}



DEFUN(account_default_act_set,
      account_default_act_set_cmd,
      "account default ACT",
      ACCOUNT_STR
      DEFAULT_STR
      ACTION_STR)
{
    return cmd_acr_account_default_act_set(vty, argv[0]);
}







void
acr_write_config_vty(void *data, void *param)
{
    acr_t *entry = NULL;

    struct vty *vty = NULL;

    char action_str[NAGA_ACTION_STR_SZ];

    if ((NULL == param) || (NULL == data))
    {
        return;
    }

    entry = (acr_t *) data;
    vty   = (struct vty *) param;

    naga_action_string(&entry->acl.actions, action_str);
    vty_out(vty, "account %s %s %s", entry->account, action_str, VTY_NEWLINE);
	
}



void acr_cmd_config_write(struct vty *vty)
{
	 acr_iter(acr_write_config_vty, vty);
}






/*
 * acr module cmdline register and init
 *
 * */
void cmdline_acr_init(void)
{
	install_element(CMD_NODE, &account_cmd);
	install_element(CMD_NODE, &remove_account_cmd);
	install_element(CMD_NODE, &remove_account_all_cmd);
	install_element(CMD_NODE, &load_account_cmd);
	install_element(CMD_NODE, &clear_account_stat_cmd);
	install_element(CMD_NODE, &clear_account_stat_all_cmd);
	install_element(CMD_NODE, &show_account_cmd);
	install_element(CMD_NODE, &show_account_all_cmd);

    return ;
}

