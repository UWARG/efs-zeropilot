#include "pathStateClasses.hpp"

void commsWithAttitude::execute(pathManager* pathMgr)
{
    //initial mode
    pathMgr -> setState(getFromTelemetry::getInstance());
}

pathManagerState& commsWithAttitude::getInstance()
{
    static commsWithAttitude singleton;
    return singleton;
}

void getFromTelemetry::execute(pathManager* pathMgr)
{
    //communicate with telemetry
    //retrieve landing data
    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(getSensorData::getInstance());
    }
}

pathManagerState& getFromTelemetry::getInstance()
{
    static getFromTelemetry singleton;
    return singleton;
}

void getSensorData::execute(pathManager* pathMgr)
{
    //obtain sensor data
    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(sensorFusion::getInstance());
    }
}

pathManagerState& getSensorData::getInstance()
{
    static getSensorData singleton;
    return singleton;
}

void sensorFusion::execute(pathManager* pathMgr)
{
    //fuse sensor data
    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(sensorFusion::getInstance());
    }
}

pathManagerState& sensorFusion::getInstance()
{
    static sensorFusion singleton;
    return singleton;
}

void resetVariables::execute(pathManager* pathMgr)
{   
    //resetting the variables for passby
    resetPassby(&cruisingState::_outputdata.controlDetails);

    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    //if the enums for landing state, set to each landing state
    switch(pathMgr->stage){
        case TRANSITION:
            pathMgr->setState(landingTransitionStage::getInstance());
            break;
        case SLOPE:
            pathMgr->setState(landingSlopeStage::getInstance());
            break;
        case FLARE:
            pathMgr->setState(landingFlareStage::getInstance());
            break;
        case DECRAB:
            pathMgr->setState(landingDecrabStage::getInstance());
            break;
        case TOUCHDOWN:
            pathMgr->setState(landingTouchdownStage::getInstance());
            break;
        case CRUISING:
            pathMgr->setState(cruisingState::getInstance());
            break;
        case ROLL:
            pathMgr->setState(takeoffRollStage::getInstance());
            break;
        case CLIMB:
            pathMgr->setState(takeoffClimbStage::getInstance());
            break;
        default:
            pathMgr->setState(cruisingState::getInstance());
    }
}

pathManagerState& resetVariables::getInstance()
{
    static resetVariables singleton;
    return singleton;
}

