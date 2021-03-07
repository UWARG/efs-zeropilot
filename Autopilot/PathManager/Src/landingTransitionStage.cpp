#include "pathStateClasses.hpp"

void landingTransitionStage::execute(pathManager* pathMgr)
{
    if(!pathMgr -> madeLandingPoints)
    {
        //requires data structure that dhruv wants to use 
        path = LandingManager::createSlopeWaypoints(getFromTelemetry::telemetryInput);

        //creating waypoints 
        pathArray[0] = landingPath.initialize_waypoint(path.intersectionPoint.longitude, path.intersectionPoint.latitude, path.intersectionPoint.altitude, PATH_FOLLOW);
        pathArray[1] = landingPath.initialize_waypoint(path.aimingPoint.longitude, path.aimingPoint.latitude, path.aimingPoint.altitude, PATH_FOLLOW);
        pathArray[2] = landingPath.initialize_waypoint(path.stoppingPoint.longitude, path.stoppingPoint.latitude, path.stoppingPoint.altitude, PATH_FOLLOW);
       
        //initializing flight path, QUESTION!!! what should I do with currentPosition parameter? its a PathData struct, but it requires info from sensorfusion
        waypointStatus = landingPath.initialize_flight_path(pathArray, 3,      );
        
        //set made madelandingPoints to true
        pathMgr->madeLandingPoints = true;
    }

    //follow the landing waypoints
    waypointStatus = landingPath.get_next_directions(getFromTelemetry::telemetryInput, &cruisingState::_outputdata);

    //calculating the difference in heading to detect if finished turning (2 differences in heading possible)
    differenceInHeading1 = getFromTelemetry::telemetryInput.landingDirection - sensorFusion::input.track;
    differenceInHeading2 = sensorFusion::input.track - getFromTelemetry::telemetryInput.landingDirection;

    //making sure both headings are positive
    if(differenceInHeading1 < 0){differenceInHeading1 += 360;}
    if(differenceInHeading2 < 0){differenceInHeading2 += 360;}

    //if the smaller heading is less than 5 degrees, set stage to slope
    if(differenceInHeading1<differenceInHeading2)
    {
        if(fabs(differenceInHeading1) <= 5)
        {
            //set enum to slope state
            pathMgr->stage = SLOPE;
        }
    }
    else
    {
        if(fabs(differenceInHeading2) <= 5)
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