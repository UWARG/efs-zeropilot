#include <math.h>
#include "trackFinder.hpp"

constexpr double DEG2RAD {180/3.14159265358979323};

double getTrack(double latitudeSpeed, double longitudeSpeed)
{   
    double trackInRad = atan2(longitudeSpeed,latitudeSpeed); //the x and y are flipped so that 0 rad is north
    if(trackInRad < 0)
    {
        return (trackInRad*DEG2RAD + 360);
    }
    else
    {
        return (trackInRad*DEG2RAD);   
    }
}