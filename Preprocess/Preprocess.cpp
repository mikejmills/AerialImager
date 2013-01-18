
#include <pthread.h>
#include "Preprocess.h"
#include "particle.h"
#include <opencv/highgui.h>

#define MAX_THREADS 1 // Maximum number of preprocessing threads


int thrd_count = MAX_THREADS;
pthread_mutex_t  thrd_count_mtx;

// The function the handles the resulting data
void (*handler)(unsigned char *ptr, size_t len);

struct thread {
    pthread_t       thread;
    pthread_attr_t  attr;
};

struct thread Threads[MAX_THREADS];


int thrd_count_inc()
{
    pthread_mutex_lock(&thrd_count_mtx);
    
    if ( thrd_count >= MAX_THREADS ) {
        pthread_mutex_unlock(&thrd_count_mtx);
        return 0;
    }
   
    thrd_count++;
    
    pthread_mutex_unlock(&thrd_count_mtx);
    
    return 1;
}


int thrd_count_dec()
{

    pthread_mutex_lock(&thrd_count_mtx);

    
    if ( thrd_count <= 0 ) {
        pthread_mutex_unlock(&thrd_count_mtx);
        return 0;
    }
   
    thrd_count--;
    
    pthread_mutex_unlock(&thrd_count_mtx);
    
    return 1;   
}

//----------------------------------------------------------------------------------------

void Pre_init(void ( *func)(unsigned char *ptr, size_t len) )
{
    thrd_count = MAX_THREADS;
    pthread_mutex_init(&thrd_count_mtx, NULL);
    
    handler = func;
}


void *Pre_Thread(void *arg)
{
    char *filename = (char *) arg;
    char imgfile[MAX_PATH_LENGTH];
    int count = 0;
    IplImage *img = NULL;
    TileFile tf;
    
    
    imagefilename(filename, imgfile, strlen(filename));

    
    
    // Load the image or wait for it to appear
    // if it does not after 10 seconds then quit
    while ( !img ) {
        img = cvLoadImage(imgfile);
        printf("Image %X %s\n", img, imgfile);
        sleep(1);
        count++;
        if (count > 10) goto Bail; 
        
    }
    
   

    tf = Gen_Tile_File(filename, img);
    
    // Tile file failed or was not worth doing.
    if (!tf) {
        cvReleaseImage(&img);
        goto Bail;
    }
    
    //---------------------------------------------------------------
    
    // Calculate the perspective correct points for tile
    Persp_Correction(tf, CAMERA_ANGLE, ASPECT_RATIO);
    
    tf = target_find(tf, img);
    
    //---------------------------------------------------------------
    
  
    Send_Data(tf, TILE_HEADER_SIZE + INT(tf, MAIN_HEADER_total_size));
  
    printf("SEND Tile %s \n", imgfile);
    
    cvReleaseImage(&img);
    Release_Tile_File(tf);
    

Bail:
    // free the filename memory passed in and free the thread usage
    free(filename);
    thrd_count_inc();
    
    return NULL;
}


void Pre_Create(char *filename)
{
    int res, len = strlen(filename) + 1; // +1 saves the \0 at end

    
    // Wait until a thread finishes if necessary
    while( !thrd_count_dec() ) sleep(1); 
    
    

    pthread_mutex_lock( &thrd_count_mtx );

    // set detached thread
    pthread_attr_init( &(Threads[thrd_count].attr) );
    pthread_attr_setdetachstate( &(Threads[thrd_count].attr), PTHREAD_CREATE_DETACHED );
    
    // Allocate fname to pass into new thread
    char *fname = (char *) malloc( len * sizeof(char) );

    strncpy(fname, filename, len);
    fname[len]='\0';
    
    res = pthread_create( &(Threads[thrd_count].thread), 
                          &(Threads[thrd_count].attr), 
                          &Pre_Thread, 
                          (void *)fname );
    
    if ( res ) {
        pthread_mutex_unlock( &thrd_count_mtx );
        fprintf(stderr, "Error creating preprocess thread\n");
        thrd_count_inc(); // undo the thread usage
    }
    
    pthread_mutex_unlock( &thrd_count_mtx );
   
    return;
    
}
