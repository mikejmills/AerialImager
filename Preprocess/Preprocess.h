/* Doc- #################################
#######################################*/

#ifndef PREPROC
#define PREPROC

#include "../include/TileObj.h"
#include <string.h>
#include <stdlib.h>
#include "perspective.h"
#include "network.h"

// Initalize the preprocessor
void Pre_init(void ( *func)(unsigned char *ptr, size_t len) );

void Pre_Create(char *filename);

#endif

