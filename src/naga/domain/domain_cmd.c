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

#include "domain.h"

#include "version.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"
#include "domain_cmd.h"
#define DOMAIN_STR					"Domain rule\n"
#define DOMAIN_NAME_STR             "Name of domain\n" 
#define DOMAIN_ALL_STR             "All the domain\n"


#define DMR_DEBUG
#ifdef  DMR_DEBUG
#define dmr_debug(fmt,args...)  vty_out(vty, "func=%s line=%d:" fmt "%s", __FUNCTION__, __LINE__, ##args, VTY_NEWLINE)
#else
#define dmr_debug(fmt,args...)
#endif  /* DEBUG */

static berr convertChar(const char *keyword, uint8_t *data, uint32_t *cnt)
{
    uint8_t temp = 0;
    
    switch (keyword[1]) 
    {
        case 'x': /* \xff */
            temp = 0;
            if (keyword[2] > 0x2f && keyword[2] < 0x3a) 
            {
                /* ascii 0-9 */
                temp += 0x10 * (keyword[2] - 0x30);
            } 
            else if ((keyword[2] | 0x20) > 0x60 && (keyword[2] | 0x20) < 0x67)
            {
                /* ascii a-f, A-F */
                temp += 0x10 * ((keyword[2] | 0x20) - 0x61 + 10);
            } 
            else 
            {
                BRET(E_FAIL);
            }
                
            if (keyword[3] > 0x2f && keyword[3] < 0x3a) 
            {
                /* ascii 0-9 */
                temp += (keyword[3] - 0x30);
            } 
            else if ((keyword[3] | 0x20) > 0x60 && (keyword[3] | 0x20) < 0x67) 
            {
                /* ascii a-f, A-F */
                temp += ((keyword[3] | 0x20) - 0x61 + 10);
            } 
            else 
            {
                BRET(E_FAIL);
            }
            
            *data = temp;
            *cnt = 3;
            break;
        case 'm': /* \xddmm , one byte data one byte mask */
            temp = 0;
            if (keyword[2] > 0x2f && keyword[2] < 0x3a) 
            {
                /* ascii 0-9 */
                temp += 0x10 * (keyword[2] - 0x30);
            } 
            else if ((keyword[2] | 0x20) > 0x60 && (keyword[2] | 0x20) < 0x67) 
            {
                /* ascii a-f, A-F */
                temp += 0x10 * ((keyword[2] | 0x20) - 0x61 + 10);
            } 
            else 
            {
                BRET(E_FAIL);
            }
            
            if (keyword[3] > 0x2f && keyword[3] < 0x3a) 
            {
                /* ascii 0-9 */
                temp += (keyword[3] - 0x30);
            } 
            else if ((keyword[3] | 0x20) > 0x60 && (keyword[3] | 0x20) < 0x67) 
            {
                /* ascii a-f, A-F */
                temp += ((keyword[3] | 0x20) - 0x61 + 10);
            } 
            else 
            {
                BRET(E_FAIL);
            }
            
            *data = temp;
            temp = 0;
            
            if (keyword[4] > 0x2f && keyword[4] < 0x3a) 
            {
                /* ascii 0-9 */
                temp += 0x10 * (keyword[4] - 0x30);
                //printf("%s.%d: keyword[4]=%c\n", __func__, __LINE__, keyword[4]);
            } 
            else if ((keyword[4] | 0x20) > 0x60 && (keyword[4] | 0x20) < 0x67) 
            {
                /* ascii a-f, A-F */
                temp += 0x10 * ((keyword[4] | 0x20) - 0x61 + 10);
                //printf("%s.%d: keyword[4]=%c\n", __func__, __LINE__, keyword[4]);
            } 
            else 
            {
                BRET(E_FAIL);
            }
            
            if (keyword[5] > 0x2f && keyword[5] < 0x3a) 
            {
                /* ascii 0-9 */
                temp += (keyword[5] - 0x30);
                //printf("%s.%d: keyword[5]=%c\n", __func__, __LINE__, keyword[5]);
            } 
            else if ((keyword[5] | 0x20) > 0x60 && (keyword[5] | 0x20) < 0x67) 
            {
                /* ascii a-f, A-F */
                temp += ((keyword[5] | 0x20) - 0x61 + 10);
                //printf("%s.%d: keyword[5]=%c\n", __func__, __LINE__, keyword[5]);
            } 
            else 
            {
                BRET(E_FAIL);
            }
            
            *cnt = 5;
            break;
        case 'i': /* ignore case , must a-z|A-Z */
            if ((keyword[2] | 0x20) > 0x7a || (keyword[2] | 0x20) < 0x61) 
            {
                BRET(E_FAIL);
            }
            
            *data = keyword[2];
            *cnt = 2;
            break;
        case '*':
            *data = '*';
            *cnt = 1;
            break;
        case 's':
            *data = ' ';
            *cnt = 1;
            break;
        case '\\':
            *data = '\\';
            *cnt = 1;
            break;
        case 'r':
            *data = '\r';
            *cnt = 1;
            break;
        case 'n':
            *data = '\n';
            *cnt = 1;
            break;
        case 't':
            *data = '\t';
            *cnt = 1;
            break;
        case '0':
            *data = 0x0;
            *cnt = 1;
            break;
        case 'b':
            *data = 0x8;
            *cnt = 1;
            break;
        case 'a':
            *data = 0x7;
            *cnt = 1;
            break;
        case 'q':
            *data = '?';
            *cnt = 1;
            break;
        case 'S':
            *data = '\'';
            *cnt = 1;
            break;

        default:
            BRET(E_FAIL);
    }
    
    return E_SUCCESS;
}


