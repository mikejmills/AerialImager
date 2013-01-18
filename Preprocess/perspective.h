/* Doc- #################################
#######################################*/

#ifndef PERSPECT
#define PERSPECT

#include "../include/Vector.h"
#include "../include/TileObj.h"

void PerspTests();


void Persp_Correction(TileFile tf, 
                      double cam_angle, double aspect_ratio);
#endif

