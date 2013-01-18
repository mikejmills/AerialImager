
#include "ViewRec.h"

#define ALT (cam->pos.z)

#define MAX_FULL_IMAGES 5
#define MAX_MEDM_IMAGES 15

#define MAX_ALTITUDE 10000

#define ZOOM_DZ 1.3

// Global View Rectangle 
ViewRec *GVRec = NULL;

void ViewRec_Init_Global(GeoCoord *org)
{
    GVRec = new ViewRec(org);
}


bool TileCompare(TileObj *t1, TileObj *t2)
{
    // Sort the tiles by altitude in decsending order
    if ( t1->Get_Altitude() < t2->Get_Altitude() ) return false;
    return true;
}


ViewRec::ViewRec(GeoCoord *org)
{
    printf("Initalize View Rectangle alt %f\n", org->alt);
    
    // Set origin and current position
    origin.Set(org);
    
    // Perspective Camera
    double camparams[4] = { CAMERA_VERT_ANGLE, 1, org->alt - 1, org->alt + 1 };

    cam = new Camera(CAM_PERSPECTIVE, camparams);

    cam->set_pos(0,0, org->alt);
    cam->set_dir(0,0, -1);
    printf("View Rect ALT %f\n", org->alt);
    pthread_mutex_init(&vr_mtx, NULL);
}

ViewRec::~ViewRec()
{
    delete cam;
}


double ViewRec::View_Radius()
{
    double radius = tan(D2R(CAMERA_ANGLE_HALF)) * ALT;
    
    if ( radius < 50 ) {
        return 50;
    }
    return radius;
}


int ViewRec::InDisplayable(Vec *pos)
{
    pthread_mutex_lock( &vr_mtx );
    //...................................
   
    if ( pos->Distance(&cpos) < View_Radius() ) {

    //...................................
        pthread_mutex_unlock( &vr_mtx );
        
        return 1;
    }
    //...................................    
    pthread_mutex_unlock( &vr_mtx );
    
    return 0;
}


void ViewRec::Move(int dx, int dy)
{
    Vec pos;
    pthread_mutex_lock( &vr_mtx );
    //...................................
    
    GL_MousetoGL(dx, dy, &pos);
    
    cam->pos.x = cpos.x = pos.x;
    cam->pos.y = cpos.y = pos.y;
    
    //...................................    
    pthread_mutex_unlock( &vr_mtx );
}


void ViewRec::GLMove(double x, double y)
{
    pthread_mutex_lock( &vr_mtx );
    //...................................
    
    cam->pos.x = cpos.x = x;
    cam->pos.y = cpos.y = y;
    
    //...................................    
    pthread_mutex_unlock( &vr_mtx );
}

void ViewRec::Zoom(int dz)
{
    Vec *p;
    
    pthread_mutex_lock( &vr_mtx );
    //...................................
    
    p = cam->get_pos();
    
    // Max altitude
    if ( (dz > 0) && (p->z > MAX_ALTITUDE)) goto UNLOCK;
    
    if (dz > 0)
        p->z *= ZOOM_DZ; 
    else 
        p->z /= ZOOM_DZ;
    
    cam->near = p->z + 1;
    cam->far  = p->z - 1;

    //...................................   
    
UNLOCK:
    pthread_mutex_unlock( &vr_mtx );
    
}


void ViewRec::DrawCam()
{
    pthread_mutex_lock( &vr_mtx );
    //...................................
    
    GL_Draw_Camera(cam);
    disp_count = 0;
    
    //...................................    
    pthread_mutex_unlock( &vr_mtx );
}


void ViewRec::Display_Count(int count)
{
    pthread_mutex_lock( &vr_mtx );
    //...................................
    
    disp_count = count;
    
    //...................................    
    pthread_mutex_unlock( &vr_mtx );
}


int ViewRec::ImageSize()
{
    
    int index = 0; // Default Max Size
    
    pthread_mutex_lock( &vr_mtx );
    //...................................
    
    if (disp_count > MAX_FULL_IMAGES) index = 1;
    
    if (disp_count > MAX_MEDM_IMAGES) index = 2;
    
    //...................................    
    pthread_mutex_unlock( &vr_mtx );
    
    if (cam->get_pos()->z < 400) index = 0;
    
    return index;
    
}


GeoCoord *ViewRec::Get_Origin()
{
    return &origin;
}
