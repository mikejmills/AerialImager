
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <string>
#include <opencv/cv.h>
#include <math.h>
#include <opencv/highgui.h>
#include "../include/TileObj.h"
#include "../include/common.h"
#include "../minilzo/minilzo.h"
#include "vfs.h"
#include "gl.h"



using namespace std;


#define HEAP_ALLOC(var,size) \
lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

#define IMG_SIZE(img, div) ( (img->width * img->height)/div * img->nChannels ) 
//------------------------------------------------------------------------------------
// Compression of image returns a pointer to the data and the size
int compress_image( IplImage *src, unsigned char **cmpimg)
{
    int res;
    lzo_uint size = IMG_SIZE(src, 1);
    
    // Allocate some working memory
    HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);
    
    // Allocate some memory to hold the compressed data temporaraly to make sure we have enough room
    unsigned char  __LZO_MMODEL *tmp = (unsigned char __LZO_MMODEL *) malloc( size + size/16 + 83 );
    
    // Compress the image using LZO
    res = lzo1x_1_compress((const unsigned char __LZO_MMODEL *) src->imageData, size, tmp, &size, wrkmem);
    
    // Create precise sized holder for data
    *cmpimg = (unsigned char *) malloc( size );
    memcpy(*cmpimg, tmp, size);
    free(tmp);
    
    return size;
}

// Decompress the image 
void decompress_image(IplImage *dst, lzo_uint dst_size, 
                      unsigned char *src, lzo_uint src_size )
{
   lzo1x_decompress((unsigned char *)src, src_size, (unsigned char *)dst->imageData, &dst_size, NULL);
}

//------------------------------------------------------------------------------------
// Dat file Parser

enum PARSE_ELEM {
    LATITUDE = 9,
    LONGITUDE,
    GPSALTITUDE= 13,
    HEADING = 12,
    ROLL = 1,
    PITCH = 0
};


float parse_line( const char *line )
{
    int i, len=strlen(line);
    
    // Find string deliminator
    for (i = 0; i < len; i++) {
        if (line[i] == '=')
            break;
    }
    
    for (;i < len; i++) {
        if (!isdigit(line[i]) && (line[i] != '-') && (line[i] != '.')) {
            continue;
        } else {
            break;
        }
    }
    
    return atof(line + i);
}

void imagefilename(char *filename, char *imgfile, int len)
{
    strncpy(imgfile, filename, strlen(filename));
    imgfile[strlen(filename)] = '\0';
    
    strncpy(imgfile + (strlen(imgfile) - EXTEN_SIZE), IMAGE_FILE_EXTENSION, EXTEN_SIZE);
    imgfile[len] = '\0';
}

// Parse the Dat file
void ParseDat          ( char   *filename, 
                         double *lat, double *lon, 
                         double *alt, 
                         double *roll, double *pitch, double *heading, 
                         char   *imgfile )
{
    //double lat, lon, alt, hdn, roll, pitch, val;
    double val;
    string line;
    int len = strlen(filename), 
        i = 0;
    
    ifstream file (filename);
    if (!file.is_open()) return;
    
    
    while (!file.eof()) {
       
        getline(file, line);
        val = (double) parse_line( line.c_str() );
        
        switch(i++) 
        {
            case LATITUDE:
                *lat = val;
                break;
                
            case LONGITUDE:
                *lon = val;
                break;
                
            case GPSALTITUDE:
                *alt = val * 0.3048; // Meters 
                break;
            
            case HEADING:
                *heading = val;
                break;
            
            case ROLL:
                *roll = R2D(val);
                break;
            
            case PITCH:
                *pitch = R2D(val);
                break;
        }
       
    }

    imagefilename(filename, imgfile, len);    
    
    
    return;
    
}

//
// Get the image number from the filename
int image_number(char *filename)
{
    char *s = filename;
    if (!filename) return 0;
    
    filename = filename + strlen(filename);

    while ( (filename > s) && !isdigit(*filename) ) filename--;
    while ( (filename > s) && isdigit(*filename) ) filename--;
    
    if (filename <= s) return 0;
    return atoi(filename + 1);
}


