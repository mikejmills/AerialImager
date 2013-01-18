/* Doc- #################################
#######################################*/

#ifndef MAINLIST
#define MAINLIST

#include <list>
#include <pthread.h>
#include "../include/TileObj.h"


class MainList
    {
        
    private:
        list<TileObj *> TList;
        pthread_rwlock_t TList_mtx;
        //pthread_mutex_t TList_mtx;
        int count;
    public:
        MainList();
        ~MainList();
        
        // Locking routines
        // These must be called when using an iterator
        void WLock();
        void RLock();
        void Unlock();
        
        void Insert(TileObj *tile);
        void Remove(TileObj *tile);

        // Insert Item assuming the system has been locked previously
        void Insert_Locked(TileObj *tile);
        
        // Remove a tile assuming the system has been locked previously
        void Remove_Locked(TileObj *tile);
        
        // Iterate through the list 
        void Iterate( void (*func)(TileObj *tile, void *compdata, MainList *l), void *compdata );
        
        // Sort the list of tiles
        void Sort( bool (*func)(TileObj *, TileObj *) );
        
        // Get the first element of the list
        TileObj *FirstElem();
        
        // Return the number of Tiles in the list
        int Count();
    };


// Main Tile List
extern MainList Main_Tile_List;


// Display Tile List
extern MainList Display_Tile_List;

// Tests
int ListTests();

#endif

