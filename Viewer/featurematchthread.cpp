
#include "featurematchthread.h"


pthread_t       FeatThrd;
pthread_attr_t  FeatThrd_attr;



// Check for tiles that should be added to the Display_Tile_List
void FeatureCheck(TileObj *tile, void *compdata, MainList *l)
{
    FeatPairVec matches;
    
    
    // Set the GLpos of the tile based on the View Rectangle
    if ( tile->GLpos == NULL ) {
        return;
    }
    
    TileObj *t = (TileObj *)compdata;
    
    if (t == tile) return;
    
    if ((t->GLpos != NULL ) && ( tile->GLpos->Distance( t->GLpos ) < CLOSE_TILES_DISTANCE )) {
        
        
        // Not enough matching points to perform a good alignment
        // remove any associations made well matching
        if (Features_Matches_Count(t, tile, matches) < MIN_MATCHING_POINTS) {
            Dissociate_Features(matches);
            return;
        }
        
        //Update Kalman filters with the new measurements
        Update_KalmanFilters(t, matches);
        
        Align_NewTile(t, matches);
        
        
        //Replace the matching features of the new tile the features we just transformed to.
        Replace_Features(matches);
        
    } 
    
}


//
// Thread function
void *FeatureThreadFunc( void *arg )
{    
    int prev_count = Features_Tile_List.Count();
    
    // Wait until more then 2 images have been recv from a server
    while ( Features_Tile_List.Count() < 1 ) {
        sleep(1);
    }
    
    while (1) {
        TileObj *t;

        
        
        // Check if a new tile has been added
        if ( prev_count == Features_Tile_List.Count() ) {
            sleep(1);
            continue;
        }
        
        prev_count = Features_Tile_List.Count();
            
        t = Features_Tile_List.LastestTile();
        
        // Initialize any remaining features with Kalman Filters
        //int count = Initialize_KalmanFilters(t);
        //printf("New Kalman filters %d\n", count);
        
        
        if (!t) continue;

        Features_Tile_List.Iterate(FeatureCheck, t);
        
        
        
               
        
        sleep(1);
    }
    
    return NULL;
}


void FeatThrd_Init()
{
    TileObj_Init();
    
    pthread_attr_init( &FeatThrd_attr );
    pthread_attr_setdetachstate( &FeatThrd_attr, PTHREAD_CREATE_DETACHED );
    
    pthread_create( &FeatThrd, &FeatThrd_attr, FeatureThreadFunc, NULL);
}
