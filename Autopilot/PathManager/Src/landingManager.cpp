#include "waypointManager.hpp"
#include "landingManager.hpp"
#include <math.h>
#include "vectorClass.hpp"

double LandingManager::changingAltitude(_WaypointManager_Data_In input, _PathData aimingPoint, _PathData intersectionPoint, _PathData stoppingPoint)
{
    //vector declaration
    Vector3D aPoint(aimingPoint.longitude, aimingPoint.latitude, aimingPoint.altitude); //aiming point
    Vector3D iPoint(intersectionPoint.longitude, intersectionPoint.latitude, intersectionPoint.altitude); //intersection point
    Vector3D sPoint(stoppingPoint.longitude, stoppingPoint.latitude, stoppingPoint.altitude); //stopping point
    Vector3D cPoint(input.longitude, input.latitude, input.altitude); //current point
    Vector3D projectedPoint;

    //calculating the vectors
    Vector3D vectorAI(iPoint.x - aPoint.x, iPoint.y - aPoint.y, iPoint.z - aPoint.z);
    Vector3D vectorAS(sPoint.x - aPoint.x, sPoint.y - aPoint.y, sPoint.z - aPoint.z);
    Vector3D vectorAC(cPoint.x - aPoint.x, cPoint.y - aPoint.y, cPoint.z - aPoint.z);
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
    double altitude = (((projectedPoint.x - aPoint.x) / vectorAI.x) + aPoint.z) * vectorAI.z;

    return altitude;
}

double LandingManager::throttleOff(void)
{
    //return 0 for now, this function needs to be fixed
    return 0;
}

double LandingManager::approachSpeed(double windSpeed, bool ifPackage)
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

double LandingManager::slowFlightSpeed(bool ifPackage)
{
    //slow flight speed calculation for both package and no package scenarios
    if(ifPackage)
    {
        return STALL_SPEED_WITH_PACKAGE + 2;
    }
    else 
    {
        return STALL_SPEED_NO_PACKAGE + 2;
    }
}

_LandingPath LandingManager::createSlopeWaypoints(Telemetry_PIGO_t input, double currentAltitude)
{
    //created comments for testing
    _LandingPath path;
    path.stoppingPoint.latitude = input.stoppingLatitude; //0
    path.stoppingPoint.longitude = input.stoppingLongitude; //0
    path.stoppingPoint.altitude = input.stoppingAltitude; //0

    //creating points from stopping point to aiming point 
    
    //setting Z of aiming point
    path.aimingPoint.altitude = input.stoppingAltitude; //0
    
    //determining x and y of aiming point
    double radianDirection = input.stoppingDirectionHeading * PI / 180.0; //PI/4

    //finding the x and y components of the rolling distance vector
    double stoppingDistX = sin(radianDirection) * DISTANCE_OF_LANDING; //5root2
    double stoppingDistY = cos(radianDirection) * DISTANCE_OF_LANDING; //5root2

    //converting into x and y components in lat and lon
    double metersPerDegLon = 40075000.0 * cos(input.stoppingLatitude*PI/180)/360.0; //111251.6316
    double stoppingDistLon = stoppingDistX / metersPerDegLon; //6.355922792e-5
    double stoppingDistLat = stoppingDistY / METERS_PER_DEG_LAT; //6.352019235e-5

 

    //subtracting the distances from stopping point to get aiming point
    path.aimingPoint.longitude = input.stoppingLongitude - stoppingDistLon;//1-6.355922792e-5
    path.aimingPoint.latitude = input.stoppingLatitude - stoppingDistLat; //2-6.352019235e-5
    
    //calculating the intersection point

    //determining altitude of intersection
    path.intersectionPoint.altitude = currentAltitude; //100

    //determining the horizontal distance of intersection
    double horizDist = (currentAltitude - path.aimingPoint.altitude) / tan(ANGLE_OF_LANDING * PI / 180.0); //altitude in meters //1108.715073

    //finding the x and y components of the horizDist vector
    double slopeDistX = sin(radianDirection) * horizDist; //783.9799468
    double slopeDistY = cos(radianDirection) * horizDist; //783.9799468

    //converting x and y components 
    double slopeDistLon = slopeDistX / metersPerDegLon;//7.046907407e-3 //same conversion factor used here because we are flying at a relatively small scale. Lat adjustments will affect it very minimally
    double slopeDistLat = slopeDistY / METERS_PER_DEG_LAT;//7.042579472e-3

    //subtracting the distances from aiming point to get intersection point
    path.intersectionPoint.longitude = path.aimingPoint.longitude - slopeDistLon; //1-6.355922792e-5 - 7.046907407e-3 
    path.intersectionPoint.latitude = path.aimingPoint.latitude - slopeDistLat; //2-6.352019235e-5 - 7.042579472e-3


    return path;
}
