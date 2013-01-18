#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "connections.h"
#include "vfs.h"
#include "Display.h"
#include <getopt.h>
#include "MainList.h"
#include "DisplayThread.h"

#define opt_dirc    'd'
#define opt_serverc 's'
#define PORT_STR "9999"

// Arguments Option
struct option longopts[] = {
    { "dir", 1, NULL, opt_dirc },
    { "server", 1, NULL,  opt_serverc }
};


void commands(int argc, char *argv[], Connections *conn)
{
    int opt;
    int has_dir = 0,
        has_conn = 0;
    while ( (opt = getopt_long(argc, argv, "if:lr", longopts, NULL)) != -1 ) {
        switch(opt) {
                
            case opt_dirc:
                
                VFS_Init(optarg); //Initialize the VFS
                has_dir = 1;
                break;
                
            case opt_serverc:
                
                if (conn->Connect(optarg, PORT_STR) < 0) // Connect to a server
                    break;
                has_conn = 1;
                
                break;
                
        }
    }
    
    // Initalize VFS if not done already
    if (!has_dir) VFS_Init("./");
    
    // Check that a connection has been made
    if (!has_conn) { 
        printf("You have not connected to any servers\n");
        exit(1);
    }

}


int main(int argc, char *argv[])
{
    Connections conn;
    
    Display_Init(argc, argv);
    commands(argc, argv, &conn);
    DispThrd_Init();
    Display_Run();
    
    return 0;
}
