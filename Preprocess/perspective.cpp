
#include "perspective.h"


struct line {
    double t;
    Vec p0, p1;
};

struct plane {
    Vec p, norm;
};


void define_points(Vec *p1, Vec *p2, Vec *p3, Vec *p4, 
                   double roll, double pitch, 
                   double cam_angle, double focal_length, int img_width, int img_height)

{
    // Camera setting values that seem to generally work.
    // OpenGL texture project and this method of tile projection
    // do no match up exactly, these settings correct that.
    // More thought is necessary but don't have time right now
    // these adjust the tiles height and width by changing the 
    // images size a little
   
    // img_width += 150; 
   // img_height -= 100;
    
    double cmdiag  = tan(D2R(cam_angle/2))*focal_length * 2; // image sensor diagonal in cm's 
    
    double pixdiag = sqrt( pow( (double)img_width, 2) + pow( (double)img_height, 2) ); // image sensor diagonal in pixels
    
    double cmPpixel = cmdiag/pixdiag; // Could be improved by taking the diagonal of a pixel c and find a for height and width
 
    double hwidth = (img_width/2 * cmPpixel)/100; // Half the images width in meters

    double hheight = (img_height/2 * cmPpixel)/100; // Half the images height in meters


   // printf("%f %f - %f %f\n", cmdiag, pixdiag, hwidth, hheight);

    focal_length /= 100;
    
    p1->Set(-hwidth, hheight,  focal_length);
    p2->Set( hwidth, hheight,  focal_length);
    p3->Set( hwidth, -hheight, focal_length);
    p4->Set(-hwidth, -hheight, focal_length);
    
    p1->Rotate(p1, pitch, roll, 0);
    p2->Rotate(p2, pitch, roll, 0);
    p3->Rotate(p3, pitch, roll, 0);
    p4->Rotate(p4, pitch, roll, 0);
    
}

// Find intersection of the plane and a line
Vec *intersection(struct plane *pl, struct line *l)
{
    Vec *tmp = new Vec; // really bad don't do this fix later zzzzzzzzzzz
    double t;
    double d     = pl->p.Dot_Product( &(pl->norm) );
    double num   = d - (pl->norm.x * l->p0.x) -
                       (pl->norm.y * l->p0.y) -
                       (pl->norm.z * l->p0.z);  
    
    double denom = pl->norm.x * (l->p1.x - l->p0.x) + 
                   pl->norm.y * (l->p1.y - l->p0.y) + 
                   pl->norm.z * (l->p1.z - l->p0.z);
    
    if (denom == 0) {
        delete tmp;
        return NULL;
    }
    
    t = num/denom;
    
    l->p0.Scl_np(tmp, -1);
    l->p1.Add(tmp, tmp);
    tmp->Scl_p(t);
    tmp->Add(tmp, &l->p0);
    
    return tmp;
}

// Essentailly convert from Vector obj to
// the data point array in TileFile
void TF_addpoint(Vec *v, TileFile tf)
{
    //printf("Point %f %f %f\n", v->x, v->y, v->z);
}


// Determine the interpolation texture values based on the distance of the 
// base of each triangle generated by teselation in OpenGL.
/*void Texture_Points(struct TileFile *tf, Vec *tri1_p1, Vec *tri1_p2, Vec *tri2_p1, Vec *tri2_p2)
{

    
    // Find the distance at the base of each triangle
    double t1 = tri1_p1->Distance(tri1_p2);
    double t2 = tri2_p1->Distance(tri2_p2);
    printf("t1 %f t2 %f\n", t1, t2);
    tf->texpoints.tleft[0] = 0;
    tf->texpoints.tleft[1] = 0;
    tf->texpoints.tleft[2] = 0;
    tf->texpoints.tleft[3] = t1/t2;
    
    tf->texpoints.tright[0] = t1/t2;
    tf->texpoints.tright[1] = 0; //0t1/t2;
    tf->texpoints.tright[2] = 0;
    tf->texpoints.tright[3] = t1/t2;
    
    tf->texpoints.bright[0] = t1/t2;
    tf->texpoints.bright[1] = t1/t2;
    tf->texpoints.bright[2] = 0;
    tf->texpoints.bright[3] = t1/t2;
    
    tf->texpoints.bleft[0] = 0;
    tf->texpoints.bleft[1] = t1/t2;
    tf->texpoints.bleft[2] = 0;
    tf->texpoints.bleft[3] = t1/t2;
    
    
        
} */                                    