// Create the Tile File structure for later use
TileFile Gen_Tile_File(char *datfilename, IplImage *baseimg)
{
    
    double          lat, lon, alt, roll, pitch, heading;
    char            imgfilename[MAX_PATH_LENGTH];
    unsigned char   *tmpptrs[NUM_IMAGES];
    
    // Create a tf header
    DYNAMIC_TILE(tf);
    INIT_TILE(tf);
    
    int size = 0;
    IplImage *cvimg;
    
    
    ifstream fd(datfilename);
    if (!fd.is_open()) goto Bail;
   
    // Parse the dat file and return each value
    ParseDat( datfilename, &lat, &lon, &alt, &roll, &pitch, &heading, imgfilename );

    // Pictures of less then 20 meters are not really usefull.
    if (alt < 20) {//***********************
        Release_Tile_File(tf);
        return NULL;
    }
    
    // Compress images and determine exact size needed for everything
    for (int i=0; i < NUM_IMAGES; i++) {
        
        INT(tf, IMAGE_HEADER_width(i))   = (baseimg->width/pow(2.0, i)); 
        INT(tf, IMAGE_HEADER_height(i))  = (baseimg->height/pow(2.0, i));
        
        //printf("width %d height %d\n", INT(tf, IMAGE_HEADER_width(i)), INT(tf, IMAGE_HEADER_height(i)));
        
        INT(tf, IMAGE_HEADER_depth(i))       = baseimg->depth;
        INT(tf, IMAGE_HEADER_nChannels(i))   = baseimg->nChannels;
        
        cvimg = cvCreateImage(cvSize(INT(tf, IMAGE_HEADER_width(i)), INT(tf, IMAGE_HEADER_height(i))), baseimg->depth, baseimg->nChannels);
        cvResize(baseimg, cvimg, CV_INTER_AREA);
        
        INT(tf, IMAGE_HEADER_image_size(i))  = IMG_SIZE(cvimg, 1);
        
        INT(tf, IMAGE_HEADER_compressed_size(i)) = compress_image(cvimg, &(tmpptrs[i])); // create compressed image in tmpptr
        printf("Compressed Size %d\n", INT(tf, IMAGE_HEADER_compressed_size(i)));
        size += INT(tf, IMAGE_HEADER_compressed_size(i));
        
        cvReleaseImage(&cvimg);
    }
    
    //cout << "Creating Continuous memory" << endl;
    
    // Allocate the new continuous memory object
    tf = (unsigned char *)realloc( tf, TILE_HEADER_SIZE + size );
    
    INT(tf, MAIN_HEADER_targetcount) = 0;
    INT(tf, MAIN_HEADER_imgid)       = image_number(datfilename);
    DOUBLE(tf, MAIN_HEADER_lat)      = lat;
    DOUBLE(tf, MAIN_HEADER_lon)      = lon;
    DOUBLE(tf, MAIN_HEADER_heading)  = heading;
    DOUBLE(tf, MAIN_HEADER_alt)      = alt; 
    DOUBLE(tf, MAIN_HEADER_roll)     = roll;
    DOUBLE(tf, MAIN_HEADER_pitch)    = pitch;
    
    
    // Save the size of all included compressed images
    INT(tf, MAIN_HEADER_total_size) = size;
    
    // Copy everything into the new continuous memory object
    for (int i=0; i < NUM_IMAGES; i++) {
        
        // Calculate the number of bytes to the beginning of compressed image data
        int offset = IMAGE_HEADER_offset(i);
        INT(tf, offset) = 0;
        for (int j=i; j-- > 0; ) {
            INT(tf, offset) += INT(tf, IMAGE_HEADER_compressed_size(j));
        }
        
        
        
        // Copy compressed image into the main TileFile structure
        //   The stange casting is to make this pointer arithemtic add bytes not pointer type sizes. 
        memcpy(tf + TILE_HEADER_SIZE + INT(tf, IMAGE_HEADER_offset(i)),
               tmpptrs[i],
               INT(tf, IMAGE_HEADER_compressed_size(i) ) );
        
        // Free the temporary image data
        free(tmpptrs[i]);
    }
    
    return tf;
    
Bail:
    perror("Error");
    return NULL;
}

