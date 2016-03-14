#include <string.h>

#include "zebra.h"
#include "version.h"
#include "getopt.h"
#include "command.h"
#include "filter.h"
#include "prefix.h"
#include "privs.h"

#include "naga_types.h"
#include "itf.h"
#include "itf_cmd.h"
#include "itf_stat.h"

DEFUN(itf_rxtx, 
      itf_rxtx_cmd,
      "interface bussiness (add|remove|test) IFNAME", "interface setting\nenable or disable\nrx or tx\n")
{
    if( !strcmp (argv[0], "add"))
    {
        char * ifname = strdup(argv[1]);
        berr rv=  libpcap_rx_loop_setup(ifname);
        if(rv == E_SUCCESS)
            vty_out(vty, "Success to add %s rx%s", ifname, VTY_NEWLINE);
        else
            vty_out(vty, "Failed to add %s rx%s", ifname, VTY_NEWLINE);
        free(ifname);
        return 0;
    }
	else if(!strcmp (argv[0], "remove"))
	{
        char * ifname = strdup(argv[1]);
        berr rv=  libpcap_rx_loop_unset(ifname);
        if(rv == E_SUCCESS)
            vty_out(vty, "Success to remove %s rx%s", ifname, VTY_NEWLINE);
        else
            vty_out(vty, "Failed to remove %s rx%s", ifname, VTY_NEWLINE);
        free(ifname);
        return 0;		
	}
    else if(!strcmp (argv[0], "test"))
    {
        hytag_t hytag;
        memset(&hytag, 0x0, sizeof(hytag));

        char buffer[] = {
            0x00,0x25 ,0x9e,0xf3 ,0x4d,0x3d ,0x08,0x00 ,0x27,0xbd ,0x13,0x60 ,0x08,0x00 ,0x45,0x00, 
        	0x01,0xca ,0xfa,0xb6 ,0x40,0x00 ,0x40,0x06 ,0x89,0x50 ,0xc0,0xa8 ,0x14,0xed ,0x3d,0x87, 
        	0xa2,0x0a ,0x13,0x62 ,0x00,0x50 ,0xd5,0x06 ,0x96,0xc3 ,0x89,0xef ,0x66,0xfd ,0x50,0x18, 
        	0xfa,0xf0 ,0x21,0xa1 ,0x00,0x00 ,0x47,0x45 ,0x54,0x20 ,0x2f,0x20 ,0x48,0x54 ,0x54,0x50, 
        	0x2f,0x31 ,0x2e,0x31 ,0x0d,0x0a ,0x41,0x63 ,0x63,0x65 ,0x70,0x74 ,0x3a,0x20 ,0x61,0x70, 
        	0x70,0x6c ,0x69,0x63 ,0x61,0x74 ,0x69,0x6f ,0x6e,0x2f ,0x78,0x2d ,0x73,0x68 ,0x6f,0x63, 
        	0x6b,0x77 ,0x61,0x76 ,0x65,0x2d ,0x66,0x6c ,0x61,0x73 ,0x68,0x2c ,0x20,0x69 ,0x6d,0x61, 
        	0x67,0x65 ,0x2f,0x67 ,0x69,0x66 ,0x2c,0x20 ,0x69,0x6d ,0x61,0x67 ,0x65,0x2f ,0x78,0x2d, 
        	0x78,0x62 ,0x69,0x74 ,0x6d,0x61 ,0x70,0x2c ,0x20,0x69 ,0x6d,0x61 ,0x67,0x65 ,0x2f,0x6a, 
        	0x70,0x65 ,0x67,0x2c ,0x20,0x69 ,0x6d,0x61 ,0x67,0x65 ,0x2f,0x70 ,0x6a,0x70 ,0x65,0x67, 
        	0x2c,0x20 ,0x61,0x70 ,0x70,0x6c ,0x69,0x63 ,0x61,0x74 ,0x69,0x6f ,0x6e,0x2f ,0x76,0x6e, 
        	0x64,0x2e ,0x6d,0x73 ,0x2d,0x65 ,0x78,0x63 ,0x65,0x6c ,0x2c,0x20 ,0x61,0x70 ,0x70,0x6c, 
        	0x69,0x63 ,0x61,0x74 ,0x69,0x6f ,0x6e,0x2f ,0x76,0x6e ,0x64,0x2e ,0x6d,0x73 ,0x2d,0x70, 
        	0x6f,0x77 ,0x65,0x72 ,0x70,0x6f ,0x69,0x6e ,0x74,0x2c ,0x20,0x61 ,0x70,0x70 ,0x6c,0x69, 
        	0x63,0x61 ,0x74,0x69 ,0x6f,0x6e ,0x2f,0x6d ,0x73,0x77 ,0x6f,0x72 ,0x64,0x2c ,0x20,0x2a, 
        	0x2f,0x2a ,0x0d,0x0a ,0x41,0x63 ,0x63,0x65 ,0x70,0x74 ,0x2d,0x4c ,0x61,0x6e ,0x67,0x75, 
        	0x61,0x67 ,0x65,0x3a ,0x20,0x7a ,0x68,0x2d ,0x63,0x6e ,0x0d,0x0a ,0x41,0x63 ,0x63,0x65, 
        	0x70,0x74 ,0x2d,0x45 ,0x6e,0x63 ,0x6f,0x64 ,0x69,0x6e ,0x67,0x3a ,0x20,0x67 ,0x7a,0x69, 
        	0x70,0x2c ,0x20,0x64 ,0x65,0x66 ,0x6c,0x61 ,0x74,0x65 ,0x0d,0x0a ,0x55,0x73 ,0x65,0x72, 
        	0x2d,0x41 ,0x67,0x65 ,0x6e,0x74 ,0x3a,0x20 ,0x4d,0x6f ,0x7a,0x69 ,0x6c,0x6c ,0x61,0x2f, 
        	0x34,0x2e ,0x30,0x20 ,0x28,0x63 ,0x6f,0x6d ,0x70,0x61 ,0x74,0x69 ,0x62,0x6c ,0x65,0x3b, 
        	0x20,0x4d ,0x53,0x49 ,0x45,0x20 ,0x36,0x2e ,0x30,0x3b ,0x20,0x57 ,0x69,0x6e ,0x64,0x6f, 
        	0x77,0x73 ,0x20,0x4e ,0x54,0x20 ,0x35,0x2e ,0x31,0x3b ,0x20,0x53 ,0x56,0x31 ,0x29,0x0d, 
        	0x0a,0x43 ,0x6f,0x6f ,0x6b,0x69 ,0x65,0x3a ,0x20,0x42 ,0x41,0x49 ,0x44,0x55 ,0x49,0x44, 
        	0x3d,0x36 ,0x30,0x35 ,0x33,0x38 ,0x46,0x32 ,0x43,0x38 ,0x44,0x30 ,0x41,0x30 ,0x37,0x39,
        	0x38,0x45 ,0x42,0x35 ,0x35,0x45 ,0x38,0x43 ,0x41,0x33 ,0x37,0x45 ,0x33,0x31 ,0x34,0x33, 
        	0x31,0x3a ,0x46,0x47 ,0x3d,0x31 ,0x0d,0x0a ,0x43,0x6f ,0x6e,0x6e ,0x65,0x63 ,0x74,0x69, 
        	0x6f,0x6e ,0x3a,0x20 ,0x4b,0x65 ,0x65,0x70 ,0x2d,0x41 ,0x6c,0x69 ,0x76,0x65 ,0x0d,0x0a, 
        	0x48,0x6f ,0x73,0x74 ,0x3a,0x20 ,0x77,0x77 ,0x77,0x2e ,0x68,0x61 ,0x6f,0x31 ,0x32,0x33, 
        	0x2e,0x63 ,0x6f,0x6d ,0x0d,0x0a ,0x0d,0x0a
        };


        hytag.pbuf.ptr = (void *)buffer;
        hytag.pbuf.len =  sizeof(buffer);
        hytag.pbuf.ptr_offset = 0;
        hytag.m = NULL;



        naga_data_process_module(&hytag);
        vty_out(vty, "Success to test %s", VTY_NEWLINE);
        return;
              
    }
    return 0;
}



