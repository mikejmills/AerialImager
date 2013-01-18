
#include <iostream>
#include "DirHandle.h"
#include "../include/TileObj.h"
#include "Preprocess.h"
#include "connections.h"
#include "network.h"
#include "perspective.h"
#include <opencv/highgui.h>
#include <opencv/cv.h>

void handler( unsigned char *ptr, size_t len )
{
        
        
}

int main(int argc, char *argv[])
{
    
    //Pre_init();
    //DirHandler dir("../testdir/");
       
    /*
	struct TileFile *test = Gen_Tile_File("../testdir/00300.dat");
    int i=0;
    
    cout << test->images[i].width << " " << test->images[i].height << " " << test->images[i].depth << " " << test->images[i].nChannels << endl;
    IplImage *img = cvCreateImage(cvSize(test->images[i].width, test->images[i].height), 
                                  test->images[i].depth, 
                                  test->images[i].nChannels);
    
    decompress_image(img, test->images[i].image_size, (unsigned char*)((int)test->images[i].data_ptr + (int)test), test->images[i].compressed_size);
    
    cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
    cvShowImage("test", img);
    cvWaitKey(0);
    */
    
    //Release_Tile_File(test);
    //Connections conn(9999);
    
    //network_init(9999);
    //network_close();
    
    //    PerspTests();    
   
   // srand(time(NULL));
    Pre_init(handler);
    Network_Init(9999);
    DirHandler dir("../testdir/");
    
    while(1) sleep(10);
    
    Network_Close();
    cout << "Shut Down Server" << endl;
    
    

    return 0;
}
