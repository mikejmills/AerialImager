
#include "server.h"
#include <opencv/highgui.h>
#include <opencv/cv.h>

// Reads data until it has read size bytes
// if for some reason it fails returns number of bytes 
// left to read
int read_data(int sock, unsigned char *buff, int size)
{

    int len, total = 0;
    
    while ( size > total ) {
        len = read(sock, buff + total, size - total);
        if ( len == -1 ) {
            perror("Read data");
            return -1;
        }
        total += len;
       
    }
    
    //cout << " exit Size " << size << " " << len << endl;
    
    return size - total;
}

// Read the TileFile Header and data into one large allocation and return
// the TileFile on success and NULL otherwise
TileFile recv_TileFile(Server *srvptr)
{
    int res;
    size_t len; 
    
    STATIC_TILE(header);
    DYNAMIC_TILE(tf);

    
    // Read the header
    // Read the TileObj header information first 
    if ( read_data(srvptr->s_sock, header, TILE_HEADER_SIZE) != 0 ) {
        fprintf(stderr, "Cannot read all of header %d bytes remain\n", res);
        return NULL;
    }
    
    // Realloc memory for header and the image data
    tf = (TileFile) realloc(tf, TILE_HEADER_SIZE + INT(header, MAIN_HEADER_total_size));
    
    if (!tf) {
        fprintf(stderr, "Cannot allocate memory of new tile\n");
        return NULL;
    }
        
    // Copy header info into the new memory region
    memcpy(tf, header, TILE_HEADER_SIZE);

    // Read the data image data still in Kernel buffers
    if ( read_data(srvptr->s_sock, tf + TILE_HEADER_SIZE, INT(header, MAIN_HEADER_total_size)) != 0) {
        fprintf(stderr, "Could not read all Tile File data %d\n", res);
        Release_Tile_File(tf);
        return NULL;
    }
    
    printf("Recv TileFile %d\n",INT(header, MAIN_HEADER_total_size) + TILE_HEADER_SIZE );

    return tf;
    
}


void *server_thread(void *arg)
{
    TileFile tf;
    
    Server *srvptr = (Server *)arg;
    
    
    while(1) {
        
        
        // Make sure no errors have occured
        // at this point no retry is attempted the Image is essentially lost
        tf = recv_TileFile(srvptr);
        if ( !tf ) {
            fprintf(stderr, "Tile File not received correctly\n");
            continue;
        }
        
        // Add new TileObj to the Main List 
        TileObj *tile = new TileObj(tf);
        if (!tile) 
            fprintf(stderr, "The Tile object could not be created\n");
        else
            Main_Tile_List.Insert( tile );
        
    }
    
    return NULL;
}


Server::Server(int sock, struct addrinfo *info)
{
    
    // Set sock
    s_sock = sock;
    
    // Copy addrinfo stuff
    memcpy((void *)&addr, (void *)info, sizeof(struct addrinfo));
    
    // Create thread to listen for new data
    if ( pthread_create(&Thread, NULL, &server_thread, (void *) this) )
        fprintf(stderr, "Error creating server thread %s\n", addr);
    
}


Server::~Server()
{
    pthread_cancel(Thread);
    close(s_sock);
}



