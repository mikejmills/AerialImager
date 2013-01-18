/* Doc- #################################
#######################################*/

#ifndef VIEWREC
#define VIEWREC

#include "GeoCoord.h"
#include "../include/TileObj.h"
#include "gl.h"
#include <pthread.h>
#include "cameras.h"

class ViewRec
{
private:
    GeoCoord origin;  //Geographic origins
    Vec      cpos;    //GL position z is altitude
    
    double view_radius;
    Camera *cam;
    
    pthread_mutex_t vr_mtx; // Protection for structure

    int disp_count;
    
public:
    
    ViewRec(GeoCoord *org);
    ~ViewRec();
    
    // Check if tile is displayable
    int InDisplayable(Vec *pos);
    
    // Return the View Radius based on the Altitude stored in cpos.z
    double View_Radius();
    
    void Move(int dx, int dy);
    void Zoom(int dz);
    
    // Move Camera to pos x,y 
    void GLMove(double x, double y);
    
    // Run the cameras draw routine
    void DrawCam();
    
    // set how many tiles are to be displayed
    void Display_Count(int count);
    
    // Return the index of tile image
    int ImageSize();
    
    // Get the origin coordinates
    GeoCoord *Get_Origin();
    
};

//
// Global View Rectangle Pointer
extern ViewRec *GVRec;

//
// Compare two tiles to sort 
bool TileCompare(TileObj *t1, TileObj *t2);

//
// Initalize the global View Rectangle
void ViewRec_Init_Global(GeoCoord *org);

#endif

