/* Doc- #################################
#######################################*/

#ifndef ALIGNMENT
#define ALIGNMENT

#include "MainList.h"
#include "../OpenSURF/surflib.h"
#include "vfs.h"
#include "Feature.h"
#include "vector.h"

typedef std::vector<std::pair<void *, Feature *> > FeatPairVec;
#define MIN_MATCHING_POINTS 30

// How close tiles should be before matching is performed
#define CLOSE_TILES_DISTANCE 200 //meters

int Initial_ProcessFeatures(TileObj *tf);

int Features_Matches_Count(TileObj *ntile, TileObj *tile, FeatPairVec &matches);

int Replace_Features(FeatPairVec &matches);

int Dissociate_Features(FeatPairVec &matches); 

void Align_NewTile(TileObj *t, FeatPairVec &matches);

int Initialize_KalmanFilters(TileObj *tile);

void Update_KalmanFilters(TileObj *t, FeatPairVec &matches);



#endif

