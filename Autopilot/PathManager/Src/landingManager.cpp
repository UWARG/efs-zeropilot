#include "waypointManager.hpp"
#include "landingManager.hpp"
#include <math.h>
#include <DataStructures/vectorClass.hpp>

const static int SET_THROTTLE_OFF = -1000;
const static double DISTANCE_OF_LANDING = 10; //in meters
const static int METERS_PER_DEG_LAT = 111320;
const static int ANGLE_OF_LANDING = 5; //in degrees
const static int STALL_SPEED_NO_PACKAGE = 20; //in km/h
const static int STALL_SPEED_WITH_PACKAGE = 30; //in km/h
const static double PI = 3.14159265358979323846;

double LandingManager::changingAltitude(_WaypointManager_Data_In * input, _PathData * aimingPoint, _PathData * intersectionPoint, _PathData * stoppingPoint)
{
    //vector declaration
    Vector3D aPoint(aimingPoint->longitude, aimingPoint->latitude, aimingPoint->altitude); //aiming point
    Vector3D iPoint(intersectionPoint->longitude, intersectionPoint->latitude, intersectionPoint->altitude); //intersection point
    Vector3D sPoint(stoppingPoint->longitude, stoppingPoint->latitude, stoppingPoint->altitude); //stopping point
    Vector3D cPoint; //current point
    Vector3D projectedPoint;

    //calculating the vectors
    Vector3D vectorAI(aPoint.getX() - iPoint.getX(), aPoint.getY() - iPoint.getY(), aPoint.getZ() - iPoint.getZ());
    Vector3D vectorAS(aPoint.getX() - sPoint.getX(), aPoint.getY() - sPoint.getY(), aPoint.getZ() - sPoint.getZ());
    Vector3D vectorAC(aPoint.getX() - cPoint.getX(), aPoint.getY() - cPoint.getY(), aPoint.getZ() - cPoint.getZ());
    Vector3D normal;
    
    //normal of the plane created by aiming, stopping, and intersection point
    normal = vectorAI.crossProduct(vectorAS);

    //projecting vectorAC to the normal
    //equation is the dot product of normal and vectorAC, divided by the norm of the normal squared, multiplied by the normal vector
    Vector3D projectionACToNormal;

    projectionACToNormal = normal * (normal.dotProduct(vectorAC) / pow(normal.norm(),2));

    //project point onto plane (cpoint minus the projection)

    projectedPoint = cPoint - projectionACToNormal;

    //determine the equation of the line of the slope and sub in projected values (symmetric form)
    double altitude = (((projectedPoint.getX() - aPoint.getX()) / vectorAS.getX()) + aPoint.getZ()) * vectorAS.getZ();

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
