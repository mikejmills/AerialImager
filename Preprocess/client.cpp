
#include "client.h"

Client::Client(int sock, struct sockaddr_storage *addr, int addr_size)
{
    // Socket save
    csock = sock;
    
    // copy address 
    memcpy( (void *) &caddr, (void *)addr, addr_size );
    
    // get host name from sockaddr
    if ( getnameinfo( (struct sockaddr *) &caddr, addr_size, 
                       Addr_Name, sizeof(Addr_Name),
                       NULL, 0, 0 ) ) {
        fprintf(stderr, "Error getting client host name\n");
        strcpy(Addr_Name, "Unknown");
    }
    
}


Client::~Client()
{
    close(csock);
}


int Client::Send(unsigned char *data, size_t size)
{
    unsigned int snt = 0; //sent size
    
    // Send all data
    while (size > snt) {
        snt += write(csock, data + snt, size - snt);
    }
    
    return 1;
    
}

