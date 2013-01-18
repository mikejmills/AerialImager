
#include "Alignment.h"

// For any new image create the list of features to keep track of.
//  Use the scale to get good features

// Find the matching features
//    Search within the area view rectange. ( should be able to reuse the code )
//    If the features within this area match

// 

//
// Sort features based on the scale at which they where detected


bool Ipoint_Sort_Scale(Ipoint ip1, Ipoint ip2)
{
    return (ip1.scale > ip2.scale);
}



int Initial_ProcessFeatures(TileObj *tf)
{
    IpVec ipts;
    double rx, ry;
    Feature *f;
    
    // Read image from disk
    IplImage *img = VFS_ReadTileImage(tf->filename, tf->tiledata, 0);
    
    surfDetDes(img, ipts, false, 5, 4, 2, 0.0001f);
    
    // Sort the points by scale
    sort(ipts.begin(), ipts.end(), Ipoint_Sort_Scale);
    
    // Remove all but the top 10% of features based on scale
    ipts.erase(ipts.begin()+(ipts.size()*0.1), ipts.end());
    //tf->ipts.erase(tf->ipts.begin()+1, tf->ipts.end());
    
    // Create Feature list from the ipts detected
    for (unsigned int i = 0; i < ipts.size(); i++) {
        tf->PixelCoordinates(ipts[i].x, ipts[i].y, &rx, &ry);
        f = new Feature(ipts[i], rx, ry);
        f->Add_Tile(tf);
        tf->feature_list.push_back( f );

    }
    
  
    cvReleaseImage(&img);
    return 1;
}



int Features_Matches_Count(TileObj *ntile, TileObj *tile, FeatPairVec &matches)
{
    float dist, d1, d2;
    Feature *match = NULL;
    
    matches.clear();
    
    for(unsigned int i = 0; i < ntile->feature_list.size(); i++) {
        d1 = d2 = FLT_MAX;
        
        for(unsigned int j = 0; j < tile->feature_list.size(); j++) {
            dist = ntile->feature_list[i]->ip - tile->feature_list[j]->ip;
            
            if(dist<d1) // if this feature matches better than current best
            {
                d2 = d1;
                d1 = dist;
                match = tile->feature_list[j];
            }
            else if(dist<d2) // this feature matches better than second best
            {
                d2 = dist;
            }
        }
        
        // If match has a d1:d2 ratio < 0.65 ipoints are a match
        if(d1/d2 < 0.65) 
        { 
            matches.push_back(std::make_pair( (void *)(ntile->feature_list[i]), match));
            match->Add_Tile(ntile);
        }
    
    }
    return matches.size();
}

int Replace_Features(FeatPairVec &matches)
{
    unsigned int i;
    
    for (i = 0; i < matches.size(); i++) {
        //delete (Feature *)(matches[i].first);
        matches[i].first = (void *)matches[i].second;
        
       // printf("Replace Feature %f %f\n", ((Feature *)matches[i].first)->px, ((Feature *)matches[i].first)->py);
    }
    return i;
}

int Dissociate_Features(FeatPairVec &matches) 
{
    unsigned int i;
    
    for (i = 0; i < matches.size(); i++) {
        matches[i].second->Remove_Last_Tile();
       
    }
    return i; 
}


int Initialize_KalmanFilters(TileObj *tile)
{
    int count = 0;
    
    
    for(unsigned int i = 0; i < tile->feature_list.size(); i++) {
        if (tile->feature_list[i]->kalman == NULL) {
            tile->feature_list[i]->Create_KalmanFilter();
            count++;
        }
    }
    
    return count;
}

void Transform_Point(double *h, 
                     double x, double y, 
                     double *xres, double *yres, double *zres)
{
    *zres = 1./(h[6]*x + h[7]*y + h[8]);
    *xres = (h[0]*x + h[1]*y + h[2])*(*zres);
    *yres = (h[3]*x + h[4]*y + h[5])*(*zres);
    
}

void Update_Features(TileObj *t, double *h)
{
    
    double xres, yres, zres;
    for (unsigned int i=0; i < t->feature_list.size(); i++) {
  
        Transform_Point(h, 
                        t->feature_list[i]->px, t->feature_list[i]->py,
                        &xres, &yres, &zres);
        
        t->feature_list[i]->px = xres;
        t->feature_list[i]->py = yres;
    }
}

int In_List(TileObj *t, vector<TileObj *> &l)
{
    for (unsigned int i=0; i < l.size(); i++) {
        if (l[i] == t) return 1;
    }
    return 0;
}


