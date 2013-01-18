
#include "gl.h"

// Global GL mutex
pthread_mutex_t GL_mtx;

void GL_init()
{
    GLfloat eyePlaneS[] =
    {1.0, 0.0, 0.0, 0.0};
    GLfloat eyePlaneT[] =
    {0.0, 1.0, 0.0, 0.0};
    GLfloat eyePlaneR[] =
    {0.0, 0.0, 1.0, 0.0};
    GLfloat eyePlaneQ[] =
    {0.0, 0.0, 0.0, 1.0};
    
    printf("Initalize OpenGL\n");
    
    //glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_SMOOTH);
    
    // A handy trick -- have surface material mirror the color.
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    
    // Setup texturing
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_S, GL_EYE_PLANE, eyePlaneS);
    
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_T, GL_EYE_PLANE, eyePlaneT);
    
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_R, GL_EYE_PLANE, eyePlaneR);
    
    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_Q, GL_EYE_PLANE, eyePlaneQ);
}


void GL_MousetoGL(int x, int y, Vec *glpos)
{   
   // pthread_mutex_lock( &GL_mtx );
    //.......................................

    double modelview[16], projection[16];
    int viewport[4];
    float z; 
    
	//get the projection matrix		
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
	
    //get the modelview matrix		
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	
    //get the viewport		
    glGetIntegerv( GL_VIEWPORT, viewport );
	
    //Read the window z co-ordinate 
    //(the z value on that point in unit cube)	
	// OpenGL glReadPixels requires the z var to be a float type not double.
    // This seems to be consistent accross Mac and Linux.  
    glReadPixels( x, viewport[3]-y, 1, 1,
                 GL_DEPTH_COMPONENT, GL_FLOAT, &z );
    
    //Unproject the window co-ordinates to 
    //find the world co-ordinates.
    gluUnProject( x, viewport[3]-y, z, 
                 modelview, projection, viewport,
                 &(glpos->x), &(glpos->y), &(glpos->z) );
    
    //.......................................
    //pthread_mutex_unlock( &GL_mtx );
}


void GL_Draw_Camera(Camera *cam)
{
    //pthread_mutex_lock( &GL_mtx );
    //.......................................

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    cam->draw(); 
    
    //.......................................
    //pthread_mutex_unlock( &GL_mtx );
}

void GL_SwapBuffers()
{
   // pthread_mutex_lock( &GL_mtx );
    //.......................................
    
    glutSwapBuffers();
    
    //.......................................
   // pthread_mutex_unlock( &GL_mtx );
}


void GL_Load_Texture(IplImage *img, GLuint tName)
{
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glBindTexture(GL_TEXTURE_2D, tName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                 img->width, img->height, 
                 0, GL_BGR, GL_UNSIGNED_BYTE, 
                 img->imageData);
}

GLuint GL_Alloc_Texture()
{
    GLuint name;
    glGenTextures(1, &name);
    return name;
}

void GL_Release_Texture(GLuint *tName)
{
    glDeleteTextures(1, tName);
}

void GL_lock()
{
    pthread_mutex_lock( &GL_mtx );
}

int GL_trylock()
{
    return pthread_mutex_trylock( &GL_mtx );
}

void GL_unlock()
{
    pthread_mutex_unlock( &GL_mtx );
}



