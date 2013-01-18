/*
 *  Vector.h
 *  
 *
 *  Created by mike on 07/02/07.
 *  Vector Data type, with accessor macros
 *
 */
 
#ifndef VECTOR
#define VECTOR

#include "../include/common.h"
#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>

/*
    Vector Data type
*/
class Vec {
private:
    double d;
    
public:
    double x, y, z;
    
    Vec(double vx, double vy, double vz);
    Vec();
    ~Vec();
    
    
    // Persistent scalar multi
    void Scl_p(double scl);
    
    // non Persistent scalar multi
    void Scl_np(Vec *res, double scl);
    
    // Add this vector and another into a res vector you provide
    void Add(Vec *res, Vec *v);
    
    // Return vector Dot Product
    double Dot_Product(Vec *v);
    
    // Return Magnitude of Vector
    double Mag();
    
    // Rotate this vector x,y,z angles 
    void Rotate(Vec *res, double x_angle, double y_angle, double z_angle);
    
    // Set this vectors values with ptr
    void Set(Vec *ptr);
    
    // Set this vectors values with x,y,z
    void Set(double xv, double yv, double zv);
    
    // Make res a unit vector of this vector
    void Unit(Vec *res);
    
    // Return distance between vectors
    double Distance(Vec *pos);
    void Print();
    
};

void VecTests();

#endif