void Persp_Correction(TileFile tf, 
                      double cam_angle, double aspect_ratio)
{
    struct line   lines[4]; //Lines that define the corner rays.
    struct plane  pl;       //Plane to intersect with
    Vec          *v[4];    //Solution points
    struct line   center;
    Vec          *res;
    
    // Define vectors
    lines[0].p0.Set(0.0, 0.0, 0.0);
    lines[1].p0.Set(0.0, 0.0, 0.0);
    lines[2].p0.Set(0.0, 0.0, 0.0);
    lines[3].p0.Set(0.0, 0.0, 0.0);
    
    // Define the points needed
    
    define_points( &lines[0].p1, &lines[1].p1, &lines[2].p1, &lines[3].p1,
                   DOUBLE(tf, MAIN_HEADER_roll), 
                   DOUBLE(tf, MAIN_HEADER_pitch), 
                   cam_angle,
                   5, INT(tf, IMAGE_HEADER_width(0)), INT(tf, IMAGE_HEADER_height(0)) );
    
    
    // Set plane points
    pl.p.Set(0, 0, DOUBLE(tf, MAIN_HEADER_alt));
    pl.p.Unit(&pl.norm);
    
    center.p0.Set(0, 0, 0);
    center.p1.Set(0, 0, DOUBLE(tf, MAIN_HEADER_alt));
    
    center.p1.Rotate( &center.p1, 
                     DOUBLE(tf, MAIN_HEADER_pitch), 
                     DOUBLE(tf, MAIN_HEADER_roll), 0);
    
    res = intersection(&pl, &center);
    
    // Find the four unit points solutions
    v[0] = intersection(&pl, &lines[0]);
    v[1] = intersection(&pl, &lines[1]);
    v[2] = intersection(&pl, &lines[2]);
    v[3] = intersection(&pl, &lines[3]);
       
    
    // Set the points in the TileFile Obj
    DOUBLE(tf, TILE_POINT_OFF(0, 0)) = v[0]->x;
    DOUBLE(tf, TILE_POINT_OFF(0, 1)) = v[0]->y;
    DOUBLE(tf, TILE_POINT_OFF(0, 2)) = v[0]->z;
    
    
    DOUBLE(tf, TILE_POINT_OFF(1, 0)) = v[1]->x;
    DOUBLE(tf, TILE_POINT_OFF(1, 1)) = v[1]->y;
    DOUBLE(tf, TILE_POINT_OFF(1, 2)) = v[1]->z;
    
    DOUBLE(tf, TILE_POINT_OFF(2, 0)) = v[2]->x;
    DOUBLE(tf, TILE_POINT_OFF(2, 1)) = v[2]->y;
    DOUBLE(tf, TILE_POINT_OFF(2, 2)) = v[2]->z;
    
    
    DOUBLE(tf, TILE_POINT_OFF(3, 0)) = v[3]->x;
    DOUBLE(tf, TILE_POINT_OFF(3, 1)) = v[3]->y;
    DOUBLE(tf, TILE_POINT_OFF(3, 2)) = v[3]->z;
    
    DOUBLE(tf, TILE_POINT_OFF(4, 0)) = res->x;
    DOUBLE(tf, TILE_POINT_OFF(4, 1)) = res->y;
    DOUBLE(tf, TILE_POINT_OFF(4, 2)) = res->z;
    
    
    
    delete v[0];
    delete v[1];
    delete v[2];
    delete v[3];
    delete res;

}



void PerspTests()
{
    struct plane pl;
    struct line  l;
    
    
    // Straight line down the z axis
    pl.norm.Set(0, 0, -221);
    pl.p.Set(0,0,221);
    
    l.p0.Set(0, 0, 0);
    l.p1.Set(0, 0, 3);
    Vec *res= intersection(&pl, &l);
    
    if (res) {
        printf(" Correct 0 0 221 ");
        res->Print();
    } else 
        cout << "Error" << endl;
    
    //// Straight line down the z axis inverse plane norm
    pl.norm.Set(0, 0, 221);
    pl.p.Set(0,0,221);
    
    l.p0.Set(0, 0, 0);
    l.p1.Set(0, 0, 3);
    res= intersection(&pl, &l);
    
    if (res) {
        printf(" Correct 0 0 221 ");
        res->Print();
    } else 
        cout << "Error" << endl;
 
    
    pl.norm.Set(0, 0, 221);
    pl.p.Set(0,0,221);
    
    l.p0.Set(0, 0, 0);
    l.p1.Set(0, 0, 3);
    res= intersection(&pl, &l);
    
    if (res) {
        printf(" Correct 0 0 221 ");
        res->Print();
    } else 
        cout << "Error" << endl;
}


