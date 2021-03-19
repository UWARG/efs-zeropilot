#include <math.h>
#include "trackFinder.hpp"

double getTrack(double latitudeInitial, double longitudeInitial, double latitudeCurrent, double longitudeCurrent)
{
    double deg2rad = 3.14159265358979323/180;
    double x = cos(latitudeCurrent*deg2rad) * sin((longitudeCurrent - longitudeInitial)*deg2rad);
    double y = cos(latitudeInitial*deg2rad) * sin(latitudeCurrent*deg2rad) - sin(latitudeInitial*deg2rad) * cos(latitudeCurrent*deg2rad) * cos((longitudeCurrent-longitudeInitial)*deg2rad);
    
    if(atan2(x,y)<0)
    {
        return (atan2(x,y)/deg2rad + 360);
    }
    else
    {
        return (atan2(x,y)/deg2rad);   
    }
}