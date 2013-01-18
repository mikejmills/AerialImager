/* Doc- #################################
#######################################*/

#ifndef NETWORK
#define NETWORK

#include "connections.h"
#include "client.h"
#include <pthread.h>
#include <list>

// Initalize the network
int Network_Init(int port);

// close network 
void Network_Close();

// Send ptr data to all clients
void Send_Data(unsigned char *ptr, size_t size);
// Add a Client
void Add_Client(int sock, struct sockaddr_storage *addr, int addr_size);
#endif

