/* Doc- #################################
#######################################*/

#ifndef TEXTURE_T
#define TEXTURE_T

#include <opencv/cv.h>
#include "../include/common.h"
#include <pthread.h>
#include "gl.h"

#define NO_TEXTURE -1


#define IS_TEX_STATE(s) ((s > TEX_STATE_BEGIN) && (s < TEX_STATE_END))     

class Texture
{
private:
    char             *filename;                // filename of the images Tile structure
    IplImage        *disk_img;                 // pointer to the data from disk
    TileFile        filestructure;             // structure of the Tile file on disk
    unsigned char * target_array;              // Pointer to TileObjs target data
    unsigned int      level;                   // image level 
    pthread_mutex_t t_mtx;                     // Locking for this struct
    
    void Draw_Label();                         //Draw Image label
    void Draw_Targets();                       // Draw the target rectangles right into the image
    
    
public:
    
    GLuint        GLtexName;                   // GL texture identity
    
    Texture(char *fname, TileFile data, unsigned char *targets);
    ~Texture();
    
    
    // Returns current image level
    int Current_Level();
    
    // Loads the image from disk into IplImage 
    // Returns 0 on failure
    int Disk_Load(int level);
    
    // Loads the Image into the GL 
    // Returns 0 on failure
    int GL_Load();                             
    
    // Release the memory image
    void MemImgRelease();              
    
    // Release the OpenGL texture memory
    void GLImgRelease();                  
    
};


#endif