void Release_Tile_File(TileFile tf)
{
    free(tf);
}


unsigned char *Add_Targets( TileFile tf, unsigned char *targets_mem, int num )
{
    // New size of Tile Data not actual memory size see realloc
    int nsize = INT(tf, MAIN_HEADER_total_size) + (IMAGE_TARGETS_HEADER * num);
    
    // Save the tf pointer incase of realloc failure
    TileFile prev = tf;
    
    // + TILE_HEADER_SIZE to get full Tile Size
    tf = (unsigned char *) realloc(tf, nsize + TILE_HEADER_SIZE);
    
    if (!tf) goto Bail;
    
    memcpy( (void *) (tf + TILE_HEADER_SIZE + INT(tf, MAIN_HEADER_total_size)),
            (void *) targets_mem, 
            IMAGE_TARGETS_HEADER * num);
    
    INT(tf, MAIN_HEADER_total_size)  = nsize; 
    INT(tf, MAIN_HEADER_targetcount) = num;
    
    
    return tf;
    
Bail:
    fprintf(stderr, "Error adding target failed\n");
    return prev;
}


//------------------------------------------------------------------------------------
#ifdef VIEWER
#include "matrix.h"
#include <sys/stat.h>

Thread_Pool *Tile_Thread_Pool;

void TileObj_Init()
{
    // initialize the thread pool for tile image reading
    Tile_Thread_Pool = new Thread_Pool(10);
}


// TILE locking functions
void TileObj::lock()
{
    pthread_mutex_lock(&tile_mtx);
}

int TileObj::trylock()
{
    return pthread_mutex_trylock(&tile_mtx); 
}

void TileObj::unlock()
{
    pthread_mutex_unlock(&tile_mtx);
}

//............................................................
TileObj::TileObj(TileFile tf)
{
    //Setup the headers static memory
    INIT_TILE(tiledata);
    
    //Generate the unique name for the file
    sprintf(filename, "lat%flon%f", DOUBLE(tf, MAIN_HEADER_lat), DOUBLE(tf, MAIN_HEADER_lon));
    
    // If the file already exists keep adding numbers to the end
    // till we find a free name 
    int count = 0;
    while (vfs_fexist(filename)) {
        sprintf(filename, "lat%flon%f%d", DOUBLE(tf, MAIN_HEADER_lat), DOUBLE(tf, MAIN_HEADER_lon), count);
        // Check for path length over run
        // should only happen if the plane sits running for way to long
        if (strlen(filename) > MAX_PATH_LENGTH) 
            fprintf(stderr, "WARNING the filename has grown to long to handle move the plane god damn it\n %s", filename);
        count++;
    }
    
    // Make space to save target coordinates
    target_array = (unsigned char *) malloc( IMAGE_TARGETS_HEADER * INT(tf, MAIN_HEADER_targetcount) );
    
    if ( !target_array ) 
        fprintf(stderr, "Error allocating space for targets\n");
    else
        memcpy( (void *)target_array, (void *)IMAGE_TARGET(tf, 0), IMAGE_TARGETS_HEADER * INT(tf, MAIN_HEADER_targetcount) );
    
    
    // Write the TileFile to disk returning the Unique Name
    VFS_WriteTileFile(filename, tf);
    
    // Copy the header info into tiledata for later 
    // inspection
    memcpy( tiledata, tf, TILE_HEADER_SIZE );
    
    
    // Set the geoposition of the tile
    geo_position.lat = DOUBLE(tf, MAIN_HEADER_lat);
    geo_position.lon = DOUBLE(tf, MAIN_HEADER_lon);
    geo_position.alt = DOUBLE(tf, MAIN_HEADER_alt);
    
    // Release the whole tile file now that its written to disk
    Release_Tile_File(tf);
    
    // This value will be set later by ViewRec when it knows
    GLpos = NULL;
    
    // Generate a new texture obj and pass relevant data to it
    texobj = new Texture(filename, tiledata, target_array);
    
    // The Tile currently holds no data
    state = TILE_INACTIVE; 
    
    // Initalize tile mutex
    pthread_mutex_init(&tile_mtx, NULL);
    
}

