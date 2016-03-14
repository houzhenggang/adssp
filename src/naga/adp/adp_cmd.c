#include <string.h>

#include "zebra.h"
#include "version.h"
#include "getopt.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"
#include "packet.h"
#include "naga_types.h"
#include "nag_adp.h"
#include "adp_cmd.h"
#include "bts_cnt.h"

DEFUN(adp_interval, 
      adp_interval_cmd,
      "adp interval <1-100000>", "Adp module\ninterval\nper http-get packet <1-100000> url will\n")
{
    int interval = strtoul(argv[0], NULL, 0);
    return adp_set_interval(interval);
}

DEFUN(adp_interval_get, 
      adp_interval_get_cmd,
      "show adp interval", "Adp module\ninterval\nget interval http-get packet <1-100000> url will\n")
{
    int interval;
    uint64_t adp_can_push_count;
    uint64_t adp_push_tx_success;
	uint64_t adp_push_ack_success;
    time_t *boottime = NULL;

	adp_get_interval(&interval);

	adp_can_push_count = CNT_GET(ADP_ALL_CAN_PUSH); 
	adp_push_tx_success = CNT_GET(ADP_PUSH_TX_SUCCESS);
	adp_push_ack_success = CNT_GET(ADP_PUSH_ACK_SUCCESS);

    boottime = adp_get_start_time();

    if(boottime !=  NULL)
    {
        vty_out(vty, "Boot Time             : %s%s", ctime(boottime), VTY_NEWLINE);
    }
    vty_out(vty, "Interval              : %d%s", interval, VTY_NEWLINE);
	
    vty_out(vty, "All can push          : %ld%s",  adp_can_push_count, VTY_NEWLINE);    
    vty_out(vty, "Pushed(tx success)    : %ld%s",  adp_push_tx_success, VTY_NEWLINE);
	
	if(adp_push_tx_success==0)
	{
		vty_out(vty, "Pushed(ack success)   : %ld (0%%)%s", adp_push_ack_success, VTY_NEWLINE);
		return 0;
	}
	else
	{
		float fpushed = adp_push_tx_success;
		float fsuccess = adp_push_ack_success;
	
		float aver =  (fsuccess/fpushed)*100;
    
		vty_out(vty, "Pushed(ack success)   : %ld (%f%%)%s", adp_push_ack_success, aver, VTY_NEWLINE);
	}
    return 0;
}


DEFUN(adp_interval_clean, 
      adp_interval_clean_cmd,
      "clear adp interval", "Adp module\ninterval\nget interval http-get packet <1-100000> url will\n")
{

	
    adp_clear_interval();

    return 0;
}




DEFUN(adp_switch_set_on, 
      adp_switch_set_on_cmd,
      "adp switch on", "\n")
{
    adp_switch_set(1);
    return 0;
}

DEFUN(adp_switch_set_off, 
      adp_switch_set_off_cmd,
      "adp switch off", "\n")
{

    adp_switch_set(0);
    return 0;
}



DEFUN(adp_switch_template, 
      adp_switch_template_cmd,
      "adp switch template (pc|mobile) (on|off)", "\n")
{

      int type;
      
     if(argc < 2 )
     {
         vty_out(vty, "PARAM Err argc < 2 %s", VTY_NEWLINE);
         return 0; 
     }  
      if(!strcmp(argv[0], "pc") )
     {
        type    = AD_TEMPLATE_PC;
     }
     else  if(!strcmp(argv[0], "mobile") )
     {
          type    = AD_TEMPLATE_MOBILE;
     }
     else
     {
         vty_out(vty, "PARAM Err (pc|mobile) %s", VTY_NEWLINE);
         return 0;      
     }

      if(!strcmp(argv[1], "on") )
     {  
         adp_switch_template_set(type, 1);        
     }
     else  if(!strcmp(argv[1], "off") )
     {
           adp_switch_template_set(type, 0);
     }
     else
     {
         vty_out(vty, "PARAM Err (on|off)%s", VTY_NEWLINE);
         return 0;        
     }


     
    return 0;
}



DEFUN(adp_mac_custom_set, 
        adp_mac_custom_set_cmd,
      "adp (dmac|smac) custom", "\n")
{

	int dst_src;
	uint8_t mac[6];
	
	if(argv[0] == NULL)
	{
		vty_out(vty, "PARAM NULL %s", VTY_NEWLINE);
		return E_SUCCESS;
	}
	else
	{
		if(!strcmp(argv[0], "dmac"))
			dst_src = 0;
		else if(!strcmp(argv[0], "smac"))	
			dst_src = 1;
		else
			return E_PARAM;
		
		ads_mac_set(dst_src, 0, mac);	
	}

	
    return 0;
}

