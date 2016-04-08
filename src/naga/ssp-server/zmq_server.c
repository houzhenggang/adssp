
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

int zmq_server_init (void)
{
    srandom ((unsigned) time (NULL));

    void *context = zmq_ctx_new ();
    void *server = zmq_socket (context, ZMQ_REP);
    zmq_bind (server, "tcp://*:5555");
	char buffer[2048];
	int size;
	
    int cycles = 0;
    while (1) {
		
		size=zmq_recv (server, buffer, 2000, 0);

		if(size = -1)
		{
			printf("zmq_recv Failed\n");
			continue;
		}
		buffer[size] = 0;
		printf("recv len(%d) %s\n", size, buffer);
        s_send (server, "OK");
        free (request);
    }
    zmq_close (server);
    zmq_ctx_destroy (context);
    return 0;
}
