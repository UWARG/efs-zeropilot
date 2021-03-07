#include "pathStateClasses.hpp"

void landingTransitionStage::execute(pathManager* pathMgr)
{
    if(!pathMgr->madeLandingPoints)
    {
        //make landing points from landing data
        //requires data structure that dhruv wants to use (waiting for data in)
        path = LandingManager::createSlopeWaypoints(getFromTelemetry::telemetryInput);
        pathArray[0] = &path.intersectionPoint;
        pathArray[1] = &path.aimingPoint;
        pathArray[2] = &path.stoppingPoint;
        //creating the intersection point, requires initialization of waypoint manager (data in required) (current location needs to be passed in from sensorfusion?)
        waypointStatus = landingPath.initialize_flight_path(pathArray, 3, );
        //set made madelandingPoints to true
        pathMgr->madeLandingPoints = true;
        //follow the landing waypoints (requires data in)
        waypointStatus = landingPath.get_next_directions(getFromTelemetry::telemetryInput, &cruisingState::_outputdata);
    }
    else
    {
        //follow the landing waypoints(requires data in)
        waypointStatus = landingPath.get_next_directions(getFromTelemetry::telemetryInput, &cruisingState::_outputdata);
    }

    differenceInHeading1 = getFromTelemetry::telemetryInput.landingDirection - sensorFusion::input.track;
    differenceInHeading2 = sensorFusion::input.track - getFromTelemetry::telemetryInput.landingDirection;
    if(differenceInHeading1<0){differenceInHeading1+=360;}
    if(differenceInHeading2<0){differenceInHeading2+=360;}
    if(differenceInHeading1<differenceInHeading2)
    {
        if(fabs(differenceInHeading1)<=5)
        {
            //set enum to slope state
            pathMgr->stage = SLOPE;
        }
    }
    else
    {
        if(fabs(differenceInHeading2)<=5)
        {
            //set enum to slope state
            pathMgr->stage = SLOPE;
        }
    }
    
    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& landingTransitionStage::getInstance()
{
    static landingTransitionStage singleton;
    return singleton;
}