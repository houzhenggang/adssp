#include "pid.h"

#define MAX_METHOD_LEN 64
#define STRING_HTTP_GET  "GET"
#define STRING_HTTP_GET_LEN  3
#define STRING_HTTP_VERSION_MAX  32

#define STRING_HTTP_HOST "Host"
#define STRING_HTTP_HOST_LEN 4


#define STRING_HTTP_REFERENCE "Referer"
#define STRING_HTTP_REFERENCE_LEN  7
#define STRING_HTTP_AGENT     "User-Agent"
#define STRING_HTTP_AGENT_LEN 10

static inline int pid_http_request_method(uint8_t *p ,  uint8_t *method, uint16_t *len)
{
	int i = 0;
	while((0x20 != *p) && ('\0' != *p))    /*GET*/
	{
		if (i + 1 > MAX_METHOD_LEN)
		{
			pid_incr_count(HTTP_METHOD_EXCEED);
			return FALSE;
		}
		else
		{
			method[i++] = *(p++);
		}
	}

	method[i] = '\0';
	*len = i;

	return TRUE;
}



static inline berr pid_http_request_url(uint8_t *p ,  uint8_t *url, uint16_t *len)
{
	int i = 0;
	while((0x20 != *p) && ('\0' != *p))    /*GET*/
	{
		if (i + 1 > URL_MAX_LEN)
		{
			//pid_incr_count(HTTP_URL_EXCEED);
			//BRET(E_FAIL);
			return E_FAIL;
		}
		else
		{
			url[i++] = *(p++);
		}
	}

	url[i] = '\0';
	*len = i;

	return E_SUCCESS;
}


static inline berr pid_http_request_version_skip(uint8_t *p, uint16_t *len)
{
	int i = 0;
	while(('\r' != *p) && ('\0' != *p)) 
	{
		if (i + 1 > STRING_HTTP_VERSION_MAX)
		{
			//pid_incr_count(HTTP_URL_EXCEED);
			//BRET(E_FAIL);
			return E_FAIL;
		}
		else
		{
			p++;
		}

		i ++;
	}

	*len = i + 1;

	return E_SUCCESS;
}


#if 0
static inline berr pid_http_request_host(uint8_t *p ,  uint8_t *host, uint16_t *len)
{
	int i = 0;
	while(('\r' != *p) && ('\0' != *p))    /*GET*/
	{
		if (i + 1 > MAX_HOST_LEN)
		{
			//pid_incr_count(HTTP_URL_EXCEED);
			BRET(E_FAIL);
		}
		else
		{
			host[i++] = *(p++);
		}
	}

	host[i] = '\0';
	*len = i;

	return E_SUCCESS;
}

#endif

berr pid_http_down(struct pbuf *p ,  hytag_t * hytag )
{
    if( NULL == p || NULL == hytag)
        BRET(E_PARAM);
    return E_SUCCESS;
}
#if 0

berr pid_http_up(struct pbuf *p ,  hytag_t * hytag )
{
	uint16_t method_len = 0, vsr_len = 0;
	uint16_t len = 0;
	uint8_t *http_p = NULL;
	uint8_t method_http[MAX_METHOD_LEN] = {0};
	char *line = NULL, *begin = NULL, *context_p = NULL;
	
	PBUF_CUR_FORMAT(uint8_t *, http_p, p);

    hytag->app_type = APP_TYPE_HTTP_OTHER; 

	if(!pid_http_request_method(http_p, method_http, &method_len))
	{
        /*Tcp syn without method*/
		return E_SUCCESS;
	}
	if ((method_len != STRING_HTTP_GET_LEN ) ||
		(memcmp(STRING_HTTP_GET, method_http, STRING_HTTP_GET_LEN)))
	{
		pid_incr_count(APP_HTTP_OTHER);
		return E_SUCCESS;
	}
	else
	{
		pid_incr_count(APP_HTTP_GET);
	}

	len = method_len + 1;
	UPDATE_PBUF_OFFSET(p, len);
	PBUF_CUR_FORMAT(uint8_t *, http_p, p);

	if (pid_http_request_url(http_p, (uint8_t *)hytag->uri, &hytag->uri_len))
	{
		//hytag->app_type = URL_IN_NULL;
		return E_SUCCESS;
	}
	else
	{
		hytag->app_type = APP_TYPE_HTTP_GET_OR_POST;
	}
    	//printf("the url is:  %s\n", hytag->url);



	

	len = hytag->uri_len + 1;
	UPDATE_PBUF_OFFSET(p, len);
	PBUF_CUR_FORMAT(uint8_t *, http_p, p);

	if(pid_http_request_version_skip(http_p, &vsr_len))
	{
		return E_SUCCESS;
	}

	len = vsr_len + 1;
	UPDATE_PBUF_OFFSET(p, len);
#if 1
	PBUF_CUR_FORMAT(char *, context_p, p);

	char http_buf[1500];
	char *http_bak=http_buf;
        
	char line_buf[1500];
	char *line_bak=line_buf;
	
	memcpy(http_bak, context_p, p->len-p->ptr_offset);
	http_bak[p->len - p->ptr_offset+1] = '\0';	
			
	
	while(NULL != (line = strsep(&http_bak, "\n")))
	{

		line_bak= line_buf;
		strcpy(line_bak, line);
		begin = strsep(&line_bak, ":");
		if(  NULL != line_bak && NULL != begin )
		{	
		#if 0
			int len =  strlen(line_bak);
			if(len <= 2)
			{
				//printf("line = %s<%s>\n", line_bak, context_p);
				continue;
			}	
			if (hytag->host_len ==0 
					&& !strncasecmp(STRING_HTTP_HOST, begin, STRING_HTTP_HOST_LEN)) 
			{

				memcpy(hytag->host, &line_bak[1], (len-2));
				hytag->host_len = len -2;
			}
			if (hytag->user_agent_len== 0 
					&& !strncasecmp(STRING_HTTP_AGENT, begin, STRING_HTTP_AGENT_LEN))   
			{

				memcpy(hytag->user_agent, &line_bak[1], (len-2));
				hytag->user_agent_len = len-2;
			}				
		#endif
		}

	}


#endif
	/*check The First char*/
	if(hytag->uri[0] == '/')
	{
		hytag->url_len= snprintf(hytag->url, 256, "http://%s%s",hytag->host, hytag->uri);
	}
	return E_SUCCESS;
}




