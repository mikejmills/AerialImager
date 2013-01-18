/* Doc- #################################
#######################################*/

#ifndef SERVER
#define SERVER
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
#include "MainList.h"
#include "../include/TileObj.h"

#define NETDB_MAX_HOST_NAME_LENGTH 256
class Server 
    {
    private:
        struct addrinfo    addr;   
        
        pthread_t          Thread;
        pthread_attr_t     attr;
        
    public:
        int                s_sock;
        char Addr_Name[NETDB_MAX_HOST_NAME_LENGTH];
        
        Server(int sock, struct addrinfo *addr);
        ~Server();
        
    };

#endif

