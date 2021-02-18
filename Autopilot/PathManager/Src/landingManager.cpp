#include "waypointManager.hpp"
#include "landingManager.hpp"
#include <cmath>

const double SET_THROTTLE_OFF = -1000;

double LandingManager::changingAltitude(_WaypointManager_Data_In * input, _PathData * aimingPoint, _PathData * intersectionPoint, _PathData * stoppingPoint)
{
    //arrays to hold the important points
    float aPoint[3]; //aming point
    float iPoint[3]; //intersection point
    float sPoint[3]; //stopping point
    float cPoint[3]; //current point
    float projectedPoint[3];

    //loading important points
    aPoint[0] = aimingPoint->latitude;
    aPoint[1] = aimingPoint->longitude;
    aPoint[2] = aimingPoint->altitude; 

    iPoint[0] = intersectionPoint->latitude;
    iPoint[1] = intersectionPoint->longitude;
    iPoint[2] = intersectionPoint->altitude;

    sPoint[0] = stoppingPoint->latitude;
    sPoint[1] = stoppingPoint->longitude;
    sPoint[2] = stoppingPoint->altitude;

    cPoint[0] = input->latitude;
    cPoint[1] = input->longitude;
    cPoint[2] = input->altitude;

    //calculating the vectors
    float vectorAI[3];
    float vectorAS[3];
    float vectorAC[3];
    float normal[3];

    //aiming point to intersection point
    vectorAI[0] = iPoint[0] - aPoint[0];
    vectorAI[1] = iPoint[1] - aPoint[1];
    vectorAI[2] = iPoint[2] - aPoint[2];

    //aiming point to stopping point
    vectorAS[0] = sPoint[0] - aPoint[0];
    vectorAS[1] = sPoint[1] - aPoint[1];
    vectorAS[2] = sPoint[2] - aPoint[2];

    //aiming point to current point
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
/*
double LandingManager::horizontalAdjustment(_WaypointManager_Data_In input, _PathData * aimingPoint, _PathData * intersectionPoint)
{
    //determine path and location
    //using follow waypoints algorithm, output the desired horizontal track
}
*/
double LandingManager::throttleOff()
{
    //return an altitude so that the throttle turns off
    return SET_THROTTLE_OFF;
}

double LandingManager::alignHeading(_PathData * aimingPoint, _PathData * intersectionPoint)
{
    float aPoint[2];
    float iPoint[2];
    float dVector[2];
    double heading;

    aPoint[0] = aimingPoint->latitude;
    aPoint[1] = aimingPoint->longitude;

    iPoint[0] = intersectionPoint->latitude;
    iPoint[1] = intersectionPoint->longitude;

    //determine current heading and direction of landing
    dVector[0] = aPoint[0] - iPoint[0];
    dVector[1] = aPoint[1] - iPoint[1];

    if(dVector[0] >= 0 && dVector[1] > 0) //quadrant 1
    {
        heading = atan(dVector[0] / dVector[1]) * 180 / M_PI;
    }
    else if(dVector[0] < 0 && dVector[1] > 0) //quadrant 2
    {
        heading = atan(dVector[0] / dVector[1]) * 180 / M_PI + 360;
    }
    else if(dVector[0] <= 0 && dVector[1] < 0) //quadrant 3
    {
        heading = atan(dVector[0] / dVector[1]) * 180 / M_PI + 180;
    }
    else if(dVector[0] > 0 && dVector[1] < 0) //quadrant 4
    {
        heading = atan(dVector[0] / dVector[1]) * 180 / M_PI + 180;
    }
    else if(dVector[1] == 0)
    {
        if(dVector[0] > 0)
        {
            heading = 90;
        }
        else if (dVector[0] < 0)
        {
            heading = 270;
        }
        else
        {
            heading = 0; //error, the points have the same coordinates, just head north
        }
    }

    //return desired heading
    return heading;
}

void LandingManager::createSlopeWaypoints(double stoppingLatitude, double stoppingLongitude, double stoppingAltitude, double directionLanding)
{
    //load in the GPS locations of the important points
    //setup waypoints
}