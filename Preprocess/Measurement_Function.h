/* Doc- #################################
#######################################*/

#ifndef MEASUREMENT_FUNCTION
#define MEASUREMENT_FUNCTION

#include <cxcore.h>
#include <cv.h>
#include "../include/common.h"

#define M_SIZE 3
#define M_INV_SIZE 3

#define M_INV_X(point)   FLOAT_MAT_ELEM(point, 0, 0)
#define M_INV_Y(point)   FLOAT_MAT_ELEM(point, 1, 0)
#define M_INV_C(point)   FLOAT_MAT_ELEM(point, 2, 0)

#define M_RANGE(point)   FLOAT_MAT_ELEM(point, 0, 0)
#define M_BEARING(point) FLOAT_MAT_ELEM(point, 1, 0)
#define M_CORESP(point)  FLOAT_MAT_ELEM(point, 2, 0)




//
// This computes world coordinates (x, y) of a point (px, py) in pixels with respect to
// the pose and the pitch and roll of the camera
int    Measurement_Inverse(IplImage *img, int px, int py, float focal_length,
                           float pitch, float roll, float alt,
                           CvMat *feature_point_res);


//
// This function computes the range and bearing to a point from a 
// given pose.
void Measurement(CvMat *pose, CvMat *feature_point, CvMat *res);



//
// Sets the jacobian matrix wrt pose position at feature and pose coord
void Measurement_Jacobian_pose(CvMat *pose, CvMat *feature_coord, CvMat *jacobian);



//
// Sets the jacobian matrix wrt feature position at feature and pose coord
void Measurement_Jacobian_feature(CvMat *pose, CvMat *feature_coord, CvMat *jacobian);


//
// Multiply the input matrix by the Measurement covariance
// INPUT * COV => res
void Measurement_Covariance_Mul(CvMat *input, CvMat *res, int inv = 0);

//
// Add the Covariance matrix to input 
void Measurement_Covariance_Add(CvMat *input, CvMat *res, int inv = 0);


#endif


