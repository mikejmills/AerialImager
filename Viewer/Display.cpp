
#include "Display.h"
#include "FlagGUI.h"

int mouse_x, mouse_y, mouse_x2, mouse_y2 = 0;
int menu_mouse_save_x,menu_mouse_save_y;

int SCREEN_HALF_WIDTH, SCREEN_HALF_HEIGHT;

TileObj *load_tile = NULL; // Variable to hold the texture to be loaded during this latest update

// Run the tile Draw function for tile
void TileDisplay(TileObj *tile, void *comp, MainList *l)
{
    
    // If the tile does not draw a texture image
    // then it needs to be loaded
    if ( (tile->GL_run() == TILE_GL_LOAD) && !load_tile) {
        //printf("SERIAL GL LOAD\n");
        load_tile = tile;
    }
}

void LoadTexture()
{
    // Load the image of a single tile into GL memory
    if (load_tile)
        load_tile->GL_Load();
    load_tile = NULL;
}


void Display_Func()
{
    // No images have come in from the server yet
    if (!GVRec) return; 
    
    // Setup camera 
    GVRec->DrawCam();
 
    // draw each tile
    Display_Tile_List.Iterate(TileDisplay, NULL);
    
    GL_SwapBuffers();
    
    // Load any waiting texture image
    LoadTexture();
    
}





//----------------------------------------------------------------------------------------------
void Display_Reshape(int width, int height)
{
    SCREEN_WIDTH  = width;
    SCREEN_HEIGHT = height;
    
    // calculate half the screen dimensions
    SCREEN_HALF_WIDTH  = width/2;
    SCREEN_HALF_HEIGHT = height/2;
    
    glViewport(0, 0, width, height);
}


void Mouse_Motion(int button, int state, int x, int y)
{

    switch (button) 
    {
        case GLUT_LEFT_BUTTON:
            mouse_x2 = mouse_x = x;  // Set origin point of motion
            mouse_y2 = mouse_y = y;
            break;
            
        case GLUT_RIGHT_BUTTON:
            mouse_x = x;
            mouse_y = y;
            printf("%d %d\n", mouse_x, mouse_y);
            break;
            
    }
    
}

void Motion(int x, int y)
{
    mouse_x2 = x; //Update motion origin
    mouse_y2 = y;
}


void Special_Key(int key, int x, int y)
{
    
    switch (key)
    {
        case GLUT_KEY_UP:
            GVRec->Zoom(-1);
            break;
    
        case GLUT_KEY_DOWN:
            GVRec->Zoom(1);
            break;
    }
    
    
}

void Passive_Motion(int x, int y)
{
    Vec glpos;
    GeoCoord geopos;
    char titlestr[40];
    
    if (!GVRec) return;
    
    GeoCoord *orig = GVRec->Get_Origin();

    GL_MousetoGL(x, y, &glpos);
    
    geopos.Meters_Geo(orig, &glpos);
    
    sprintf(titlestr, "lat %.8f lon %.8f\n", geopos.lat, geopos.lon);
    glutSetWindowTitle(titlestr);
    
    menu_mouse_save_x = x;
    menu_mouse_save_y = y;
}



void Timer_Func(int value)
{
    if (!GVRec) goto RESET_TIMER; // No images have come in from the server yet

    Display_Func();
    
    // No movement has occured so just reset the timer
    if ( (mouse_x == mouse_x2) && (mouse_y == mouse_y2) ) goto RESET_TIMER;
    
    
    // Move the view rectange to new position
    GVRec->Move(( mouse_x - mouse_x2) + SCREEN_HALF_WIDTH, 
                ( mouse_y - mouse_y2) + SCREEN_HALF_HEIGHT);
        
    // reset the mouse coordinates
    mouse_x = mouse_x2;
    mouse_y = mouse_y2;
    
RESET_TIMER:
    
    glutTimerFunc(REDRAW_TIMER_INTERVAL, Timer_Func, 0); //Must reset the timer in GLUT
}




#define ADD_FLAG 1
void menu_process(int option)
{
    Vec glpos;
    GeoCoord geopos;
    
    
    switch (option) {
        case ADD_FLAG:
            printf("%d %d\n", mouse_x, mouse_y);
            if (!GVRec) break;
            
            Display_Func();
            GL_MousetoGL(menu_mouse_save_x, menu_mouse_save_y, &glpos);
            geopos.Meters_Geo(GVRec->Get_Origin(), &glpos);
            Add_Flag(glpos.x, glpos.y, geopos.lat, geopos.lon);
            
            printf("ADD FLAG %f %f %f %f\n", glpos.x, glpos.y, geopos.lat, geopos.lon);
            
            break;
    }
    
}

void Create_Menu()
{
    
    int menu = glutCreateMenu(menu_process);
    
    //add entries to our menu
	glutAddMenuEntry("Add Flag",ADD_FLAG);
    
    // attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void Display_Init(int argc, char *argv[])
{
    load_tile = 0;
    glutInit(&argc, argv);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("VIEWER");
    
    glutDisplayFunc( &Display_Func );
    glutReshapeFunc( &Display_Reshape );
    
    Display_Reshape(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    GL_init();
    
    glutMouseFunc(&Mouse_Motion);
    glutMotionFunc(&Motion);
    glutPassiveMotionFunc(Passive_Motion);
    
    glutTimerFunc(REDRAW_TIMER_INTERVAL, Timer_Func, 0);
    printf("INIT TIMER\n");
    glutSpecialFunc(Special_Key);
    Create_Menu();
    
}

void Display_Run()
{
    // Start the Flag GUI 
    FlagGUI_Start();
    //sleep(50);
    glutMainLoop();
}