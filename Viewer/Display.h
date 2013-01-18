/* Doc- #################################
#######################################*/

#ifndef WXGL
#define WXGL

#include "../include/common.h"
#include "MainList.h"
#include "gl.h"
#include "ViewRec.h"
#include "FlagGUI.h"
#include "../include/threadpool.h"

#define REDRAW_TIMER_INTERVAL 100 // In msec


//
// Initialize the Display system
void Display_Init(int argc, char *argv[]);

//
// Run display loop this function does not return
void Display_Run();


#endif

