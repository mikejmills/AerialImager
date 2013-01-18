
#include "texture.h"
#include "vfs.h"
#include <opencv/highgui.h>


//-----------------------------------------------------------------------

Texture::Texture(char *fname, TileFile data, unsigned char *targets)
{
    filename      = fname; 
    filestructure = data;   // save the pointer to TileObj header info
    
    pthread_mutex_init(&t_mtx, NULL);
    
    target_array = targets;
    disk_img  = NULL;
    GLtexName = NO_TEXTURE;
    level     = -1;
}

Texture::~Texture()
{
    GLImgRelease();
    MemImgRelease();
    
}

void Texture::MemImgRelease()
{
    pthread_mutex_lock(&t_mtx);
    //...........................
    
    if (disk_img) cvReleaseImage(&disk_img);
    
    disk_img = NULL;

    //...........................
    pthread_mutex_unlock(&t_mtx);
    
}

void Texture::GLImgRelease() 
{
    pthread_mutex_lock(&t_mtx);
    //...........................
    
    if ((int)GLtexName != NO_TEXTURE) {
        GL_Release_Texture(&GLtexName);
        GLtexName = NO_TEXTURE;
    }
    
    //...........................
    pthread_mutex_unlock(&t_mtx);
    
}

int Texture::Current_Level()
{
    int l;
    
    pthread_mutex_lock(&t_mtx);
    //...........................
    
    l = level;
    
    //...........................
    pthread_mutex_unlock(&t_mtx);
    
    return l;
}

void Texture::Draw_Targets()
{
    // Make sure image is loaded
    if (!disk_img) return;
    if (!target_array) return;
    
    int center_x, center_y, width, height;
    float xf, yf;
    
    xf = INT(filestructure, IMAGE_HEADER_width(0))  / disk_img->width;
    yf = INT(filestructure, IMAGE_HEADER_height(0)) / disk_img->height;
    
    //printf("Factors %f %f\n", xf, yf);
    for (int i = INT(filestructure, MAIN_HEADER_targetcount) - 1; i >= 0; i--)
    {
        center_x = INT(target_array + i * IMAGE_TARGETS_HEADER, IMAGE_TARGETS_center_x);
        center_y = INT(target_array + i * IMAGE_TARGETS_HEADER, IMAGE_TARGETS_center_y);
        width    = INT(target_array + i * IMAGE_TARGETS_HEADER, IMAGE_TARGETS_width);
        height   = INT(target_array + i * IMAGE_TARGETS_HEADER, IMAGE_TARGETS_height);
        
        center_x /= xf;
        center_y /= yf;
        width  /= xf;
        height /= yf;
       // printf("Target %d %d \n",center_x, center_y);
        cvRectangle( disk_img, cvPoint(center_x - width/2, center_y - height/2), 
                     cvPoint(center_x + width/2, center_y + height/2), 
                     CV_RGB(255,0,0), 5);
    }
}

void Texture::Draw_Label()
{
    CvFont font;
    cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 1,1,0,1);
    
    char label[20];
    sprintf(label, "%d", INT(filestructure, MAIN_HEADER_imgid));
    
    cvPutText(disk_img, label, cvPoint(disk_img->width/2, disk_img->height/2), &font, cvScalar(255,255,255));
}

int Texture::Disk_Load(int index)
{
    
    // Make sure the previous image has been freed
    MemImgRelease();
    
    pthread_mutex_lock(&t_mtx);
    //...........................
    
    // Set the new current level
    level = index;   
    disk_img = VFS_ReadTileImage(filename, filestructure, level);

    if (!disk_img) {
        fprintf(stderr, "Failed to load image %s %d\n", filename, level);
        pthread_mutex_unlock(&t_mtx);
        //...............................
        return 0;
    }
    
    Draw_Targets();
    // only draw label at level 0
    if (level == 0)
        Draw_Label();
    
    //...........................
    pthread_mutex_unlock(&t_mtx);

    return 1;
}



int Texture::GL_Load()
{
    
    pthread_mutex_lock(&t_mtx);
    //..........................

    // Make sure the image is loaded into system memory
    if (!disk_img) {
        //............................
        pthread_mutex_unlock(&t_mtx);
        return 0;
    }
    
    //..........................
    pthread_mutex_unlock(&t_mtx);
        
    GLImgRelease();
        
    pthread_mutex_lock(&t_mtx);
    //............................
          
    
    
    // Create new GL texture object
    GLtexName = GL_Alloc_Texture();
    
    
    GL_Load_Texture(disk_img, GLtexName);
    //............................
    pthread_mutex_unlock(&t_mtx);
    
    // There is no longer a need for the Mem image
    // now that it is in GL memory
    MemImgRelease();
    
    return 1;
}


