#include "landingTakeoffManager.hpp"
#include "vectorClass.hpp"

#include <math.h>
#include "MathConstants.hpp"

double LandingTakeoffManager::changingAltitude(const SFOutput_t & input, const _PathData & aimingPoint, const _PathData & intersectionPoint, const _PathData & stoppingPoint)
{
    //vector declaration
    Vector3D aPoint(aimingPoint.longitude, aimingPoint.latitude, aimingPoint.altitude); //aiming point
    Vector3D iPoint(intersectionPoint.longitude, intersectionPoint.latitude, intersectionPoint.altitude); //intersection point
    Vector3D sPoint(stoppingPoint.longitude, stoppingPoint.latitude, stoppingPoint.altitude); //stopping point
    Vector3D cPoint(input.longitude, input.latitude, input.altitude); //current point
    Vector3D projectedPoint;

    //calculating the vectors
    Vector3D vectorAI(iPoint.x - aPoint.x, iPoint.y - aPoint.y, iPoint.z - aPoint.z); //aiming point --> intersection point
    Vector3D vectorAS(sPoint.x - aPoint.x, sPoint.y - aPoint.y, sPoint.z - aPoint.z); //aiming point --> stopping point

    Vector3D vectorAC(cPoint.x - aPoint.x, cPoint.y - aPoint.y, cPoint.z - aPoint.z); //aiming point --> current point
    Vector3D normalVector;
    
    //normal of the plane created by aiming, stopping, and intersection point
    normalVector = vectorAI.crossProduct(vectorAS);

    //projecting vectorAC to the normal
    //equation is the dot product of normal and vectorAC, divided by the norm of the normal squared, multiplied by the normal vector
    Vector3D projectionACToNormal;

    projectionACToNormal = normalVector * (normalVector.dotProduct(vectorAC) / pow(normalVector.norm(),2));

    //project point onto plane (cpoint minus the projection)

    projectedPoint = cPoint - projectionACToNormal;

    //determine the equation of the line of the slope and sub in projected values (symmetric form)
    double altitude = (((projectedPoint.x - aPoint.x) / vectorAI.x) + aPoint.z) * vectorAI.z;

    return altitude;
}

double LandingTakeoffManager::approachSpeed(bool ifPackage)
{
    //approach speed calculation for both package and no package scenarios
    if(ifPackage)
    {
        return STALL_SPEED_WITH_PACKAGE * 1.3;
    }
    else 
    {
        return STALL_SPEED_NO_PACKAGE * 1.3;
    }
}

double LandingTakeoffManager::slowFlightSpeed(bool ifPackage)
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

_LandingPath LandingTakeoffManager::createSlopeWaypoints(const Telemetry_PIGO_t & input, float currentAltitude)
{
    _LandingPath path;
    path.stoppingPoint.latitude = input.stoppingLatitude; 
    path.stoppingPoint.longitude = input.stoppingLongitude; 
    path.stoppingPoint.altitude = input.stoppingAltitude; 

    //creating points from stopping point to aiming point 
    
    //setting Z of aiming point
    path.aimingPoint.altitude = input.stoppingAltitude; 
    
    //determining x and y of aiming point
    double radianDirection = DEG_TO_RAD(input.stoppingDirectionHeading); 

    //finding the x and y components of the rolling distance vector
    double stoppingDistX = sin(radianDirection) * DISTANCE_OF_LANDING; 
    double stoppingDistY = cos(radianDirection) * DISTANCE_OF_LANDING;

    //converting into x and y components in lat and lon
    double metersPerDegLon = LATITUDE_LONGITUDE_CONVERSION_CONSTANT * cos(DEG_TO_RAD(input.stoppingLatitude))/360.0; 
    double stoppingDistLon = stoppingDistX / metersPerDegLon; 
    double stoppingDistLat = stoppingDistY / METERS_PER_DEG_LAT; 

    //subtracting the distances from stopping point to get aiming point
    path.aimingPoint.longitude = input.stoppingLongitude - stoppingDistLon;
    path.aimingPoint.latitude = input.stoppingLatitude - stoppingDistLat; 
    
    //calculating the intersection point

    //determining altitude of intersection
    path.intersectionPoint.altitude = currentAltitude; 

    //determining the horizontal distance of intersection
    double horizDist = (currentAltitude - path.aimingPoint.altitude) / tan(DEG_TO_RAD(ANGLE_OF_LANDING));

    //finding the x and y components of the horizDist vector
    double slopeDistX = sin(radianDirection) * horizDist; 
    double slopeDistY = cos(radianDirection) * horizDist; 

    //converting x and y components 
    double slopeDistLon = slopeDistX / metersPerDegLon;
    double slopeDistLat = slopeDistY / METERS_PER_DEG_LAT;

    //subtracting the distances from aiming point to get intersection point
    path.intersectionPoint.longitude = path.aimingPoint.longitude - slopeDistLon;
    path.intersectionPoint.latitude = path.aimingPoint.latitude - slopeDistLat;


    return path;
}