TileObj::~TileObj()
{
    delete GLpos;
    delete texobj;
}


//............................................................

GeoCoord *TileObj::Get_GeoPosition()
{
    return &geo_position;
}


double TileObj::Get_Altitude()
{
    return geo_position.alt;
}

void TileObj::Set_GLpos(GeoCoord *origin)
{
    lock();
    //...........................
    
    GLpos = new Vec(0, 0, 0);
    geo_position.Geo_Meters(GLpos, origin);
    
    //...........................
    unlock();
    
}

int TileObj::GL_Load()
{
    int res;
    lock();
    //..........................
    
    // Make sure tile is not being marked for unloading
    if (state == TILE_START_UNLOAD)
        res = 1;
    
    // Load GL image and set state as now active
    res = texobj->GL_Load();
    state = TILE_ACTIVE_IMAGE;
    
    //..........................
    unlock();
    
    return res;
}

//............................................................
int TileObj::Activate()
{
    int res;
    
    lock();
    //..........................
    
    // If the tile is already active in some way
    // do not reset that state
    if (state == TILE_INACTIVE ) {
        state = TILE_ACTIVE_DISK_LOADING;

        res = 1;
    } else {
        res = 0;
    }
    
    //..........................
    unlock();
    
    return res;
}

int TileObj::Deactivate()
{
    int res;
    
    lock();
    //..........................
    
    
    // Don't unload during a disk access 
    // Don't restart unload if already UNLOADING
    if ( (state == TILE_ACTIVE_DISK_LOADING) || (state == TILE_START_UNLOAD) || (state == TILE_INACTIVE) ) {
        res = 0;
        goto exit;
    }
    
    state = TILE_START_UNLOAD;
    texobj->MemImgRelease();
    
    //..........................
exit:
    unlock();
    return res; 
}




TILE_GL_SIGNAL TileObj::GL_run()
{

    if (trylock()) return TILE_GL_DEFAULT;
    //.........................
    
    switch (state) {
            
        case TILE_INACTIVE:
            break;
            
        case TILE_ACTIVE_DISK_LOADING:
            Draw();
            break;
            
        case TILE_ACTIVE_DISK_LOADING_WAIT:
            //Draw();
            break;
            
        case TILE_ACTIVE_GL_LOAD:
            Draw();
            
            //.........................
            unlock();
            return TILE_GL_LOAD; // Signal the tile requires a GL load operation
            break;
            
        case TILE_ACTIVE_IMAGE:
            Draw();
            break;
        
        case TILE_START_UNLOAD:
            texobj->GLImgRelease(); // Release the GL image 
            state = TILE_INACTIVE;
            
            break;
            
        default:
            fprintf(stderr, "GL The Tile is in an indeterminate state\n");
            break;
    };
    
    
    //.........................
    unlock();

    return TILE_GL_DEFAULT;
}

void Image_Load_func(void *data)
{
    TileObj *tile = (TileObj *) data;
    tile->thrd_image_load();
}

void TileObj::thrd_image_load()
{
    lock();
    //.......................................
    texobj->Disk_Load(image_load_level);
    state = TILE_ACTIVE_GL_LOAD;
    //.......................................
    unlock();
}

