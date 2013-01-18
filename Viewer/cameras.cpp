#include "cameras.h"
#include <stdlib.h>
#include <iostream>



//--------------------------------------------------------------------------
Camera::Camera(int t, double *params)
{
    
    //Setup camera depending on type
    switch (t) {
        case CAM_PERSPECTIVE:
            angle  = params[0];
            aspect = params[1];
            near   = params[2];
            far    = params[3];
            camtype = t;
           
            break;
            
        case CAM_ORTHO:
            xmin   = params[0];
            xmax   = params[1];
            ymin   = params[2];
            ymax   = params[3];
            near   = params[4];
            far    = params[5];
            
            camtype   = CAM_ORTHO;
            break;
    }
    


}


void Camera::draw()
{
    Vec look;
    
  
    look.Set(&dir);
    look.Add(&look, &pos);
    
    switch (camtype) {
        case CAM_PERSPECTIVE:
        
            //Setup projection
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(angle, aspect, near, far);
            
            //Setup where the cam looks
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        
            gluLookAt( pos.x, pos.y, pos.z,
                       look.x, look.y, look.z,
                       up.x,  up.y,  up.z );
            
            break;
            
        case CAM_ORTHO:
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glPushMatrix();
                glOrtho(xmin, 
                         xmax, 
                         ymin, 
                         ymax, 
                         near,
                         far);
                         
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                
            break;
    }
                      
}