_LandingTakeoffOutput LandingTakeoffManager::translateWaypointCommands(const _WaypointManager_Data_Out & outputData)
{
    _LandingTakeoffOutput controlOutput;
    controlOutput.controlDetails.rollPercent = 0;
    controlOutput.controlDetails.rudderPercent = 0;
    controlOutput.controlDetails.throttlePercent = 0;
    controlOutput.controlDetails.pitchPercent = 0;
    controlOutput.useHeading = false;
    controlOutput.controlDetails.rudderPassby = false;
    controlOutput.controlDetails.pitchPassby = false;
    controlOutput.controlDetails.rollPassby = false;
    controlOutput.controlDetails.throttlePassby = false;
    controlOutput.desiredTrack = outputData.desiredTrack;
    controlOutput.desiredAltitude = outputData.desiredAltitude;
    return controlOutput;
}

void LandingTakeoffManager::translateLTSFCommandsToCoordTurns(const _LandingTakeoffOutput & outputData, const SFOutput_t & sensorOutput, const IMU_Data_t & imuOutput, CoordinatedTurnInput_t & turnInput, AltitudeAirspeedInput_t & altitudeAirspeedInput)
{
    //loading in data depending on if heading needs to be used
    if(outputData.useHeading)
    {
        turnInput.desiredHeadingTrack = outputData.desiredHeading;
        turnInput.currentHeadingTrack = sensorOutput.heading;
    }
    else
    {
        turnInput.desiredHeadingTrack = outputData.desiredTrack;
        turnInput.currentHeadingTrack = sensorOutput.track;
    }
    turnInput.accY = imuOutput.accy;
    altitudeAirspeedInput.currentAltitude = sensorOutput.altitude;
    altitudeAirspeedInput.desiredAltitude = outputData.desiredAltitude;
    altitudeAirspeedInput.currentAirspeed = sensorOutput.airspeed;
    altitudeAirspeedInput.desiredAirspeed = outputData.desiredAirspeed;
}

double LandingTakeoffManager::desiredRotationSpeed(bool ifPackage)
{
    if(ifPackage)
    {
        return ROTATION_SPEED_WITH_PACKAGE;
    }
    else
    {
        return ROTATION_SPEED_NO_PACKAGE;
    }
}

double LandingTakeoffManager::desiredClimbSpeed(bool ifPackage)
{
    if(ifPackage)
    {
        return CLIMB_SPEED_WITH_PACKAGE;
    }
    else
    {
        return CLIMB_SPEED_NO_PACKAGE;
    }
}

_PathData LandingTakeoffManager::createTakeoffWaypoint(double currentLatitude, double currentLongitude, float currentAltitude, float takeoffDirection)
{
    _PathData desiredWaypoint;
    //retrieve radian direction of heading (starts from positive y axis)
    double radianDirection = DEG_TO_RAD(takeoffDirection);

    //retrieve the horizontal and vertical components of takeoff distance
    double takeoffDistX = sin(radianDirection) * DISTANCE_OF_TAKEOFF; 
    double takeoffDistY = cos(radianDirection) * DISTANCE_OF_TAKEOFF; 

    //convert horizontal and vertical components into latitude and longitude
    double metersPerDegLon = LATITUDE_LONGITUDE_CONVERSION_CONSTANT * cos(DEG_TO_RAD(currentLatitude)) /360.0; 
    double takeoffDistLon = takeoffDistX / metersPerDegLon; 
    double takeoffDistLat = takeoffDistY / METERS_PER_DEG_LAT; 

    //add components onto the current location to determine takeoff point
    desiredWaypoint.latitude = currentLatitude + takeoffDistLat;
    desiredWaypoint.longitude = currentLongitude + takeoffDistLon; 

    /*
    takeoff waypoint will have the same altitude as the current position, constant additional altitude 
    could be added in the future, but it is not needed as vertical component is already controlled by
    max throttle in the climb 
    */
    desiredWaypoint.altitude = currentAltitude;

    return desiredWaypoint;
}






