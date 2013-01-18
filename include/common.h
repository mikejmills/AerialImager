/*
 *  common.h
 *  
 *
 *  Created by mike on 07/02/07.
 *  Common things needed
 *
 */
#ifndef _COMMON_
#define _COMMON_

using namespace std;

#ifdef __APPLE__ 
    #include <GLUT/glut.h> 
    #include <OpenGL/gl.h>
#else 
   #include <GL/glut.h> 
   #include <GL/gl.h>
#endif
#include <assert.h>
#include <math.h>
#include <iostream>

#define PI 3.14159265

// Global Type of the TileFile
typedef unsigned char*  TileFile;

// Camera constants 
//#define CAMERA_ANGLE      33     // Calculated using the image data focal length
#define CAMERA_ANGLE      44     
#define CAMERA_VERT_ANGLE 27 // 
#define CAMERA_ANGLE_HALF CAMERA_ANGLE/2
#define ASPECT_RATIO      1.5


// Dat file extension 
#define DATA_FILE_EXTENSION   "dat"
#define IMAGE_FILE_EXTENSION  "jpg"
#define EXTEN_SIZE            3
#define MAX_PATH_LENGTH       100




// Screen dimensions
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

// Base Altitude
extern double Base_Altitude;

//reduce resolution by the number
#define SCALE_FACTOR 4

//Convert radians to degrees
#define R2D(r) ( (r) * 57.2957795 )

//Convert degrees to radians
#define D2R(d) ( (d) * 0.0174532925 )



// Terrains boundries 
#define TERRAIN_X_MIN -1000
#define TERRAIN_X_MAX  1000
#define TERRAIN_Z_MIN -1000
#define TERRAIN_Z_MAX  1000

#ifdef VIEWER
//
// axis for debugging
void axis(); 

//
//Terrain 
void terrain();
#endif
//
// Given the size calculates the next power of two
double Next_Power_2(double size);

#endif