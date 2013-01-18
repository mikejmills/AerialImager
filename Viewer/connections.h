/* Doc- #################################
#######################################*/

#ifndef CLIENT_CON
#define CLIENT_CON

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/poll.h>
#include <list>
#include "server.h"
#include "../include/common.h"

class Connections
    {
    private:
        list<Server *> servers_list;
        int server_count;
        
        
    public:
        Connections();
        ~Connections();
        
        int Connect(char *ip, char *port);
        
        
    };

#endif

