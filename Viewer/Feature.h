/* Doc- #################################
#######################################*/

#ifndef FEATURE
#define FEATURE

#include <opencv/cv.h>
#include "../OpenSURF/ipoint.h"
#include <vector.h>
class TileObj;

class Feature
{
private:
  
    CvMat     *measurement; 
    float F[4]; // kalman transition matrix
public:
    vector<TileObj *> TileList; // List of tiles which share this feature
    float     px, py;
    CvKalman  *kalman;      // Kalman filter struct
    Ipoint    ip;
    
    Feature(Ipoint ip, double x, double y);
    void Create_KalmanFilter();
    const CvMat *Update_Filter(float x, float y);
    void Add_Tile(TileObj *t);
    void Remove_Last_Tile();
    
};
#endif

