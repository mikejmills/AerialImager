
#include "DisplayThread.h"

pthread_t       DispThrd;
pthread_attr_t  DispThrd_attr;

//------------------------------------------------------------------------
// Main_Tile_List functions

// Check for tiles that should be added to the Display_Tile_List
void DisplayCheck(TileObj *tile, void *compdata, MainList *l)
{
    // Set the GLpos of the tile based on the View Rectangle
    if ( tile->GLpos == NULL ) {
        tile->Set_GLpos(GVRec->Get_Origin());
    }
    
    
    if ( GVRec->InDisplayable(tile->GLpos) ) {
        // Swap the tile to the Display list
        // Main_Tile_List is Locked at this point
        // Display List is not 
        Main_Tile_List.Remove_Locked(tile); 
        Display_Tile_List.Insert(tile); 
        
        // Set the Tile To Active state
        tile->Activate();

    }
}



//------------------------------------------------------------------------
// Display_Tile_List functions

// Operations that must be carried out on the Display_Tile_List
void DisplayOperations(TileObj *t, void *compdata, MainList *l)
{
    
    // Check if tile is still displayable
    if ( !GVRec->InDisplayable(t->GLpos) ) {
        t->Deactivate();
    }
    
    // Resize image if necessary and check for removal from display list is possible
    if (t->Standard_run(GVRec->ImageSize()) == TILE_STD_INACTIVE) {
        // Swap the tile back to the Main List
        // Display_Tile_List is locked at this point
        // Main_Tile_List is not
        Display_Tile_List.Remove_Locked(t); 
        Main_Tile_List.Insert(t);
    }
    
       
}



//------------------------------------------------------------------------
// Thread function

void *DisplayThreadFunc( void *arg )
{    
    // Wait until a Image has been recv from a server
    while ( !Main_Tile_List.Count() ) {
        sleep(1);
    }
    
    
    
    // Setup view rect to have origin point of this first picture
    ViewRec_Init_Global( Main_Tile_List.FirstElem()->Get_GeoPosition() ); 
    
    while (1) {
        
        // Update the Display_Tile_List from Main_Tile_List
        Main_Tile_List.Iterate(DisplayCheck, NULL);
        
        // Sort the Display tiles for later priority
        Display_Tile_List.Sort(TileCompare);
        
        // Tell View rect how many pictures will be displayed
        GVRec->Display_Count( Display_Tile_List.Count() );
        
        // Run updates on the tiles in Display_Tile_List
        Display_Tile_List.Iterate(DisplayOperations, NULL);

        sleep(1);
    }
}


void DispThrd_Init()
{
    TileObj_Init();
    
    pthread_attr_init( &DispThrd_attr );
    pthread_attr_setdetachstate( &DispThrd_attr, PTHREAD_CREATE_DETACHED );
    
    pthread_create( &DispThrd, &DispThrd_attr, DisplayThreadFunc, NULL);
}

