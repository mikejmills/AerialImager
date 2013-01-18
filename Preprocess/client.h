/* Doc- #################################
#######################################*/

#ifndef CLIENT
#define CLIENT

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
#include "../include/common.h"

#define NETDB_MAX_HOST_NAME_LENGTH 256

class Client 
    {
    private:
        int csock;
        struct sockaddr_storage caddr;
        
    public:
        char Addr_Name[NETDB_MAX_HOST_NAME_LENGTH];
        
        Client(int sock, struct sockaddr_storage *addr, int addr_size);
        ~Client();
        
        int Send(unsigned char *data, size_t size);
    };

#endif