#else

berr pid_http_up(struct pbuf *p ,  hytag_t * hytag )
{
    
    uint8_t *http_p = NULL;
    char l5payload[PACKET_MTU];
    char *l5_ptr = NULL; 
    char * line = NULL;
    uint16_t l5_len = 0;
    char *method = NULL, *uri = NULL;    
    char *begin = NULL;
 
    
    l5_ptr = l5payload;
    l5_len = p->len - p->ptr_offset;


    
    PBUF_CUR_FORMAT(uint8_t *, http_p, p);
    if( l5_len <= 0  || l5_len >= PACKET_MTU)
    {
	    return E_SUCCESS;
    }	 	
    memcpy(l5_ptr, http_p, l5_len);
    l5_ptr[l5_len] = '\0'; 

    line = strsep(&l5_ptr, "\n");
    if(line != NULL)
    {
        method = strsep(&line, " ");
        if (NULL == method  || strncmp(STRING_HTTP_GET, method, STRING_HTTP_GET_LEN))
        {
        	pid_incr_count(APP_HTTP_OTHER);
        	return E_SUCCESS;
        }
        pid_incr_count(APP_HTTP_GET);
        uri = strsep(&line, " ");
        if(uri != NULL)
        {
            strncpy(hytag->uri, uri, URL_MAX_LEN);
            hytag->uri_len = strlen(uri);
            hytag->app_type = APP_TYPE_HTTP_GET_OR_POST;
        }
        else
        {
            return E_SUCCESS;
        }
        
    }
    else
    {
        return E_SUCCESS;
    }

	int len = 0;
	while(NULL != (line = strsep(&l5_ptr, "\n")))
	{
		if (NULL != (begin = strsep(&line, ":")))
		{	
			
            if( NULL == line)
                continue;

			len = strlen(line);
			if(len <= 2)
			{
				continue;
			}
            else
            {
			    len -= 2;//valid len
            }
            
			if (hytag->host_len ==0 
                && !strncmp(STRING_HTTP_HOST, begin, STRING_HTTP_HOST_LEN)) 
			{
               
                if( len > MAX_HOST_LEN )
				{	
					len = MAX_HOST_LEN;
				}
				memcpy(hytag->host, &line[1], len);
				hytag->host_len = len;
			}
			if (hytag->user_agent_len== 0 
                    && !strncmp(STRING_HTTP_AGENT, begin, STRING_HTTP_AGENT_LEN))   
			{
           
                if( len > MAX_USER_AGENT_LEN )
				{	
					len = MAX_USER_AGENT_LEN;
				}
				memcpy(hytag->user_agent, &line[1], len);
				hytag->user_agent_len = len;
			}			
		
		}
	}

    return E_SUCCESS;
}



#endif






#if 0
berr pid_http_get(struct pbuf *p,  hytag_t * hytag)
	    irintf("")	
{

       
}

berr pid_http_post(struct pbuf *p,  hytag_t * hytag)
{

       
}
#endif