void cruisingState::execute(pathManager* pathMgr)
{
    //waypoint manager stuff
    if(pathMgr->isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& cruisingState::getInstance()
{
    static cruisingState singleton;
    return singleton;
}


void coordinateTurnElevation::execute(pathManager* pathMgr)
{
    //get elevation and turning data
    if(pathMgr->isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(commsWithAttitude::getInstance());
    }
}

pathManagerState& coordinateTurnElevation::getInstance()
{
    static coordinateTurnElevation singleton;
    return singleton;
}

void fatalFailureMode::execute(pathManager* pathMgr)
{
    pathMgr->setState(fatalFailureMode::getInstance());
}

pathManagerState& fatalFailureMode::getInstance()
{
    static fatalFailureMode singleton;
    return singleton;
}

/****************************************************************************************************

LANDING STATE FUNCTIONS

****************************************************************************************************/

void landingTransitionStage::execute(pathManager* pathMgr)
{
    if(!pathMgr->madeLandingPoints)
    {
        //requires data structure that dhruv wants to use 
        landingPath = WaypointManager(0.0,0.0);
        path = LandingManager::createSlopeWaypoints(getFromTelemetry::telemetryInput, sensorFusion::sensorInput.altitude);

        //creating waypoints 
        pathArray[0] = landingPath.initialize_waypoint(path.intersectionPoint.longitude, path.intersectionPoint.latitude, path.intersectionPoint.altitude, PATH_FOLLOW);
        pathArray[1] = landingPath.initialize_waypoint(path.aimingPoint.longitude, path.aimingPoint.latitude, path.aimingPoint.altitude, PATH_FOLLOW);
        pathArray[2] = landingPath.initialize_waypoint(path.stoppingPoint.longitude, path.stoppingPoint.latitude, path.stoppingPoint.altitude, PATH_FOLLOW);
        currentLocation = landingPath.initialize_waypoint(sensorFusion::sensorInput.longitude, sensorFusion::sensorInput.latitude, sensorFusion::sensorInput.altitude, HOLD_WAYPOINT, 20); //fill in with sensor fusion data
        //initializing flight path
        waypointStatus = landingPath.initialize_flight_path(pathArray, 3, currentLocation);
        
        //set made madelandingPoints to true
        pathMgr->madeLandingPoints = true;
    }

    //follow the landing waypoints
    waypointStatus = landingPath.get_next_directions(sensorFusion::sensorInput, &cruisingState::_outputdata);

    //calculating the difference in heading to detect if finished turning (2 differences in heading possible)
    differenceInHeading1 = getFromTelemetry::telemetryInput.stoppingDirectionHeading - sensorFusion::sensorInput.track;
    differenceInHeading2 = sensorFusion::sensorInput.track - getFromTelemetry::telemetryInput.stoppingDirectionHeading;

    //making sure both headings are positive
    if(differenceInHeading1 < 0){differenceInHeading1 += 360;}
    if(differenceInHeading2 < 0){differenceInHeading2 += 360;}

    //if the smaller heading is less than 5 degrees, set stage to slope


    if(differenceInHeading1 < differenceInHeading2 && fabs(differenceInHeading1) <= 5)
    {
        pathMgr->stage = SLOPE;
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

void landingSlopeStage::execute(pathManager* pathMgr)
{
    if(sensorFusion::sensorInput.altitude <= (FLARE_ALTITUDE+getFromTelemetry::telemetryInput.stoppingAltitude)) //if less than flare altitude
    {
        pathMgr->stage = FLARE;

    }
    else
    {
        //aligning horizontal position using waypointManager get_next_directions
        landingTransitionStage::waypointStatus = landingTransitionStage::landingPath.get_next_directions(sensorFusion::sensorInput, &cruisingState::_outputdata);
        //retrieving desired altitude for slope state and setting it 
        cruisingState::_outputdata.desiredAltitude = LandingManager::changingAltitude(sensorFusion::sensorInput,landingTransitionStage::path.aimingPoint, landingTransitionStage::path.intersectionPoint, landingTransitionStage::path.stoppingPoint);
        //retrieving desired speed for approach speed and setting it
        cruisingState::_outputdata.desiredSpeed = LandingManager::approachSpeed(getFromTelemetry::telemetryInput.windSpeed, getFromTelemetry::telemetryInput.packageOnBoard);
    }

    if(landingTransitionStage::waypointStatus == INVALID_PARAMETERS)
    {
        pathMgr -> isError = true;
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

pathManagerState& landingSlopeStage::getInstance()
{
    static landingSlopeStage singleton;
    return singleton;
}

void landingFlareStage::execute(pathManager* pathMgr)
{
    if(sensorFusion::sensorInput.altitude <= (DECRAB_ALTITUDE + getFromTelemetry::telemetryInput.stoppingAltitude)) //altitude is below 70 cm
    {
        pathMgr->stage = DECRAB;
    }
    else
    {
        //maintaining horizontal position
        landingTransitionStage::waypointStatus = landingTransitionStage::landingPath.get_next_directions(sensorFusion::sensorInput, &cruisingState::_outputdata);
        
        //throttle off
        cruisingState::_outputdata.controlDetails.throttlePassby = true;
        cruisingState::_outputdata.controlDetails.throttlePercent = 0;

        //maintaing speed for flare attitude
        cruisingState::_outputdata.desiredSpeed = LandingManager::slowFlightSpeed(getFromTelemetry::telemetryInput.packageOnBoard);
    }   

    if(landingTransitionStage::waypointStatus == INVALID_PARAMETERS)
    {
        pathMgr -> isError = true;
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

pathManagerState& landingFlareStage::getInstance()
{
    static landingFlareStage singleton;
    return singleton;
}

void landingDecrabStage::execute(pathManager* pathMgr)
{
    if(sensorFusion::sensorInput.altitude <= (TOUCHDOWN_ALTITUDE + getFromTelemetry::telemetryInput.stoppingAltitude)) //altitude is 5 cm or less/ultrasonic sensor sensed 5cm or less
    {
        pathMgr->stage = TOUCHDOWN;
    }
    else
    {
        //align heading with landing direction
        cruisingState::_outputdata.desiredHeading = getFromTelemetry::telemetryInput.stoppingDirectionHeading;
        //retrieving desired slow flight speed
        cruisingState::_outputdata.desiredSpeed = LandingManager::slowFlightSpeed(getFromTelemetry::telemetryInput.packageOnBoard);
        
        //throttle off
        cruisingState::_outputdata.controlDetails.throttlePassby = true;
        cruisingState::_outputdata.controlDetails.throttlePercent = 0;
    }

    if(landingTransitionStage::waypointStatus == INVALID_PARAMETERS)
    {
        pathMgr -> isError = true;
    }
    if(pathMgr -> isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& landingDecrabStage::getInstance()
{
    static landingDecrabStage singleton;
    return singleton;
}

void landingTouchdownStage::execute(pathManager* pathMgr)
{
    //throttle off
    cruisingState::_outputdata.controlDetails.throttlePassby = true;
    cruisingState::_outputdata.controlDetails.throttlePercent = 0;

    //aligning heading
    cruisingState::_outputdata.desiredHeading = getFromTelemetry::telemetryInput.stoppingDirectionHeading;
    if(pathMgr -> isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& landingTouchdownStage::getInstance()
{
    static landingTouchdownStage singleton;
    return singleton;
}


/****************************************************************************************************

TAKEOFF STATE FUNCTIONS

****************************************************************************************************/

void takeoffRollStage::execute(pathManager* pathMgr)
{
    //max throttle
    cruisingState::_outputdata.controlDetails.throttlePassby = true;
    cruisingState::_outputdata.controlDetails.throttlePercent = 100;

    if(!pathMgr->madeTakeoffPoints)
    {
        takeoffPath = WaypointManager(0.0,0.0);
        takeoffPoint = TakeoffManager::createTakeoffWaypoint(sensorFusion::sensorInput.latitude,sensorFusion::sensorInput.longitude, sensorFusion::sensorInput.altitude, getFromTelemetry::telemetryInput.takeoffDirectionHeading);
        pathArray[0] = takeoffPath.initialize_waypoint(takeoffPoint.longitude, takeoffPoint.latitude, takeoffPoint.altitude, PATH_FOLLOW);
        currentLocation = takeoffPath.initialize_waypoint(sensorFusion::sensorInput.longitude, sensorFusion::sensorInput.latitude, sensorFusion::sensorInput.altitude, HOLD_WAYPOINT);
        waypointStatus = takeoffPath.initialize_flight_path(pathArray, 1, currentLocation);

        pathMgr->madeTakeoffPoints = true;
    }

    if(sensorFusion::currentAirspeed>(TakeoffManager::desiredRotationSpeed(getFromTelemetry::telemetryInput.windSpeed,pathMgr->isPackage)))
    {
        pathMgr->stage = CLIMB;
    }
}

pathManagerState& takeoffRollStage::getInstance()
{
    static takeoffRollStage singleton;
    return singleton;
}

void takeoffClimbStage::execute(pathManager* pathMgr)
{
    if(sensorFusion::sensorInput.altitude>(takeoffRollStage::takeoffPoint.altitude + EXIT_TAKEOFF_ALTITUDE))
    {
        pathMgr->stage = CRUISING;
    }
    else
    {   
        takeoffRollStage::waypointStatus = takeoffRollStage::takeoffPath.get_next_directions(sensorFusion::sensorInput, &cruisingState::_outputdata);
        cruisingState::_outputdata.desiredSpeed = TakeoffManager::desiredClimbSpeed(getFromTelemetry::telemetryInput.windSpeed,pathMgr->isPackage);
        
        //maxThrottle
        cruisingState::_outputdata.controlDetails.throttlePassby = true;
        cruisingState::_outputdata.controlDetails.throttlePercent = 80;
    }
}

pathManagerState& takeoffClimbStage::getInstance()
{
    static takeoffClimbStage singleton;
    return singleton;
}