void Update_KalmanFilters(TileObj *t, FeatPairVec &matches)
{
    Vec trans, v;
    double px, py, avgmag = 0;
    vector<TileObj *> l;
    
    printf("Initial list size %d\n", matches[0].second->TileList.size());
    for (unsigned int i=0; i < matches.size(); i++) {
        v.x = ((Feature *)(matches[i].first))->px;
        v.y = ((Feature *)(matches[i].first))->py;
        
        ((Feature *)(matches[i].first))->Update_Filter(matches[i].second->px, matches[i].second->py);
        

        
        v.x = ((Feature *)(matches[i].first))->px - v.x;
        v.y = ((Feature *)(matches[i].first))->py - v.y;
        v.Print();        
        printf("v mag %d\n", v.Mag());
        
        avgmag += v.Mag();
        trans.Add(&trans, &v);
        
        for (unsigned int j = 0; j < ((Feature *)(matches[i].first))->TileList.size(); j++) {
            if (!In_List(matches[i].second->TileList[j], l)) {
                l.push_back(matches[i].second->TileList[j]);
                //Search_Tile(matches[i].second->TileList[j], l, 5);
                printf("Push Tile %d\n", l.size());
            }
        }
        
    }
    
    //sort(l.begin(), l.end());
    //l.erase( unique( l.begin(), l.end() ), l.end() );
    
    
    
    trans.Unit(&trans);
    trans.Scl_p(avgmag/matches.size());
    
    printf("New Size %d\n", l.size());
    trans.Print();
    
    for (unsigned int i=0; i < l.size(); i++) {
        printf("Tile %X\n", (void *)l[i]);
        l[i]->GLpos->x += trans.x;
        l[i]->GLpos->y += trans.y;
    }
}

void Align_NewTile(TileObj *t, FeatPairVec &matches)
{
    unsigned int n = 0;
    vector<CvPoint2D32f> pt1, pt2;
    CvMat ntpts, kfpts;
    double xres, yres, zres;
    double h[9];
    CvMat hmMat = cvMat(3, 3, CV_64F, h);
    
    n = (unsigned int)matches.size();
    
    printf("Match Size for Homography matrix is %d\n", n);
    
    pt1.resize(n);
    pt2.resize(n);
    
    // Find Homography from the new Tile to the kalman filter positions of the matching features
    for (unsigned int i = 0; i < n; i++) {
        pt1[i] = cvPoint2D32f( ((Feature *)(matches[i].first))->px, ((Feature *)(matches[i].first))->py);
        pt2[i] = cvPoint2D32f( matches[i].second->px, matches[i].second->py);
    }
    
    
    
    ntpts = cvMat(1, n, CV_32FC2, &pt1[0]);
    kfpts = cvMat(1, n, CV_32FC2, &pt2[0]);
    
//    CV_LMEDS
    // Find the homography (transformation) between the two sets of points
    cvFindHomography(&ntpts, &kfpts, &hmMat, CV_RANSAC, 1);  // this line requires opencv 1.1
    
//    printf(" %f %f %f\n %f %f %f\n %f %f %f\n", h[0],h[1],h[2], h[3],h[4],h[5], h[6],h[7],h[8]);
    
    //
    // Update the features positions.
    Update_Features(t, h);
    
    
    //
    // Recalc Texture projection Up vector
    Transform_Point(h, 
                    t->Up.x, t->Up.y,
                    &xres, &yres, &zres);
    
    t->Up.x = xres - h[2]; // Remove translational component
    t->Up.y = yres - h[5];
    
    
    //
    // tleft
    Transform_Point(h, 
                    DOUBLE(t->tiledata, TILE_POINT_OFF(0,0)), DOUBLE(t->tiledata, TILE_POINT_OFF(0,1)),
                    &xres, &yres, &zres);
                    
    DOUBLE(t->tiledata, TILE_POINT_OFF(0,0)) = xres;
    DOUBLE(t->tiledata, TILE_POINT_OFF(0,1)) = yres;
    printf("%f %f\n", xres, yres);
    
    //
    // tright
    Transform_Point(h, 
                    DOUBLE(t->tiledata, TILE_POINT_OFF(1,0)), DOUBLE(t->tiledata, TILE_POINT_OFF(1,1)),
                    &xres, &yres, &zres);
    
    DOUBLE(t->tiledata, TILE_POINT_OFF(1,0)) = xres;
    DOUBLE(t->tiledata, TILE_POINT_OFF(1,1)) = yres;
    printf("%f %f\n", xres, yres);
    
    //
    // bright
    Transform_Point(h, 
                    DOUBLE(t->tiledata, TILE_POINT_OFF(2,0)), DOUBLE(t->tiledata, TILE_POINT_OFF(2,1)),
                    &xres, &yres, &zres);
    
    DOUBLE(t->tiledata, TILE_POINT_OFF(2,0)) = xres;
    DOUBLE(t->tiledata, TILE_POINT_OFF(2,1)) = yres;
    printf("%f %f\n", xres, yres);
    
    //
    // bleft
    Transform_Point(h, 
                    DOUBLE(t->tiledata, TILE_POINT_OFF(3,0)), DOUBLE(t->tiledata, TILE_POINT_OFF(3,1)),
                    &xres, &yres, &zres);
    
    DOUBLE(t->tiledata, TILE_POINT_OFF(3,0)) = xres;
    DOUBLE(t->tiledata, TILE_POINT_OFF(3,1)) = yres;
    printf("%f %f\n", xres, yres);
    
    //
    // center
    Transform_Point(h, 
                    DOUBLE(t->tiledata, TILE_POINT_OFF(4,0)), DOUBLE(t->tiledata, TILE_POINT_OFF(4,1)),
                    &xres, &yres, &zres);
    
    DOUBLE(t->tiledata, TILE_POINT_OFF(4,0)) = xres;
    DOUBLE(t->tiledata, TILE_POINT_OFF(4,1)) = yres;
    printf("%f %f\n", xres, yres);
    
       
    
}
