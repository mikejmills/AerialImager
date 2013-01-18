
#include "network.h"

// Connections global object
Connections *conn;

// Global Clients list
list<Client *>  clients;
pthread_rwlock_t clients_rwmtx; 

pthread_mutex_t  Send_mtx;

int Network_Init(int port)
{
    pthread_rwlock_init(&clients_rwmtx, NULL);
    
    pthread_mutex_init(&Send_mtx, NULL);
    
    // Setup the connection for listening
    conn = new Connections(port, &clients);
    return 1;
}


void Network_Close()
{
    delete conn;
}


bool remove_dup( Client* cli)
{
    list<Client *>::iterator i;
    
    for (i=clients.begin(); i != clients.end(); ++i) {

        if ( !strcmp( (*i)->Addr_Name, cli->Addr_Name ) ) {
            clients.remove(*i);
            delete (*i);
            return 1;
        }
    }
    
    return 0;
}

void Add_Client(int sock, struct sockaddr_storage *addr, int addr_size)
{
    Client *cli = new Client(sock, addr, addr_size);
    
    pthread_rwlock_wrlock(&clients_rwmtx);
    //......................................    
    
    // Check for duplicate clients and remove them
    remove_dup(cli);
    clients.push_front( cli );
    
    //......................................    
    pthread_rwlock_unlock(&clients_rwmtx);

}

void Send_Data(unsigned char *ptr, size_t size)
{
    list<Client *>::iterator i;
    
    
    pthread_rwlock_rdlock(&clients_rwmtx);
    
    //......................................   
    for ( i=clients.begin(); i != clients.end(); ++i) {
    //......................................   
        pthread_rwlock_unlock(&clients_rwmtx);    
 
        
        pthread_mutex_lock(&Send_mtx);
        (*i)->Send(ptr, size);
        pthread_mutex_unlock(&Send_mtx);
        
        pthread_rwlock_rdlock(&clients_rwmtx);
    //......................................        
    }
    
    //......................................    
    pthread_rwlock_unlock(&clients_rwmtx);
}