TILE_STD_SIGNAL TileObj::Standard_run(int image_level)
{
    if (trylock()) return TILE_STD_DEFAULT;
    //....................................
    
    // Check if the image level has changed and the tile is not inactive
    if ( (texobj->Current_Level() != image_level) && (state != TILE_INACTIVE) ) 
        state = TILE_ACTIVE_DISK_LOADING;
    
    switch (state) {
            
        case TILE_INACTIVE:
            //.........................
            unlock();
            return TILE_STD_INACTIVE;
            break;
        
        
        case TILE_ACTIVE_DISK_LOADING:
            // save the image level and add the work to the tile
            // thread pool
            image_load_level = image_level;
            state = TILE_ACTIVE_DISK_LOADING_WAIT;
            Tile_Thread_Pool->add(Image_Load_func, (void*)this);
            break;
        
        case TILE_ACTIVE_DISK_LOADING_WAIT:
            // Nothing happens here because its 
            // happening in a seperate thread
            break;
            
            
        // The following do nothing because they are handled by the GL_run method
        case TILE_ACTIVE_GL_LOAD:
            break;
        case TILE_ACTIVE_IMAGE:
            break;
        case TILE_START_UNLOAD:
            break;
            
        // Error handling
        default:
            fprintf(stderr, "STD The Tile is in an indeterminate state\n");
            break;
    };
    //.............................
    unlock();
    
    return TILE_STD_DEFAULT;
}

//............................................................
// Drawing functions

#define RECTDRAW(tf, index) glVertex3d( DOUBLE(tf, TILE_POINT_OFF(index,0) ), \
                                        DOUBLE(tf, TILE_POINT_OFF(index,1) ), \
                                        0  )       


void TileObj::tex_project()
{
    float modelview[16];
    float invmodelview[16];
    
    
    glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    
    glMatrixMode(GL_TEXTURE);

    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.0f);
    glScalef(0.5, -0.5, 1.0);
    gluPerspective(CAMERA_VERT_ANGLE, ASPECT_RATIO, 1, DOUBLE(tiledata, MAIN_HEADER_alt));
    
    
    gluLookAt( 0,//GLpos->x, 
               0,//GLpos->y, 
               geo_position.alt,             // Eye position
              
               DOUBLE(tiledata, TILE_POINT_OFF(4,0)), // x - center
               DOUBLE(tiledata, TILE_POINT_OFF(4,1)), // y - center
               0,                            // z - center
              
               0, 1, 0                        // Up vector
             );
    
    glTranslatef(0, 0, -1);
    My_Matrix_Inverse(modelview, 16, 4, invmodelview);
    glMultMatrixf(invmodelview);
    glMatrixMode(GL_MODELVIEW);
    
}

void TileObj::draw_texture_box()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    tex_project();
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    glBindTexture(GL_TEXTURE_2D, texobj->GLtexName);
    
    glBegin(GL_QUADS);
    
    RECTDRAW(tiledata, 0);
    RECTDRAW(tiledata, 1);
    RECTDRAW(tiledata, 2);
    RECTDRAW(tiledata, 3);
    
    
    glEnd();
    
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glDisable(GL_TEXTURE_2D);
    
    
}

void TileObj::draw_empty_box()
{
    glColor3f(0.3, 0.3, 0.3);
    
    glBegin(GL_QUADS);
    
    RECTDRAW(tiledata, 0);
    RECTDRAW(tiledata, 1);
    RECTDRAW(tiledata, 2);
    RECTDRAW(tiledata, 3);
    
    glEnd();
}


int TileObj::Draw()
{
    
    // Draw the Tile either with or without the image
    glPushMatrix();

    glTranslatef(GLpos->x, GLpos->y, 0);
    glRotatef( DOUBLE(tiledata, MAIN_HEADER_heading), 0, 0, -1);
    
    
    if (state == TILE_ACTIVE_IMAGE) {
        draw_texture_box();
        glPopMatrix();
        return 1;
    }
    
    draw_empty_box();
    glPopMatrix();
    

    return 0;
}









