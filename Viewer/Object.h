/*
 *  Object.h
 *  
 *  Created by mike on 07/02/07.
 *  Basic properties of any entity on the screen.
 *  Such as positioning, forward vector and Up vector
 *
 */

#ifndef OBJECT
#define OBJECT

#include "../include/Vector.h"
#include "../include/common.h"

/*
 Object type
*/
class object 
    {
    public:
        Vec init_orient, pos, dir, up;
        
        object();
        void set_pos(Vec *p);
        void set_pos(double x, double y, double z);
        
        Vec *get_pos();
        
        void set_dir(Vec *d);
        void set_dir(double x, double y, double z);
        
        Vec *get_dir();
        void move(float Vecamount);
    };


#endif

