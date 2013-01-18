
#include "Feature.h"

#define STATE_SIZE 2
 
Feature::Feature(Ipoint ip, double x, double y)
{
    this->ip = ip;

    px = x;
    py = y;
    
    
    
    kalman = NULL;
    Create_KalmanFilter();
}

void Feature::Create_KalmanFilter()
{
    printf("Create Kalman\n");
    // Create Kalman filter
    kalman = cvCreateKalman(STATE_SIZE, 2, 0);
    
    // Set initial position
    kalman->state_post->data.fl[0] = (float)px;
    kalman->state_post->data.fl[1] = (float)py;
    
    cvSetIdentity( kalman->measurement_matrix,	  cvRealScalar(1) ); 
    cvSetIdentity( kalman->process_noise_cov,	  cvRealScalar(1e-5) ); 
    cvSetIdentity( kalman->measurement_noise_cov, cvRealScalar(1e-1) );
    cvSetIdentity( kalman->error_cov_post,        cvRealScalar(1));
    
    F[0] = 1;
    F[1] = 0;
    F[2] = 0;
    F[3] = 1;
    memcpy( kalman->transition_matrix->data.fl, F, sizeof(F));
    
    measurement = cvCreateMat(2, 1, CV_32FC1);
    printf("Done\n");
}

const CvMat *Feature::Update_Filter(float x, float y)
{
    const CvMat *p;
    printf("Kalman Predict\n");
    p = cvKalmanPredict( kalman, 0 );
    
    measurement->data.fl[0] = x;
    measurement->data.fl[1] = y;
    
    printf("Kalman Correct\n");
    cvKalmanCorrect( kalman, measurement );
    
    px = kalman->state_post->data.fl[0];
    py = kalman->state_post->data.fl[1];
    printf("Done Updates\n");
    return p;
}

void Feature::Add_Tile(TileObj *t)
{
    TileList.push_back(t);
    
}

void Feature::Remove_Last_Tile()
{
    TileList.pop_back();
}