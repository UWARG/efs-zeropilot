#include "waypointManager.hpp"
#include "landingManager.hpp"
#include <math.h>

const static int SET_THROTTLE_OFF = -1000;
const static double DISTANCE_OF_LANDING = 10; //in meters
const static int METERS_PER_DEG_LAT = 111320;
const static int ANGLE_OF_LANDING = 5; //in degrees
const static int STALL_SPEED_NO_PACKAGE = 20; //in km/h
const static int STALL_SPEED_WITH_PACKAGE = 30; //in km/h
const static double PI = 3.14159265358979323846;

double LandingManager::changingAltitude(_WaypointManager_Data_In * input, _PathData * aimingPoint, _PathData * intersectionPoint, _PathData * stoppingPoint)
{
    //arrays to hold the important points
    float aPoint[3]; //aiming point
    float iPoint[3]; //intersection point
    float sPoint[3]; //stopping point
    float cPoint[3]; //current point
    float projectedPoint[3];

    //loading important points
    aPoint[0] = aimingPoint->longitude;
    aPoint[1] = aimingPoint->latitude;
    aPoint[2] = aimingPoint->altitude; 

    iPoint[0] = intersectionPoint->longitude;
    iPoint[1] = intersectionPoint->latitude;
    iPoint[2] = intersectionPoint->altitude;

    sPoint[0] = stoppingPoint->longitude;
    sPoint[1] = stoppingPoint->latitude;
    sPoint[2] = stoppingPoint->altitude;

    cPoint[0] = input->longitude;
    cPoint[1] = input->latitude;
    cPoint[2] = input->altitude;

    //calculating the vectors
    float vectorAI[3];
    float vectorAS[3];
    float vectorAC[3];
    float normal[3];

    //A to I
    vectorAI[0] = iPoint[0] - aPoint[0];
    vectorAI[1] = iPoint[1] - aPoint[1];
    vectorAI[2] = iPoint[2] - aPoint[2];

    //A to S
    vectorAS[0] = sPoint[0] - aPoint[0];
    vectorAS[1] = sPoint[1] - aPoint[1];
    vectorAS[2] = sPoint[2] - aPoint[2];

    //A to C
    vectorAC[0] = cPoint[0] - aPoint[0];
    vectorAC[1] = cPoint[1] - aPoint[1];
    vectorAC[2] = cPoint[2] - aPoint[2];
    
    //normal of the plane created by aiming, stopping, and intersection point
    normal[0] = vectorAI[1]*vectorAS[2] - vectorAI[2]*vectorAS[1];
    normal[1] = vectorAI[2]*vectorAS[0] - vectorAI[0]*vectorAS[2]; //flipped negative
    normal[2] = vectorAI[0]*vectorAS[1] - vectorAI[1]*vectorAS[0];

    //projecting vectorAC to the normal
    //equation is the dot product of normal and vectorAC, divided by the norm of the normal squared, multiplied by the normal vector
    float projectionACToNormal[3];

    projectionACToNormal[0] = (normal[0]*vectorAC[0] + normal[1]*vectorAC[1] + normal[2]*vectorAC[2]) / (pow(normal[0],2) + pow(normal[1],2)+ pow(normal[2],2)) * normal[0];
    projectionACToNormal[1] = (normal[0]*vectorAC[0] + normal[1]*vectorAC[1] + normal[2]*vectorAC[2]) / (pow(normal[0],2) + pow(normal[1],2)+ pow(normal[2],2)) * normal[1];
    projectionACToNormal[2] = (normal[0]*vectorAC[0] + normal[1]*vectorAC[1] + normal[2]*vectorAC[2]) / (pow(normal[0],2) + pow(normal[1],2)+ pow(normal[2],2)) * normal[2];

    //project point onto plane (cpoint minus the projection)
    projectedPoint[0] = cPoint[0] - projectionACToNormal[0];
    projectedPoint[1] = cPoint[1] - projectionACToNormal[1];
    projectedPoint[2] = cPoint[2] - projectionACToNormal[2];

    //determine the equation of the line of the slope and sub in projected values (symmetric form)
    double altitude = (((projectedPoint[0] - aPoint[0]) / vectorAS[0]) + aPoint[2]) * vectorAS[2];

    return altitude;
}

double LandingManager::throttleOff(void)
{
    //return 0 for now, this function needs to be fixed
    return 0;
}

double approachSpeed(double windSpeed, bool ifPackage)
{
    //approach speed calculation for both package and no package scenarios
    if(ifPackage)
    {
        return STALL_SPEED_WITH_PACKAGE * 1.3 + windSpeed;
    }
    else 
    {
        return STALL_SPEED_NO_PACKAGE * 1.3 + windSpeed;
    }
}

double slowFlightSpeed(bool ifPackage)
{
    //slow flight speed calculation for both package and no package scenarios
    if(ifPackage)
    {
        return STALL_SPEED_WITH_PACKAGE + 5;
    }
    else 
    {
        return STALL_SPEED_NO_PACKAGE + 5;
    }
}

_LandingPath LandingManager::createSlopeWaypoints(_WaypointManager_Data_In * input, float stoppingLatitude, float stoppingLongitude, float stoppingAltitude, double directionLanding)
{
    _LandingPath path;
    path.stoppingPoint.latitude = stoppingLatitude;
    path.stoppingPoint.longitude = stoppingLongitude;
    path.stoppingPoint.altitude = stoppingAltitude;

    //creating points from stopping point to aiming point 
    
    //setting Z of aiming point
    path.aimingPoint.altitude = stoppingAltitude;
    
    //determining x and y of aiming point
    double radianDirection = directionLanding * 180 / PI;

    //finding the x and y components of the rolling distance vector
    double stoppingDistX = sin(radianDirection) * DISTANCE_OF_LANDING;
    double stoppingDistY = cos(radianDirection) * DISTANCE_OF_LANDING;

    //converting into x and y components in lat and lon
    float stoppingDistLat = stoppingDistY / METERS_PER_DEG_LAT;

    float metersPerDegLon = 40075000 * cos(stoppingLatitude)/360;

    float stoppingDistLon = stoppingDistX / metersPerDegLon;

    //subtracting the distances from stopping point to get aiming point
    path.aimingPoint.latitude = stoppingLatitude - stoppingDistLat;
    path.aimingPoint.longitude = stoppingLongitude - stoppingDistLon;

    //calculating the intersection point

    //determining altitude of intersection
    path.intersectionPoint.altitude = input->altitude;

    //determining the horizontal distance of intersection
    float horizDist = (input->altitude) / (ANGLE_OF_LANDING * PI / 180); //altitude in meters

    //finding the x and y components of the horizDist vector
    double slopeDistX = sin(radianDirection) * horizDist;
    double slopeDistY = cos(radianDirection) * horizDist;

    //converting x and y components
    float slopeDistLat = slopeDistY / METERS_PER_DEG_LAT;

    float slopeDistLon = slopeDistX / metersPerDegLon; //same conversion factor used here because we are flying at a relatively small scale. Lat adjustments will affect it very minimally

    //subtracting the distances from aiming point to get intersection point
    path.intersectionPoint.latitude = path.aimingPoint.latitude - slopeDistLat;
    path.intersectionPoint.longitude = path.aimingPoint.longitude - slopeDistLon;

    return path;
}
