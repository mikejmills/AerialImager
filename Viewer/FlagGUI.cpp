
#include "FlagGUI.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include "ViewRec.h"

pthread_t       Flagthread;
pthread_attr_t  Flagattr;

int send_socket = -1;

#define SOCKET_PATH "/tmp/viewer_socket"
#define PACKET_SIZE 256
//
// Setup the Domain socket return the connection socket descriptor
int setup_socket_server()
{
    struct sockaddr_un address;
    int socket_fd, conn_fd;
    size_t address_length;
    
    
    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        perror("Socket creation failed\n");
        return 0;
    } 
    
    
    // make sure the file node is free
    unlink(SOCKET_PATH);
    address.sun_family = AF_UNIX;
    address_length = sizeof(address.sun_family) + sprintf(address.sun_path, SOCKET_PATH) + 1; // add extra 1 because it seems to need it
    
    
    // Bind the socket to the address
    if(bind(socket_fd, (struct sockaddr *) &address, address_length) != 0)
    {
        perror("bind() failed\n");
        close(socket_fd);
        return 0;
    }

    if(listen(socket_fd, 5) != 0)
    {
        perror("Listen failed\n");
        close(socket_fd);
        return 1;
    }
    
    
    conn_fd = accept(socket_fd, (struct sockaddr *) &address, (socklen_t *)&address_length);
    
    if (conn_fd < -1) {
        perror("Accept connection error");
        return -1;
    }
    
    close(socket_fd);
    
    return conn_fd;
}


//
// Handle any incomming data
int read_signal(int sock_fd, char *buff, int size)
{
    return read(sock_fd, buff, size);
}


void Add_Flag(double x, double y, double lat, double lon)
{
    char data[PACKET_SIZE];
    if (send_socket == -1) return;
    
    // Create the string to send
    snprintf(data, PACKET_SIZE, "%.5f:%.5f:%.5f:%.5f\n", x, y, lat, lon);
    
    if (write(send_socket, data, PACKET_SIZE) != PACKET_SIZE)
        fprintf(stderr, "Not all data about flag sent\n");
    
    return;
}


void *FlagGUI_func(void *)
{
    char command[PACKET_SIZE];
    double x, y;
    
    // Cycle the connection listen code to allow only one
    // client at a time but allow the client to disconnect
    // and reconnect
    while (1) {
        // Setup and wait for connection
        send_socket = setup_socket_server();
        if (  send_socket == -1) return NULL;

        // listen for commands and run the events
        while(1) {
            if ( !read_signal(send_socket, command, PACKET_SIZE) ) break;
            x = atof(command);
            y = atof(command + strlen(command) + 1);
            
            // Make sure that the View Rectangle has been initalized
            if (!GVRec) continue;
            printf("MOVE TO %f %f\n", x, y);
            // Jump to the given coordinates
            GVRec->GLMove(x, y);
        }
    
        close(send_socket);
        unlink(SOCKET_PATH);
    }
    
    
    return NULL;
}


void FlagGUI_Start()
{
    int res;
    
    pthread_attr_init( &Flagattr );
    pthread_attr_setdetachstate( &Flagattr, PTHREAD_CREATE_DETACHED );
    res = pthread_create( &Flagthread, &Flagattr, &FlagGUI_func, NULL);
    
    if ( res ) {
        fprintf(stderr, "Error creating FlagGUI thread\n");
    }
    
    return;
}


