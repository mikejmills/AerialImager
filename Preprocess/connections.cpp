
#include "connections.h"

void *listen_thread(void *arg)
{
    Connections *conn = (Connections *)arg;
    
    conn->clisten();
    conn->caccept();
    return NULL;
}

Connections::Connections(int port, list<Client *> *clients)
{
    int res;
    
    portnum = port;
    clients_list = clients;
    
    // set detached thread
    pthread_attr_init( &lsnattr );
    pthread_attr_setdetachstate( &lsnattr, PTHREAD_CREATE_DETACHED );
    
    res = pthread_create( &lsnthrd, &lsnattr, listen_thread, (void *)this );
}


Connections::~Connections()
{
    //pthread_kill(lsnthrd, 15);
    pthread_cancel(lsnthrd);
    close(sockfd);
}


void Connections::clisten()
{
    int on = 1;
    
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    
    if (sockfd < 0) {
        perror("Socket");
        exit(1);
    }
    
    bzero( (char *) &server_addr, sizeof(server_addr) );
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) 
    {
        perror("setsockopt(SO_REUSEADDR) failed");
    }
    
    server_addr.sin6_family   = AF_INET6;
    server_addr.sin6_addr     = in6addr_any;
    server_addr.sin6_port     = htons(portnum);
    
    if ( bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 ) {
        perror("Bind");
        exit(1);
    }
    
    if (listen(sockfd, 10) < 0) {
        perror("Listen");
    }
}


void Connections::caccept()
{
    fd_set fdsock, fdtest;
    int    res, client_sock;
    
    struct sockaddr_storage client_addr;
    socklen_t               client_len = sizeof(struct sockaddr_storage);
    
    FD_ZERO(&fdsock);
    FD_SET(sockfd, &fdsock);
    
    while(1) {
        fdtest = fdsock;
        res = select( MAXCLIENTS,
                         &fdtest,
                         (fd_set *)NULL,
                         (fd_set *)NULL,
                         (struct timeval *) NULL
                       );
        
        if (res < 1) {
            fprintf(stderr, "Error accepting connection\n");
            continue;
        }
        
        for( int i = 0; i < MAXCLIENTS; i++ ) {
            // Not this fd
            if ( !FD_ISSET(i, &fdtest) ) continue;
            
            // Accept the connection
            if (i == sockfd) {
                cout << "Got new Connection" << endl;
                client_sock = accept( sockfd, (struct sockaddr *) &client_addr, (socklen_t *) &client_len );
                Add_Client(client_sock, &client_addr, (int)client_len);
            }
        }
    }
    cout << "Done accepting" << endl;
}