#include "MainList.h"

MainList Main_Tile_List;
MainList Display_Tile_List;


MainList::MainList()
{
    pthread_rwlock_init(&TList_mtx, NULL);
    count = 0;
}

MainList::~MainList()
{
    
    list<TileObj *>::iterator i;
    
    for (i = TList.begin(); i != TList.end();i++){
        delete (*i);
    }
}

void MainList::WLock()
{
    pthread_rwlock_wrlock(&TList_mtx);
}

void MainList::RLock()
{
    pthread_rwlock_rdlock(&TList_mtx);
}


void MainList::Unlock()
{
    pthread_rwlock_unlock(&TList_mtx);
}




void MainList::Insert(TileObj *tile)
{
    WLock();
    //.................................
    
    TList.push_front(tile);
    count++;
    
    //.................................
    Unlock();
    
}

void MainList::Remove(TileObj *tile)
{
    WLock();
    //.................................
    
    TList.remove(tile);
    count--;
    
    //.................................
    Unlock();
}

void MainList::Insert_Locked(TileObj *tile)
{
    //.................................
    TList.push_front(tile);
    count++;
    //.................................
}

void MainList::Remove_Locked(TileObj *tile)
{
    //.................................
    TList.remove(tile);
    count--;
    //.................................
}

TileObj *MainList::FirstElem()
{
    return *(TList.begin());
}


void MainList::Sort( bool (*func)(TileObj *, TileObj *) )
{
    WLock();
    //...............................
    TList.sort(func);
    //...............................
    Unlock();
}


void MainList::Iterate( void (*func)(TileObj *tile, void *compdata, MainList *l), void *compdata )
{
    if (!func) return;
    
    if (count <= 0) return;
    
    list<TileObj *>::iterator i;
    TileObj *tmp;
    
    
    WLock();
    //..............................
    for (i = TList.begin(); i != TList.end();){
        tmp = (TileObj *)(*i); // Save pointer 
        i++; // increment before the func incase the elem is removed
             // this is probably not necessary but it is safe
        func( tmp, compdata, this );
        
    }
    //..............................
    Unlock();
    
}

int MainList::Count()
{
    int t;
    
    RLock();
    t = count;
    Unlock();
    
    return t;
}



void func (TileObj *tile, void *comp, MainList *l)
{
    cout << *((int *)tile) << endl;
    if ( *((int *)tile) == 1 ) {
        cout << "Remove 1" << endl;
        l->Remove((TileObj *)tile); // test the remove from list during a iteration
    }
}


int ListTests()
{
    MainList l;
    
    int t1= 1, t2=2;
    
    l.Insert( (TileObj *)&t1 );
    l.Insert( (TileObj *)&t2 );
    
   // l.Iterate(func, NULL);
   // l.Iterate(func, NULL);
    return 0;
}