static berr host_str2bit(const char *str, uint8_t *data, uint32_t *len)
{
	uint32_t i = 0, j = 0;
    uint32_t cnt = 0;

    if ((str == NULL) || (data == NULL) || (len == NULL))
    {
        BRET(E_FAIL);
    }

    for (i = 0; i < MAX_HOST_LEN; i++, j++) 
    {
	    switch (str[i]) 
	    {
	        case '\0':
	        case '\n':
	        case '\r':
	        case '\t':
	            return E_SUCCESS;
	        case '\\':
	            if (convertChar(&str[i], &data[j], &cnt) != E_SUCCESS)
	            {              
	                BRET(E_FAIL);
	            }
	            
	            i += cnt; /* skip character */
	            break;
	        case '*':
	            data[j] = 0x0;
	            break;
	        /*case ' ':  //unsupport input space(' ') keyword
	            return ERROR;*/
	        case ' ':   //support input space(' ') keyword
	            data[j] = ' ';
	            break;
	        case '?':
	            data[j] = '?';
	            break;
	        default:
	            data[j] = (uint8_t)str[i];
	            break;
	    }
    	
        *len = j;
    }

    /*Check Input strlen out of range*/
    if (str[i] != '\0')
    {
        return  E_FAIL;
    }
    return E_SUCCESS;

}

static dmr_t *dmr_entry_new(void)
{
	dmr_t *entry = NULL;

	entry = malloc(sizeof(dmr_t));
	if (NULL != entry)
	{
		memset(entry, 0, sizeof(dmr_t));
	}

	return entry;
}

void
domain_dump_vty(void *data, void *param)
{
    dmr_t *entry = NULL;
    struct vty *vty = NULL;
    dmr_param_t *dmr_pram ;
    
    char action_str[NAGA_ACTION_STR_SZ] = {0};

    if ((NULL == param) || (NULL == data))
    {
        return;
    }

    dmr_pram= (dmr_param_t *)param;

    entry = (dmr_t *) data;
    vty   = (struct vty *) dmr_pram->vty;

    naga_action_string(&entry->acl.actions, action_str);


    {
            vty_out(vty, "%-32s %-32s %-16ld %-16ld %-16ld %-8d %s", entry->host, action_str, 
                (uint64_t) entry->acl.cnt.cnt,(uint64_t)entry->acl.vcnt.cnt,
                    (uint64_t)entry->acl.pushed_cnt.cnt, entry->interval, VTY_NEWLINE);

            dmr_pram->cnt_total +=  (uint64_t) entry->acl.cnt.cnt;
            dmr_pram->non_drop  +=  (uint64_t)entry->acl.vcnt.cnt;
            dmr_pram->pushed    += (uint64_t)entry->acl.pushed_cnt.cnt;  
    
    }
}

static int cmd_dmr_show(struct vty *vty, const char *host)
{
	int ret = 0;
    dmr_param_t pram;
	dmr_t *entry = NULL;
   
    
	if (NULL == host)
	{
        return CMD_ERR_NOTHING_TODO;
	}

    pram.vty = vty;
    pram.flag = FLAG_SHOW_ALL;
    entry = api_dmr_get((char *)host);
    if (NULL == entry)
    {
        vty_out(vty, "dmr host<%s> empty%s", host, VTY_NEWLINE);
        return CMD_WARNING;
    }
	vty_out(vty, "%-32s %-32s %-16s %-16s %-16s %-8s %s","host","action", "get_cnt","adp_push_cnt", "push_success","interval",VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------%s", VTY_NEWLINE);

    dmr_dump_vty((void *)entry, (void *)&pram);
   
    return CMD_SUCCESS;
}



static int cmd_domain_show_all(struct vty *vty)
{

    dmr_param_t pram;
    memset(&pram, 0, sizeof(dmr_param_t));
	vty_out(vty, "%-32s %-32s %-16s %-16s %s","host", "cnt","valid", "pushed",VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------------------------------%s", VTY_NEWLINE);

    pram.vty = vty;
    domain_iter(domain_dump_vty, (void*)&pram);
    vty_out(vty, "================================================================================%s", VTY_NEWLINE);
    vty_out(vty, "%-32s %-32s %-16ld %-16ld %-16ld%s", "Total", "NONE", 
                pram.cnt_total, pram.non_drop, pram.pushed, VTY_NEWLINE);
    return CMD_SUCCESS;
}




DEFUN(show_domain_all,
      show_domain_all_cnt_cmd,
      "show counter domain all",
      SHOW_STR
      DOMAIN_STR
      DOMAIN_ALL_STR)
{
    return cmd_domain_show_all(vty);
}




void cmdline_domain_init(void)
{
	install_element(CMD_NODE, &show_domain_all_cnt_cmd);
    return ;
}