static int itf_cmd_show_status(struct vty *vty)
{
    int rv;
    itf_stat stat;
    stat.direction = ITF_TX;
    rv = itf_stat_get( &stat);
    if (rv)
    {
        vty_out(vty, "get tx status fail rv(%d)%s", rv, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "TX :%s%s", stat.enable == ITF_ENABLE ?"Enable":"Disable", VTY_NEWLINE);
    }

    stat.direction = ITF_RX;
    rv = itf_stat_get( &stat);
    if (rv)
    {
        vty_out(vty, "get rx status fail rv(%d)%s", rv, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "RX :%s%s", stat.enable == ITF_ENABLE ?"Enable":"Disable", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(itf_show_stat, 
      itf_show_stat_cmd,
      "show itf status",
      SHOW_STR
      INTERFACE_STR
      "Status information\n" 
      )
{
    return itf_cmd_show_status(vty);
}


static int itf_cmd_set(struct vty *vty, const char *dir_str, const char *en_str)
{
    int ret = 0;
    itf_stat stat;
    if ('t' == dir_str[0])
    {
        stat.direction = ITF_TX;
    }
    else
    if ('r' == dir_str[0])
    {
        stat.direction = ITF_RX;
    }
    else
    {
        vty_out(vty, "parameter err %s %s", dir_str, VTY_NEWLINE);
        return CMD_WARNING;
    }

    if ('e' == en_str[0])
    {
        stat.enable = ITF_ENABLE;
    }
    else
    if ('d' == en_str[0])
    {
        stat.enable = ITF_DISABLE;
    }
    else
    {
        vty_out(vty, "parameter err %s %s", en_str, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = itf_stat_set(&stat);
    if (ret)
    {
        vty_out(vty, "itf stat set fail ret(%d)%s", ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}
DEFUN(itf_set,
      itf_set_cmd,
      "itf (rx|tx) (enable|disable)",
      INTERFACE_STR
      "Rx or Tx direction\n"
      "En or Disable operation\n" 
      )
{
    return itf_cmd_set(vty, argv[0], argv[1]);
}

static int interface_cmd_set(struct vty *vty, const char *if_str, const char *en_str)
{
    int ret = 0;
    port_stat stat;

    stat.port_id = atoi(if_str);
    if ('e' == en_str[0])
    {
        stat.enable = ITF_ENABLE;
    }
    else
    if ('d' == en_str[0])
    {
        stat.enable = ITF_DISABLE;
    }
    else
    {
        vty_out(vty, "parameter err %s %s", en_str, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = interface_stat_set(&stat);
    if (ret)
    {
        vty_out(vty, "interface stat set fail ret(%d)%s", ret, VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(interface_set,
      interface_set_cmd,
      "interface <0-1> (enable|disable)",
      INTERFACE_STR
      "Interface Number\n"
      "En or Disable operation\n" 
      )
{
    return interface_cmd_set(vty, argv[0], argv[1]);
}

static int interface_cmd_show_status(struct vty *vty)
{
    int rv;
    int i;
    port_stat stat;
    struct rte_eth_stats stats;
    uint8_t port_id; 
    for ( i = 0; i < INTERFACE_NUM_MAX; i++ )
    {
        stat.port_id = i;
        rv = interface_stat_get( &stat);
        if (rv)
        {
            vty_out(vty, "get tx status fail rv(%d)%s", rv, VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "Port(%d) :%s%s", i, stat.enable == ITF_ENABLE ?"Enable":"Disable", VTY_NEWLINE);
        }

        port_id = (uint8_t)i;
        rte_eth_stats_get(port_id, &stats);
   
    	vty_out(vty, "Rx Packet             :%-20ld %s", stats.ipackets, VTY_NEWLINE);  
        vty_out(vty, "Rx bytes              :%-20ld %s", stats.ibytes, VTY_NEWLINE); 
 
  

        vty_out(vty, "RX Drop               :%-20ld %s", stats.imissed, VTY_NEWLINE);   
    	vty_out(vty, "Rx CRC                :%-20ld %s", stats.ibadcrc, VTY_NEWLINE);   
    	vty_out(vty, "Rx Badlen             :%-20ld %s", stats.ibadlen, VTY_NEWLINE);   
    	vty_out(vty, "RX ERR(Total)         :%-20ld %s", stats.ierrors, VTY_NEWLINE);   
     	vty_out(vty, "Rx Mcasts             :%-20ld %s", stats.imcasts, VTY_NEWLINE); 
  
    	vty_out(vty, "Rx No mbuf            :%-20ld %s", stats.rx_nombuf, VTY_NEWLINE); 
        
    	vty_out(vty, "Rx Filter match       :%-20ld %s", stats.fdirmatch, VTY_NEWLINE); 
    	vty_out(vty, "Rx Filter nomatch     :%-20ld %s", stats.fdirmiss, VTY_NEWLINE);  
        vty_out(vty, "Rx Pause_xoff         :%-20ld %s", stats.rx_pause_xoff, VTY_NEWLINE);         
    	vty_out(vty, "Rx Pause_xon          :%-20ld %s", stats.rx_pause_xon, VTY_NEWLINE);  
             

        vty_out(vty, "Tx Packet             :%-20ld %s", stats.opackets, VTY_NEWLINE);  
    	vty_out(vty, "Tx Byte               :%-20ld %s", stats.obytes, VTY_NEWLINE);     
        vty_out(vty, "TX ERR(Total)         :%-20ld %s", stats.oerrors, VTY_NEWLINE); 
    	vty_out(vty, "Tx Pause_xon          :%-20ld %s", stats.tx_pause_xon, VTY_NEWLINE);  
    	vty_out(vty, "Tx Pause_xoff         :%-20ld %s", stats.tx_pause_xoff, VTY_NEWLINE); 


    }


    return CMD_SUCCESS;
}

DEFUN(interface_show_stat, 
      interface_show_stat_cmd,
      "show interface status",
      SHOW_STR
      INTERFACE_STR
      "Status information\n" 
      )
{
    return interface_cmd_show_status(vty);
}

void itf_cmd_config_write(struct vty *vty)
{
    int rv;
    itf_stat stat;
    stat.direction = ITF_TX;
    rv = itf_stat_get( &stat);
    if ( E_SUCCESS == rv)
    {
        vty_out(vty, "itf tx %s%s", stat.enable == ITF_ENABLE ?"enable":"disable", VTY_NEWLINE);
    }

    stat.direction = ITF_RX;
    rv = itf_stat_get( &stat);
    if ( E_SUCCESS == rv)
    {
        vty_out(vty, "itf rx %s%s", stat.enable == ITF_ENABLE ?"enable":"disable", VTY_NEWLINE);
    }

    {
        int i;
        port_stat stat;
        for ( i = 0; i < INTERFACE_NUM_MAX; i++ )
        {
            stat.port_id = i;
            rv = interface_stat_get( &stat);
            if ( E_SUCCESS == rv)
            {
                vty_out(vty, "interface %d %s%s", i, stat.enable == ITF_ENABLE ?"enable":"disable", VTY_NEWLINE);
            }
        }
    }

extern struct list_head	handle_head;
	struct list_head *pos = NULL, *next = NULL;
	libpcap_handler_t *handle = NULL;
	list_for_each_safe(pos, next,&handle_head)
	{
		handle = (libpcap_handler_t *)list_entry(pos, libpcap_handler_t, node);
		vty_out(vty, "interface bussiness add %s%s", handle->ifname, VTY_NEWLINE);			
	}


}






/*
 * vsr module cmdline register and init 
 *
 * */
void cmdline_itf_init(void)
{
    install_element(CMD_NODE, &itf_rxtx_cmd);
    install_element(CMD_NODE, &itf_show_stat_cmd);
    install_element(CMD_NODE, &itf_set_cmd);
    install_element(CMD_NODE, &interface_set_cmd);
    install_element(CMD_NODE, &interface_show_stat_cmd);

    return ;
}


