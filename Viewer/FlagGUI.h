/* Doc- #################################
#######################################*/

#ifndef FLAGGUI
#define FLAGGUI

#include <pthread.h>

void FlagGUI_Start();


//
// Add a flag with x,y GL coordinates
void Add_Flag(double x, double y, double lat, double lon);

#endif

