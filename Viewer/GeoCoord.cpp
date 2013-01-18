
#include "GeoCoord.h"

Vec NORTH_VEC(0, 1, 0); //Init North Vector

// finds the distance in meters between two geograhic coordinates
// accounts for the curve of the earth using Haversine formula.
double GeoDistance(GeoCoord *pos, GeoCoord *origpoint)
{
    double dlat = pos->lat - origpoint->lat;
    double dlon = pos->lon - origpoint->lon;
    
    double a = (sin( D2R(dlat/2) ) * sin( D2R(dlat/2) )) + 
    (cos( D2R(origpoint->lat) ) * cos( D2R(pos->lat) ) * 
    (sin( D2R(dlon/2) ) * sin( D2R(dlon/2) )));
    
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return (EARTH_RADIUS * c) * 1000;
}

double Bearing_angle(Vec *v)
{
    // Find angle between north vector and the vector defined by the coordinates
    double angle = R2D(acos( (v->Dot_Product(&NORTH_VEC) / v->Mag()) ));
    
    // Correct for rotation over 180
    if ( v->x < NORTH_VEC.x ) angle = 360 - angle ;
    
    return angle;
    
}   



GeoCoord::GeoCoord(double lati, double loni, double alti)
{
    lat = lati;
    lon = loni;
    alt = alti;
}

GeoCoord::GeoCoord()
{
    lat = 0;
    lon = 0;
    alt = 0;
}

void GeoCoord::Set(GeoCoord *org)
{
    lat = org->lat;
    lon = org->lon;
    alt = org->alt;
}

double GeoCoord::Distance(GeoCoord *org)
{
    return GeoDistance(this, org);
}

void GeoCoord::Geo_Meters(Vec *GLpos, GeoCoord *origpoint)
{
    double d = GeoDistance(this, origpoint);
    
    GLpos->x = lon - origpoint->lon;
    GLpos->y = lat - origpoint->lat;
    GLpos->z = 0;
        
    GLpos->Unit(GLpos);
    GLpos->Scl_p(d);
    if (d == 0) GLpos->Set(0, 0, 0);
    
}


void GeoCoord::Meters_Geo(GeoCoord *orig, Vec *GLpos)
{
    
    double d = GLpos->Mag() / 1000; // Convert to km
    
    double angle = D2R(Bearing_angle(GLpos));
    
    double dE = d/EARTH_RADIUS;
    
    lat = R2D(asin( sin(D2R(orig->lat)) * cos(dE) + cos(D2R(orig->lat)) * sin(dE) * cos(angle) ));
    
    lon = orig->lon + R2D(atan2( sin(angle) * sin(dE) * cos(D2R(orig->lat)),
                                 cos(dE) - sin(D2R(orig->lat)) * sin(D2R(lat)) ));
}
