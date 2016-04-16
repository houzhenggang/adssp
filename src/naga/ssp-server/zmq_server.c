
//  Lazy Pirate server
//  Binds REQ socket to tcp://*:5555
//  Like hwserver except:
//   - echoes request as-is
//   - randomly runs slowly, or exits to simulate a crash.

#include "zhelpers.h"
#include <unistd.h>
#include "bts_hashtable.h"

#include "ad.h"
#include "ssp_server.h"

typedef enum
{
	ENUM_ZMQ_HOST = 0,
	ENUM_ZMQ_USER_AGENT,
	ENUM_ZMQ_COOKEIS,	
}enum_msg_t;


#define PHP_SPLIT  "__php_split__"


extern uint64_t  drop_push_cnt_total ;
extern uint64_t  success_push_cnt_total ;

extern uint64_t  today_end_second;



int zmq_server_init (void)
{
	pthread_t thread;

	char * con1 = "tcp://*:5555";
	char * con2 = "tcp://*:5556";
	
	pthread_create(&thread, NULL, zmq_server_init_by, con1);
	pthread_detach(thread);

	pthread_create(&thread, NULL, zmq_server_init_by, con2);
	pthread_detach(thread);	
	return 0;
}

void*  zmq_server_init_by (void * prog)
{

	char buffer[2048];
	int size;
	char sendbuffer[2048];
    int cycles = 0;
	int l = 0;
	char *section = NULL;
	int section_offset = 0;
	apply_info_t  info;
	char *buf_ptr = buffer;
	ad_struct_t* adlist = NULL;

    void *context = zmq_ctx_new ();
    void *server = zmq_socket (context, ZMQ_REP);
    zmq_bind (server, (char *)prog);

	printf("init server %s\n", (char *)prog);
		
    while (1) {

		size=zmq_recv (server, buffer, 2000, 0);
		buf_ptr = buffer;

		if(size == -1)
		{
			printf("zmq_recv Failed\n");
			break;;
		}
		
		buffer[size] = 0;		
		//printf("recv len(%d) %s\n", size, buffer);
		section_offset = 0;
		section = NULL;
		memset(&info, 0x0 , sizeof(info));
		
		while  (NULL != ( section = strsep(&buf_ptr, "@")))
		{
				switch(section_offset)
			{
				/*
				case 0:
					info.adtype = strtoul(section, NULL, 0);
					section_offset++;
					break;
				*/
				case 0:
					strncpy(info.useragent ,(section), 1024);
					section_offset++;
					break;
				case 1:
					strncpy(info.refer,(section), 1024);
								section_offset++;
					break;
				case 2:
					strncpy(info.userip, (section), 16);
					section_offset++;
					break;
				case 3:
					if(section != NULL)
					{
						strncpy(info.cookies, (section), 1024);
						info.cookies_len = strlen(info.cookies);
						section_offset++;
					}
					
					break;
				default:
					printf("Failed to Success\n");

					break;

			}
		}
		info.ntime = time(NULL);
		#if 0
		printf("adtype = %d\n", info.adtype);
		printf("useragent = %s\n", info.useragent);
		printf("refer = %s\n", 		info.refer);
		printf("ip=%s\n", info.userip);
		printf("cookies = %s\n", info.cookies);
		#endif

		if( strstr(info.useragent, "Phone") 
			|| strstr(info.useragent, "Android")
			|| strstr(info.useragent, "iPad")
			|| strstr(info.useragent, "BlackBerry")
			|| strstr(info.useragent, "SymbianOS")
		
			)
		{

			info.adtype  = 2;	
		}
		
		else if(
			strstr(info.useragent, "MSIE")
		   || strstr(info.useragent, "Windows")
		   ||strstr(info.useragent, "Chrome")
		   || strstr(info.useragent, "Macintosh")
		   )
		{
			info.adtype  = 1;
		}
		else
		{
			info.adtype = 2;
		}


		
		int times = 0;
		uint32_t cell = 0;
		
		if(info.cookies_len == 0)
		{
			times = 0;
			
			if(usercookeis_assess_add(info.cookies, 
							&(info.cookies_len)) != E_SUCCESS)
			{
				goto err_code;
			}		
		}
		else
		{
			times = 1;
		}
		#if 0
		else
		{

			
			times = usercookeis_assess_check(info.cookies, info.cookies_len);

			
			if(times < 0 )
			{
				goto err_code;
			}	

		}
		#endif
		
		adlist =  apply_valid_ad (&info, times);
		if(adlist == NULL)
		{
			goto err_code;
		}
		else 
		{	

			if(adlist->typejs)
			{
				//printf("adlist typejs = %d\n");
				if(adlist->jscode == NULL)
				{
																			
				}
				else
				{
					l = snprintf(sendbuffer, 2048, 
						"$cookes=\"%s\";setcookie(\"__host_COOK\", $cookes, %d);"PHP_SPLIT"%s",
						 info.cookies, today_end_second
						,adlist->jscode);
				}				
			}
			else
			{
				int pc_mb_name = 0;
				switch(info.adtype)
				{
					case 1:
						pc_mb_name = 15;
						break;
					case 2:
						pc_mb_name = 16;
						break;					
				}

				
				if(times == 0) //without cookiess
				{
					l = snprintf(sendbuffer, 2048,
					"$cookes=\"%s\";setcookie(\"__host_COOK\", $cookes, %d);"
					PHP_SPLIT
					"document.write(suspendcode%d);"
					"document.getElementById(\"suspendcode15iframe\").src=\"%s\";"
					"setTimeout(\"close_framer()\", %d);"
					, info.cookies, today_end_second
					 ,pc_mb_name, adlist->push_url,
					 adlist->showtime*1000);

						
				}
				else
				{
					l = snprintf(sendbuffer, 2048,
					"return;"	
					PHP_SPLIT	
					"document.write(suspendcode%d);"
					"document.getElementById(\"suspendcode15iframe\").src=\"%s\";"		
					"setTimeout(\"close_framer()\", %d);"
					,pc_mb_name , adlist->push_url, adlist->showtime*1000);
				}	
			}
			
		}	

	size= zmq_send(server, sendbuffer, l , 0);
	adlist->cnt_push_all_day++;
	adlist->cnt_push_one_day++;	

	success_push_cnt_total++;
	//printf("send len(%d) %s\n", size, sendbuffer);
	continue;
err_code:
		size= zmq_send(server, "return;", 6 , 0);
		drop_push_cnt_total++;
		continue;
    }
	
    zmq_close (server);
    zmq_ctx_destroy (context);
    return NULL;
}

