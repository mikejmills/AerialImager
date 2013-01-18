
#include "Measurement_Function.h"


struct line {
    double t;
    CvMat p0, *p1;
};
float line_p0[] = { 0, 0, 0, 0 };



struct plane {
    CvMat p, norm;
}; 
float plane_norm[]  = {0, 0, 1, 0};


// 
// Rotate around the x axis
void xRotate(CvMat *vec, float angle, CvMat *res)
{
    angle = D2R(angle);
    float vals[] = { 1, 0, 0, 0,
                     0, cos(angle), sin(angle),  0,
                     0, -sin(angle), cos(angle), 0,
                     0, 0, 0, 1 };
    
    CvMat mat;
    cvInitMatHeader( &mat, 4, 4, CV_32FC1, &vals );
    cvMatMul(&mat, vec, res);
}

// 
// Rotate around the y axis
void yRotate(CvMat *vec, float angle, CvMat *res)
{
    angle = D2R(angle);
    float vals[] = { cos(angle), 0, -sin(angle), 0,
                     0, 1, 0, 0,
                     sin(angle), 0, cos(angle), 0,
                     0, 0, 0, 1};    
    CvMat mat;
    cvInitMatHeader( &mat, 4, 4, CV_32FC1, &vals );
    cvMatMul(&mat, vec, res);
}

// 
// Rotate around the z axis
void zRotate(CvMat *vec, float angle, CvMat *res)
{
    angle = D2R(angle);
    float vals[] = { cos(angle), sin(angle), 0, 0,
                     -sin(angle), cos(angle), 0, 0, 
                     0, 0, 1, 0,
                     0, 0, 0, 1 };
    
    CvMat mat;
    cvInitMatHeader( &mat, 4, 4, CV_32FC1, &vals );
    cvMatMul(&mat, vec, res);
}


//
// Find the sensor coordinates relative to the camera origin in meters
void pixelTometer(IplImage *img, CvMat *pix)
{
    CvMat conv;
    
    // Conversion matrix to locate the physical sensors position
    // relative to the focal point the values are derived by
    // finding the sensor size in meters and dividing by the number
    // of pixels
    float vals[] = { 0.0000122, 0, 0, -(img->width/2)  * 0.0000122, \
                     0, 0.0000122, 0, -(img->height/2) * 0.0000122, \
                     0, 0, 1, 0,                                    \
                     0, 0, 0, 1  };
        
    cvInitMatHeader( &conv, 4, 4, CV_32FC1, vals );
    cvMatMul(&conv, pix, pix);
}


int plane_intersection(struct plane *pl, struct line *l, CvMat *res)
{
    float d = cvDotProduct(&(pl->p), &(pl->norm));
    float num = d - (FLOAT_MAT_ELEM(&(pl->norm), 0, 0) * FLOAT_MAT_ELEM(&(l->p0), 0, 0)) -
                    (FLOAT_MAT_ELEM(&(pl->norm), 1, 0) * FLOAT_MAT_ELEM(&(l->p0), 1, 0)) - 
                    (FLOAT_MAT_ELEM(&(pl->norm), 2, 0) * FLOAT_MAT_ELEM(&(l->p0), 2, 0));

    float denom = FLOAT_MAT_ELEM(&(pl->norm), 0, 0) * ( FLOAT_MAT_ELEM(l->p1, 0, 0) - FLOAT_MAT_ELEM( &(l->p0), 0, 0) ) +
                  FLOAT_MAT_ELEM(&(pl->norm), 1, 0) * ( FLOAT_MAT_ELEM(l->p1, 1, 0) - FLOAT_MAT_ELEM( &(l->p0), 1, 0) ) +
                  FLOAT_MAT_ELEM(&(pl->norm), 2, 0) * ( FLOAT_MAT_ELEM(l->p1, 2, 0) - FLOAT_MAT_ELEM( &(l->p0), 2, 0) );
    
    if (denom == 0)
        return 0;

    float t = num/denom;
    
    cvSub(l->p1, &(l->p0), res);
    
    myMulS(res, t, res);

    return 1;
}


// 
// Measurement inverse. Given a measurement ( {x,y} pixel coordinates ) and pose calculate the 
// world coordinates of the feature
int    Measurement_Inverse(IplImage *img, int px, int py, float focal_length,
                            float pitch, float roll, float alt,
                            CvMat *feature_point_res)
{
    struct line  l;
    struct plane pl;
    float plane_point[] = { 0, 0,  alt, 1 };
    float tmps[] = { px, py, focal_length, 1 };
    float def_bear[] = { 0, 1, 0, 0 };
    CvMat pix, def_bear_mat;
    
    cvInitMatHeader(&def_bear_mat, 4, 1, CV_32FC1, &def_bear);
    cvInitMatHeader(&pix, 4, 1, CV_32FC1, &tmps); 
    
    // Init line 
    cvInitMatHeader(&(l.p0), 4, 1, CV_32FC1, line_p0);
    
    // Init plane
    cvInitMatHeader(&(pl.p), 4, 1, CV_32FC1, plane_point);
    cvInitMatHeader(&(pl.norm), 4, 1, CV_32FC1, plane_norm);
    
    
    // Convert the pixels to meters units
    pixelTometer(img, &pix);
    
    // Rotate with pitch and roll
    yRotate(&pix, roll, &pix);
    xRotate(&pix, pitch, &pix);
    
    l.p1 = &pix;
    
    // Check for no solution to plane intersection
    if ( !plane_intersection(&pl, &l, &pix) )
        return -1;
    
    M_INV_X(feature_point_res) = FLOAT_MAT_ELEM(&pix, 0, 0);
    M_INV_Y(feature_point_res) = FLOAT_MAT_ELEM(&pix, 1, 0);
    
    return 1;
}