DEFUN(adp_mac_special_set, 
        adp_mac_special_set_cmd,
      "adp (dmac|smac) set MAC", "\n")
{

	int dst_src;
	uint8_t mac[6] = {0,0,0,0,0,0};
	char * mac_str = NULL;
	char *arr = NULL; 
	int i = 0;

	
	if(argv[0] == NULL)
	{
		vty_out(vty, "PARAM NULL %s", VTY_NEWLINE);
		return E_PARAM;
	}
	else
	{
		if(!strcmp(argv[0], "dmac"))
			dst_src = 0;
		else if(!strcmp(argv[0], "smac"))	
			dst_src = 1;
		else
		{
			vty_out(vty, "PARAM smac or dmac %s", VTY_NEWLINE);		
			return E_PARAM;
		}
		if(argv[1]) 
		{	
			mac_str = strdup(argv[1]);
		}
		else
		{
			vty_out(vty, "PARAM smac or dmac %s", VTY_NEWLINE);		
			return E_PARAM;
		}	
		
		while  (NULL != (arr = strsep(&mac_str, ":")))
		{
			if(i == 6 || strlen(arr) != 2)
			{
				vty_out(vty, "PARAM mac format err AA:BB:CC:DD:EE:FF%s", VTY_NEWLINE);				
				return E_PARAM;
			}
				
			mac[i++] = strtoul(arr, NULL, 16);	
		}
		
		ads_mac_set(dst_src, 1, mac);	
		free(mac_str);
	}
	
    return 0;

}



void adp_cmd_config_write(struct vty *vty)
{
    int ret = 0;
	uint8_t mac[6];
    int custom;
	

    ret =  ads_mac_get(0 , &custom, (uint8_t *)mac);

	if(ret != E_SUCCESS)
	{
		//vty_out(vty, "Failed to get dmac %s", VTY_NEWLINE);  
		return ;
	}
	
	if(custom == 0)
    {
        vty_out(vty, "adp dmac custom%s", VTY_NEWLINE);  
    }
	else
	{
		vty_out(vty, "adp dmac set %02x:%02x:%02x:%02x:%02x:%02x%s",
						mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],
					VTY_NEWLINE); 
	}

	ret = ads_mac_get(1 , &custom, (uint8_t *)mac);

	if(ret != E_SUCCESS)
	{
		//vty_out(vty, "Failed to get smac %s", VTY_NEWLINE);  
		return ;
	}
	


	
	if(custom == 0)

	{
		vty_out(vty, "adp smac custom%s", VTY_NEWLINE);
		
	}
	else 
	{
		vty_out(vty, "adp smac set %02x:%02x:%02x:%02x:%02x:%02x%s",
						mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],
					VTY_NEWLINE); 	
	}

	int interval = 0;
    int on ;


	adp_get_interval(&interval);  

	vty_out(vty, "adp interval %d%s",
						interval,
					VTY_NEWLINE); 


    int type = AD_TEMPLATE_PC;
    adp_switch_template_get(type, &on);
	vty_out(vty, "adp switch template pc %s%s",
						on? "on":"off",
					VTY_NEWLINE); 	
    
    type = AD_TEMPLATE_MOBILE;
    adp_switch_template_get(type, &on);
	vty_out(vty, "adp switch template mobile %s%s",
						on? "on":"off",
					VTY_NEWLINE); 	
	

	adp_switch_get(&on);
	vty_out(vty, "adp switch %s%s",
						on? "on":"off",
					VTY_NEWLINE); 		

		
	
}








/*
 * vsr module cmdline register and init 
 *
 * */
void cmdline_adp_init(void)
{
    install_element(CMD_NODE, &adp_interval_cmd);
    install_element(CMD_NODE, &adp_interval_get_cmd);
    install_element(CMD_NODE, &adp_switch_set_on_cmd); 
    install_element(CMD_NODE, &adp_switch_set_off_cmd); 
	
    install_element(CMD_NODE, &adp_mac_custom_set_cmd);
    install_element(CMD_NODE, &adp_mac_special_set_cmd);
    install_element(CMD_NODE, &adp_switch_template_cmd);

	install_element(CMD_NODE, &adp_interval_clean_cmd);
    
    return ;
}

