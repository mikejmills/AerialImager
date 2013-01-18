/* Doc- #################################
#######################################*/

#ifndef PARTICLE
#define PARTICLE



#include <opencv/highgui.h>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <fcntl.h>
#include <sys/dir.h>
#include <vector>
#include <time.h>
#include "../include/common.h"
#include "../include/TileObj.h"

#define MAX_TARGETS 5

//
// Find and add targets from img to the provided tile file tf
TileFile target_find(TileFile tf, IplImage *img);

#endif

