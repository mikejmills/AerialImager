/* Doc- #################################
#######################################*/

#ifndef DISP_THREAD
#define DISP_THREAD

#include <pthread.h>
#include "MainList.h"
#include "ViewRec.h"
#include "../include/Vector.h"
//
// Start the Display Thread
// This function starts a thread which runs continuously to 
// update the Display_list 
void DispThrd_Init();

#endif