/*
 // Test cases for the Access methods 
 
int main(int argc, char *argv[])
{
    
    STATIC_TILE(test_tile);
    INIT_TILE(test_tile);
    
    int size = 101010101;
    double pi = 3.14159265;
    
    DOUBLE(test_tile, MAIN_HEADER_lat) = pi;
    INT(test_tile, MAIN_HEADER_total_size)  = size;
    
    printf("Size %d %d %.8f\n", size, INT(test_tile, MAIN_HEADER_total_size), DOUBLE(test_tile, MAIN_HEADER_lat));
    
    STATIC_TILE(main);
    
    INT(main, MAIN_HEADER_total_size) = 111111;
    DOUBLE(main, MAIN_HEADER_lat)     = 3.14159265;
    DOUBLE(main, MAIN_HEADER_lon)     = 3.14159265;
    DOUBLE(main, MAIN_HEADER_alt)     = 3.14159265;
    DOUBLE(main, MAIN_HEADER_roll)    = 3.14159265;
    DOUBLE(main, MAIN_HEADER_pitch)   = 3.14159265;
    DOUBLE(main, MAIN_HEADER_heading) = 3.14159265;
    
    
    
    for (int i = 0; i < 4; i++){
        DOUBLE(main, TILE_POINT_OFF(i, 0)) = 3.14159265;
        DOUBLE(main, TILE_POINT_OFF(i, 1)) = 3.14159265;    
        DOUBLE(main, TILE_POINT_OFF(i, 2)) = 3.14159265;
    }
    
    for (int i = 0; i < 4; i++) {
        INT(main, IMAGE_HEADER_image_size(i)) = 11111;
        INT(main, IMAGE_HEADER_compressed_size(i)) = 0;
        INT(main, IMAGE_HEADER_width(i)) = 11111;
        INT(main, IMAGE_HEADER_height(i)) = 0;
        INT(main, IMAGE_HEADER_depth(i)) = 11111;
        INT(main, IMAGE_HEADER_nChannels(i)) = 0;
        INT(main, IMAGE_HEADER_offset(i)) = 11111;
        
    }
    
    
    
    printf("Total Size %d\nlat %f\nlon %f\nalt %f\nroll %f\npitch %f\nheading %f\n\n",
           INT(main, MAIN_HEADER_total_size),
           DOUBLE(main, MAIN_HEADER_lat),
           DOUBLE(main, MAIN_HEADER_lon),
           DOUBLE(main, MAIN_HEADER_alt),
           DOUBLE(main, MAIN_HEADER_roll),
           DOUBLE(main, MAIN_HEADER_pitch),
           DOUBLE(main, MAIN_HEADER_heading) );
    
    
    for (int i = 0; i < 4; i++){
        printf("x %f\ny %f\nz %f\n\n", 
               DOUBLE(main, TILE_POINT_OFF(i, 0)),
               DOUBLE(main, TILE_POINT_OFF(i, 1)),
               DOUBLE(main, TILE_POINT_OFF(i, 2)) );
    }
    
    
    for (int i = 0; i < NUM_IMAGES; i++) {
        
        printf("image_size %d\ncompressed_size %d\nwidth %d\nheight %d\ndepth %d\nnChannels %d\noffset %d\n\n", 
               INT(main, IMAGE_HEADER_image_size(0)),
               INT(main, IMAGE_HEADER_compressed_size(0)),
               INT(main, IMAGE_HEADER_width(0)),
               INT(main, IMAGE_HEADER_height(0)),
               INT(main, IMAGE_HEADER_depth(0)),
               INT(main, IMAGE_HEADER_nChannels(0)),
               INT(main, IMAGE_HEADER_offset(0)) );
    }
    
    
    printf("DOUBLE_SIZE %d\n", sizeof(DOUBLE_SIZE));
     printf("int %d\n", sizeof(int));
     printf("long int %d\n", sizeof(long int));
     printf("unsigned char * %d\n", sizeof(unsigned char *));
     printf("int * %d\n", sizeof(int *));
     printf("USIGNED CHAR %d\n", sizeof(unsigned char));
    
    return 0;
    
}
*/


#endif


