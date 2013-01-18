/* Doc- #################################
#######################################*/

#ifndef CONN
#define CONN

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
#include "client.h"
#include "../include/common.h"
#include "network.h"

#define MAXCLIENTS 10

class Connections 
    {
    private:
        pthread_t           lsnthrd;
        pthread_attr_t      lsnattr;
        
        int                 portnum, sockfd;
        struct sockaddr_in6 server_addr;
        
        // List of clients 
        list<Client *> *clients_list;        
        
    public:
        Connections(int port, list<Client *> *clients);
        ~Connections();
        
        void clisten();
        void caccept();
    };

#endif

