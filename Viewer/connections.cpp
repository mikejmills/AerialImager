
#include "connections.h"


Connections::Connections()
{
    server_count = 0;
}


Connections::~Connections()
{
    list<Server *>::iterator i;
    
    for (i=servers_list.begin(); i != servers_list.end(); ++i) {
        delete *i;
    }
}

int Connections::Connect(char *ip, char *port)
{
    int sock;
    
    struct addrinfo hints, *res = NULL;
    
    memset (&hints, '\0', sizeof (hints));
    
    hints.ai_flags    = 0;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ( getaddrinfo(ip, port, &hints, &res) ) {
        fprintf(stderr, "Error getting server name info %s\n", ip);
        return -1;
    }
    
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    
    if (sock == -1) {
        perror("socket error");
        return -1;
    }
    
    
    if ( connect(sock, res->ai_addr, res->ai_addrlen) ) {
        fprintf(stderr, "Cannot connect to %s \n", ip);
        return -1;
    }
    
    cout << "HERE" << endl;
    // Add this server connection to a list
    servers_list.push_front( new Server(sock, res) );
   
    
    return 1;
}