/* Doc- #################################
#######################################*/

#ifndef GL_INTERFACE
#define GL_INTERFACE

#include <pthread.h>
#include "cameras.h"
#include "../include/common.h"
#include "../include/Vector.h"

void GL_init();

// Find the OpenGL world coordinates from the mouse position
// iii Must be called after a display iii
void GL_MousetoGL(int x, int y, Vec *glpos);


// Draw the camera
void GL_Draw_Camera(Camera *cam);

//  Swap GL Buffers
void GL_SwapBuffers();

// Load a img into texture memory
void GL_Load_Texture(IplImage *img, GLuint tName);

// Allocate GL texture
GLuint GL_Alloc_Texture();

// Release a texture
void GL_Release_Texture(GLuint *tName);


// GL mutex operations
void GL_lock();
int GL_trylock();
void GL_unlock();

#endif

