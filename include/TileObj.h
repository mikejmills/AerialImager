/* Doc - ############################################
   tile object is the structure that hold the image data and
   and knows how to load the image from a file
   ############################################
*/

#ifndef TILEOBJ
#define TILEOBJ

#include <opencv/cv.h>
#include "../include/common.h"
#include "../minilzo/minilzo.h"
#include "../include/Vector.h"
#include "../Viewer/GeoCoord.h"
#include <pthread.h>



#define NUM_IMAGES 3

// unsigned char array

#define INT_SIZE    (8)
#define DOUBLE_SIZE (8)

// Allows for use of = to set or get the value
//  the INT_SIZE - sizeof(int) compensates for differing type size
#define INT(name, offset) *(int *)(name + (INT_SIZE - sizeof(int)) + offset)
#define DOUBLE(name, offset) *(double *)(name + (DOUBLE_SIZE - sizeof(double)) + offset)

//---------------------------------------------------------------------------------
/* Main header of a tile
 int Total Size
 DOUBLE_SIZE lat, lon, alt, roll, pitch, heading, target_count
 */
#define MAIN_HEADER ((INT_SIZE * 2) + (DOUBLE_SIZE * 7))

// Offsets to the memory
#define MAIN_HEADER_total_size  0 
#define MAIN_HEADER_lat         (INT_SIZE)
#define MAIN_HEADER_lon         (MAIN_HEADER_lat + DOUBLE_SIZE)
#define MAIN_HEADER_alt         (MAIN_HEADER_lon + DOUBLE_SIZE)
#define MAIN_HEADER_roll        (MAIN_HEADER_alt + DOUBLE_SIZE)
#define MAIN_HEADER_pitch       (MAIN_HEADER_roll + DOUBLE_SIZE)
#define MAIN_HEADER_heading     (MAIN_HEADER_pitch + DOUBLE_SIZE)
#define MAIN_HEADER_targetcount (MAIN_HEADER_heading + INT_SIZE)
#define MAIN_HEADER_imgid       (MAIN_HEADER_targetcount + INT_SIZE)

//---------------------------------------------------------------------------------
/*  Tile Points
 DOUBLE_SIZE tleft[3], tright[3], bleft[3], bright[3], center[3]
 */
#define TILE_POINT_VEC_SIZE (DOUBLE_SIZE * 3)
#define TILE_POINTS         (TILE_POINT_VEC_SIZE * 5)

// Access a Tile_Point Vector element
#define TILE_POINT_OFF(index, elem) ( ( (TILE_POINT_VEC_SIZE * index) + (elem * DOUBLE_SIZE) ) + MAIN_HEADER )

//---------------------------------------------------------------------------------

/* Image header 
 int image_size, compressed_size, width, height, depth, nChannels, offset
 */
#define IMAGE_HEADER (INT_SIZE * 7)

#define IMAGE_ACCESS(index) (MAIN_HEADER + TILE_POINTS + (IMAGE_HEADER * index))

#define IMAGE_HEADER_image_size(index)      IMAGE_ACCESS(index) 
#define IMAGE_HEADER_compressed_size(index) (IMAGE_ACCESS(index) + INT_SIZE)
#define IMAGE_HEADER_width(index)           (IMAGE_ACCESS(index) + (INT_SIZE * 2) )
#define IMAGE_HEADER_height(index)          (IMAGE_ACCESS(index) + (INT_SIZE * 3) )
#define IMAGE_HEADER_depth(index)           (IMAGE_ACCESS(index) + (INT_SIZE * 4) )
#define IMAGE_HEADER_nChannels(index)       (IMAGE_ACCESS(index) + (INT_SIZE * 5) )
#define IMAGE_HEADER_offset(index)          (IMAGE_ACCESS(index) + (INT_SIZE * 6) )

//---------------------------------------------------------------------------------
/*
  Image targets 
    center_x, center_y, width height
*/
#define IMAGE_TARGETS_HEADER     (INT_SIZE * 4)

#define IMAGE_TARGETS_center_x   0
#define IMAGE_TARGETS_center_y   (INT_SIZE)
#define IMAGE_TARGETS_width      (IMAGE_TARGETS_center_y + INT_SIZE)
#define IMAGE_TARGETS_height     (IMAGE_TARGETS_width    + INT_SIZE)

    
//---------------------------------------------------------------------------------

// Tile headers size complete size
#define TILE_HEADER_SIZE (MAIN_HEADER + TILE_POINTS + (IMAGE_HEADER * NUM_IMAGES))

