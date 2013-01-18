/* Doc- #################################
#######################################*/

#ifndef GEOCONV
#define GEOCONV

#include "../include/Vector.h"

#define EARTH_RADIUS       6371 //km

class GeoCoord
    {
    public:
        double lat, lon, alt;
        
        GeoCoord(double lati, double loni, double alti);
        GeoCoord();
        
        void   Set(GeoCoord *org);
        double Distance(GeoCoord *org);
        void   Geo_Meters(Vec *GLpos, GeoCoord *origpoint);
        void   Meters_Geo(GeoCoord *orig, Vec *GLpos);
    };

#endif

