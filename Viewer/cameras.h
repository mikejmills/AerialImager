/*
 *  cameras.h
 *  
 *
 *  Created by mike on 14/02/07.
 *  Cameras are divided into two types perspective and orthogonal. 
 *  This file handles both through an interface with accepts the type
 *  and some parameters through an array.
 *  Perspective Array: angle, aspect, near, far
 *  Orthographic Array: xmin, xmax, ymin, ymax, near and far.
 *  
 *
 */

#ifndef CAMERA
#define CAMERA

#include "../include/common.h"
#include "../include/Vector.h"
#include "Object.h"

typedef enum {
    CAM_PERSPECTIVE,
    CAM_ORTHO
} CAM_TYPE;

//Camera type 
//Near and far are used in both camera types.
class Camera: public object
    {
    public:
        int camtype;
        
        // Perspective camera sttings        
        double angle, aspect, near, far;
        
        //Orthogonal Projection settings        
        double xmin, xmax, ymin, ymax;
        
        Camera(int camtype, double *params);
        void set_far(double f);
        void draw();
    };



#endif