// Access tile target point at index
#define IMAGE_TARGET(tf, index)  (tf + TILE_HEADER_SIZE + (INT(tf, MAIN_HEADER_total_size) - (INT(tf, MAIN_HEADER_targetcount) * IMAGE_TARGETS_HEADER)) + (index * IMAGE_TARGETS_HEADER))

// Create a TILE in static memory
#define STATIC_TILE(name) unsigned char name[TILE_HEADER_SIZE]

// Create a Tile in Dynamic memory
#define DYNAMIC_TILE(name) unsigned char *name = (unsigned char *)malloc( TILE_HEADER_SIZE );

// Clear the Tile header to zero (must be done)
#define INIT_TILE(name) memset( name, 0x00, TILE_HEADER_SIZE );


//---------------------------------------------------------------------------------
void decompress_image(IplImage *dst, lzo_uint dst_size, 
                      unsigned char *src, lzo_uint src_size );

// Func
// Create a structure that can be used to write a tile file
TileFile Gen_Tile_File(char *datfilename, IplImage *baseimg);

// Func
// Release a tile file struct including compressed data
void Release_Tile_File(TileFile tf);

// Add some extra data to the TileFile structure
// Returns the new size of the TileFile
unsigned char *Add_Targets( TileFile tf, unsigned char *targets_mem, int num );

// Take datfilename and turn it into the image filename
void imagefilename(char *filename, char *imgfile, int len);


#ifdef VIEWER

#include "../Viewer/texture.h"
#include "threadpool.h"


#define GL_LOAD_TEXTURE 1
#define GL_NO_TEXTURE   -1
#define GL_SAME_TEXTURE 2

extern Thread_Pool *Tile_Thread_Pool;

// Initialize the TileObj Thread Pool for
// loading of images from disk
void TileObj_Init();

// Internal Tile States
typedef enum {
    TILE_INACTIVE=0,
    TILE_ACTIVE_DISK_LOADING,
    TILE_ACTIVE_DISK_LOADING_WAIT,
    TILE_ACTIVE_GL_LOAD,
    TILE_ACTIVE_IMAGE,
    TILE_START_UNLOAD
} TILE_STATE;

// Signals Returned for the GL handler
typedef enum {
    TILE_GL_DEFAULT,
    TILE_GL_LOAD,
    TILE_GL_DRAW
} TILE_GL_SIGNAL;


// Signals Returned for the Standard handler
typedef enum {
    TILE_STD_DEFAULT,
    TILE_STD_DISK_LOADING,
    TILE_STD_INACTIVE
} TILE_STD_SIGNAL;

// TileObj Class
class TileObj
    {
    private:
        // Set if the tile is to free its texture data.
        //    Necessary because GL textures must be freed by the 
        //    GL thread. While the decision to not draw a tile is
        //    made by a seperate thread.
        TILE_STATE state; 
        
        char filename[MAX_PATH_LENGTH];
        STATIC_TILE(tiledata); 
        unsigned char *target_array;
        int image_load_level;
        
        // Mutex Protection
        pthread_mutex_t tile_mtx;
        
        void lock();
        int  trylock();
        void unlock();
        
        //Drawing Routines
        int Draw();
        void draw_texture_box();
        void draw_empty_box();

        
        // Position in geographic coordinates
        GeoCoord geo_position;
        
        // Setup GL texture projection
        void tex_project();
        
        
    public:
        // Texture object 
        Texture *texobj;
        
        // Position in OpenGL coordinates
        Vec *GLpos;
        
        TileObj(TileFile tf);
        ~TileObj();

        // Returns the GeoPosition
        //  no locking because after initial pass it should not change
        GeoCoord *Get_GeoPosition();
        
        // Perform a GL load of the tile texture
        int GL_Load();
        
        // returns the altitude of the image was taken at
        double Get_Altitude();
        
        void Set_GLpos(GeoCoord *origin);
        
        // Signal the tile to acitivate
        int Activate();
       
        // Signal the Tile to deactivate 
        // freeing all memory
        int Deactivate();
        
        
        // This function must be run by the thread which
        // holds the GL context
        TILE_GL_SIGNAL GL_run();
        
        // This function can be run by any thread and
        // runs operations that are important to changing state
        TILE_STD_SIGNAL Standard_run(int image_level);
        
        // Function is suppose to be run inside a seperate thread
        // to allow other disk access without freezing.
        void thrd_image_load();
        
    };

#endif

#endif

