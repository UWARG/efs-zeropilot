#include "pathStateClasses.hpp"

/***********************************************************************************************************************
 * Static Member Variable Declarations
 **********************************************************************************************************************/

Telemetry_PIGO_t commsWithTelemetry::_incomingData;
_CruisingState_Telemetry_Return cruisingState::_returnToGround;
_WaypointManager_Data_In cruisingState::_inputdata;
_WaypointManager_Data_Out cruisingState::_outputdata; 
SFOutput_t sensorFusion::_sfOutputData;
IMU_Data_t sensorFusion::_imudata;
CoordinatedTurnAttitudeManagerCommands_t coordinateTurnElevation::_rollandrudder;
AltitudeAirspeedCommands_t coordinateTurnElevation::_pitchandairspeed; 
AttitudeData commsWithAttitude::_receivedData;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void commsWithAttitude::execute(pathManager* pathMgr)
{
    
    bool newDataAvailable = GetAttitudeData(&_receivedData); // Gets attitude manager data

    // Gets data used to populate CommandsForAM struct
    CoordinatedTurnAttitudeManagerCommands_t * turnCommands = coordinateTurnElevation::GetRollAndRudder();
    AltitudeAirspeedCommands_t * altCommands = coordinateTurnElevation::GetPitchAndAirspeed();

    CommandsForAM toSend {};
    toSend.roll = turnCommands->requiredRoll;
    toSend.pitch = altCommands->requiredPitch;
    toSend.rudderPercent = turnCommands->requiredRudderPosition;
    toSend.throttlePercent = altCommands->requiredThrottlePercent;

    SendCommandsForAM(&toSend); // Sends commands to attitude manager

    pathMgr->setState(commsWithTelemetry::getInstance());
}

pathManagerState& commsWithAttitude::getInstance()
{
    static commsWithAttitude singleton;
    return singleton;
}

void commsWithTelemetry::execute(pathManager* pathMgr)
{
    // Get data from telemetry

    // Do any processing required (update struct that contains telemetry data and process it)

    // Store data inside of the Telemetry_PIGO_t struct that is a parameter of this child class
    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(sensorFusion::getInstance());
    }
}

pathManagerState& commsWithTelemetry::getInstance()
{
    static commsWithTelemetry singleton;
    return singleton;
}

void sensorFusion::execute(pathManager* pathMgr)
{
    SFError_t error = SF_GetResult(&_sfOutputData); // Gets current Sensor fusion output struct
    _imudata = SF_GetRawIMU();

    if(pathMgr->isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
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

    Telemetry_PIGO_t * telemetryData = commsWithTelemetry::GetTelemetryIncomingData(); // Get struct from telemetry state with all of the commands and values.
    SFOutput_t * sensFusionOutput = sensorFusion::GetSFOutput(); // Get sensor fusion data
    
    // Set waypoint manager input struct
    _inputdata.track = sensFusionOutput->track; // Gets track
    _inputdata.longitude = sensFusionOutput->longitude;
    _inputdata.latitude = sensFusionOutput->latitude;
    _inputdata.altitude = sensFusionOutput->altitude;

    // Call module functions
    _ModifyFlightPathErrorCode editError = editFlightPath(telemetryData, cruisingStateManager, waypointIDArray); // Edit flight path if applicable
    _GetNextDirectionsErrorCode pathError = pathFollow(telemetryData, cruisingStateManager, _inputdata, &_outputdata, goingHome, inHold); // Get next direction or modify flight behaviour pattern
    setReturnValues(&_returnToGround, cruisingStateManager, editError, pathError); // Set error codes

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
    
    SFOutput_t * sensFusionOutput = sensorFusion::GetSFOutput(); // Get sensor fusion data
    IMU_Data_t * imudata = sensorFusion::GetIMUData(); // Gets raw IMU data
    
    CoordinatedTurnInput_t turnInput {};
    AltitudeAirspeedInput_t altAirspeedInput {};                                   

    
    //get elevation and turning data
    //loading in commands data from each of the states
    switch(pathMgr->stage){
        case TRANSITION:
            LandingManager::translateLTSFCommandsToCoordTurns(landingTransitionStage::getControlOutput(), sensFusionOutput, &turnInput, &altAirspeedInput);
            break;
        case SLOPE:
            LandingManager::translateLTSFCommandsToCoordTurns(landingSlopeStage::getControlOutput(), sensFusionOutput, &turnInput, &altAirspeedInput);
            break;
        case FLARE:
            LandingManager::translateLTSFCommandsToCoordTurns(landingFlareStage::getControlOutput(), sensFusionOutput, &turnInput, &altAirspeedInput);
            break;
        case DECRAB:
            LandingManager::translateLTSFCommandsToCoordTurns(landingDecrabStage::getControlOutput(), sensFusionOutput, &turnInput, &altAirspeedInput);
            break;
        case TOUCHDOWN:
            LandingManager::translateLTSFCommandsToCoordTurns(landingTouchdownStage::getControlOutput(), sensFusionOutput, &turnInput, &altAirspeedInput);
            break;
        case CRUISING:
            _WaypointManager_Data_Out * waypointOutput = cruisingState::GetOutputData(); // Get output data from waypoint manager
        
            turnInput.currentHeadingTrack = sensFusionOutput->track; // Gets track;
            turnInput.desiredHeadingTrack = waypointOutput->desiredTrack;
            turnInput.accY = imudata->accy;
            
            // Set up the compute altitude and airspeed function input
            altAirspeedInput.currentAltitude = sensFusionOutput->altitude;
            altAirspeedInput.desiredAltitude = waypointOutput->desiredAltitude;
            altAirspeedInput.currentAirspeed = sensFusionOutput->airspeed;
            altAirspeedInput.desiredAirspeed = waypointOutput->desiredAirspeed;   
            break;
        case ROLL:
            LandingManager::translateLTSFCommandsToCoordTurns(takeoffRollStage::getControlOutput(), sensFusionOutput, &turnInput, &altAirspeedInput);
            break;
        case CLIMB:
            LandingManager::translateLTSFCommandsToCoordTurns(takeoffClimbStage::getControlOutput(), sensFusionOutput, &turnInput, &altAirspeedInput);
            break;
        default:
            //should default to cruising
            _WaypointManager_Data_Out * waypointOutput = cruisingState::GetOutputData(); // Get output data from waypoint manager
        
            turnInput.currentHeadingTrack = sensFusionOutput->track; // Gets track;
            turnInput.desiredHeadingTrack = waypointOutput->desiredTrack;
            turnInput.accY = imudata->accy;
            
            // Set up the compute altitude and airspeed function input
            altAirspeedInput.currentAltitude = sensFusionOutput->altitude;
            altAirspeedInput.desiredAltitude = waypointOutput->desiredAltitude;
            altAirspeedInput.currentAirspeed = sensFusionOutput->airspeed;
            altAirspeedInput.desiredAirspeed = waypointOutput->desiredAirspeed; 
            
    }

    // Call module functions
    AutoSteer_ComputeCoordinatedTurn(&turnInput, &_rollandrudder);
    AutoSteer_ComputeAltitudeAndAirspeed(&altAirspeedInput, &_pitchandairspeed);
    
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
    //load in sensor fusion data and telemtry data into input structure
    input.telemetryData = ;//some get from telemetry command that returns telemetryPIGO
    input.sensorOutput = *(sensorFusion::GetSFOutput());

    //making sure landing points are only made once 
    if(!pathMgr->madeLandingPoints)
    {
        //requires data structure that dhruv wants to use 
        path = LandingManager::createSlopeWaypoints(getFromTelemetry::telemetryInput, sensorFusion::sensorInput.altitude);

        //creating waypoints 
        pathArray[0] = landingPath.initialize_waypoint(path.intersectionPoint.longitude, path.intersectionPoint.latitude, path.intersectionPoint.altitude, PATH_FOLLOW);
        pathArray[1] = landingPath.initialize_waypoint(path.aimingPoint.longitude, path.aimingPoint.latitude, path.aimingPoint.altitude, PATH_FOLLOW);
        pathArray[2] = landingPath.initialize_waypoint(path.stoppingPoint.longitude, path.stoppingPoint.latitude, path.stoppingPoint.altitude, PATH_FOLLOW);
        currentLocation = landingPath.initialize_waypoint(input.sensorOutput.longitude, input.sensorOutput.latitude, input.sensorOutput.altitude, HOLD_WAYPOINT, 20); //fill in with sensor fusion data
        //initializing flight path
        waypointStatus = landingPath.initialize_flight_path(pathArray, 3, currentLocation);
        
        //set made madelandingPoints to true
        pathMgr->madeLandingPoints = true;
    }

    //follow the landing waypoints
    waypointStatus = landingPath.get_next_directions(input.sensorOutput, &waypointOutput);

    //translate waypoint commands into landing output structure
    output = LandingManager::translateWaypointCommands(waypointOutput);

    //calculating the difference in heading to detect if finished turning (2 differences possible)
    differenceInHeading1 = input.telemetryData.stoppingDirectionHeading - input.sensorOutput.track;
    differenceInHeading2 = input.sensorOutput.track - input.telemetryData.stoppingDirectionHeading;

    //making sure both headings are positive
    if(differenceInHeading1 < 0){differenceInHeading1 += 360;}
    if(differenceInHeading2 < 0){differenceInHeading2 += 360;}

    //if the smaller heading is less than 5 degrees, set stage to slope


    if(differenceInHeading1 < differenceInHeading2 && fabs(differenceInHeading1) <= 5)
    {
        pathMgr->stage = SLOPE;
    }
    else if(fabs(differenceInHeading2) <= 5)
    {
        pathMgr->stage = SLOPE;
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
    //load in sensor fusion data and telemtry data into input structure
    input.telemetryData = ;//some get from telemetry command that returns telemetryPIGO
    input.sensorOutput = *(sensorFusion::GetSFOutput());

    if(input.sensorOutput.altitude <= (FLARE_ALTITUDE + input.telemetryData.stoppingAltitude)) //if less than flare altitude
    {
        pathMgr->stage = FLARE;

    }
    else
    {
        //aligning horizontal position using waypointManager get_next_directions
        landingTransitionStage::waypointStatus = landingTransitionStage::landingPath.get_next_directions(sensorFusion::sensorInput, &waypointOutput);
        //translate waypoint ouput data into landing data
        output = LandingManager::translateWaypointCommands(waypointOutput);
        //retrieving desired altitude for slope state and setting it 
        output.desiredAltitude = LandingManager::changingAltitude(input.sensorOutput,landingTransitionStage::path.aimingPoint, landingTransitionStage::path.intersectionPoint, landingTransitionStage::path.stoppingPoint);
        //retrieving desired speed for approach speed and setting it
        output.desiredAirspeed = LandingManager::approachSpeed(pathMgr->isPackage);
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
    //load in sensor fusion data and telemtry data into input structure
    input.telemetryData = ;//some get from telemetry command that returns telemetryPIGO
    input.sensorOutput = *(sensorFusion::GetSFOutput());

    if(input.sensorOutput.altitude <= (DECRAB_ALTITUDE + input.telemetryData.stoppingAltitude)) //altitude is below 70 cm
    {
        pathMgr->stage = DECRAB;
    }
    else
    {
        //maintaining horizontal position
        landingTransitionStage::waypointStatus = landingTransitionStage::landingPath.get_next_directions(sensorFusion::sensorInput, &waypointOutput);
        
        //translating waypoint data into landing data
        output = LandingManager::translateWaypointCommands(waypointOutput);

        //throttle off
        output.controlDetails.throttlePassby = true;
        output.controlDetails.throttlePercent = 0;

        //maintaing speed for flare attitude
        output.desiredAirspeed = LandingManager::slowFlightSpeed(pathMgr->isPackage);
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
    //load in sensor fusion data and telemtry data into input structure
    input.telemetryData = ;//some get from telemetry command that returns telemetryPIGO
    input.sensorOutput = *(sensorFusion::GetSFOutput());

    if(input.sensorOutput.altitude <= (TOUCHDOWN_ALTITUDE + input.telemetryData.stoppingAltitude)) //altitude is 5 cm or less/ultrasonic sensor sensed 5cm or less
    {
        pathMgr->stage = TOUCHDOWN;
    }
    else
    {
        //align heading with landing direction
        output.desiredHeading = input.telemetryData.stoppingDirectionHeading;
        output.useHeading = true;
        //retrieving desired slow flight speed
        output.desiredAirspeed = LandingManager::slowFlightSpeed(pathMgr->isPackage);
        
        //throttle off
        output.controlDetails.throttlePassby = true;
        output.controlDetails.throttlePercent = 0;
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
    //load in sensor fusion data and telemtry data into input structure
    input.telemetryData = ;//some get from telemetry command that returns telemetryPIGO
    input.sensorOutput = *(sensorFusion::GetSFOutput());

    //throttle off
    output.controlDetails.throttlePassby = true;
    output.controlDetails.throttlePercent = 0;

    //aligning heading
    output.desiredHeading = input.telemetryData.stoppingDirectionHeading;
    output.useHeading = true;

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
    //load in sensor fusion data and telemtry data into input structure
    input.telemetryData = ;//some get from telemetry command that returns telemetryPIGO
    input.sensorOutput = *(sensorFusion::GetSFOutput());

    //max throttle
    output.controlDetails.throttlePassby = true;
    output.controlDetails.throttlePercent = 100;

    if(!pathMgr->madeTakeoffPoints)
    {
        takeoffPath = WaypointManager(0.0,0.0);
        takeoffPoint = TakeoffManager::createTakeoffWaypoint(input.sensorOutput.latitude,input.sensorOutput.longitude, input.sensorOutput.altitude, input.telemetryData.takeoffDirectionHeading);
        pathArray[0] = takeoffPath.initialize_waypoint(takeoffPoint.longitude, takeoffPoint.latitude, takeoffPoint.altitude, PATH_FOLLOW);
        currentLocation = takeoffPath.initialize_waypoint(input.sensorOutput.longitude, input.sensorOutput.latitude, input.sensorOutput.altitude, HOLD_WAYPOINT);
        waypointStatus = takeoffPath.initialize_flight_path(pathArray, 1, currentLocation);

        pathMgr->madeTakeoffPoints = true;
    }

    if(input.sensorOutput.currentAirspeed>(TakeoffManager::desiredRotationSpeed(pathMgr->isPackage)))
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
    //load in sensor fusion data and telemtry data into input structure
    input.telemetryData = ;//some get from telemetry command that returns telemetryPIGO
    input.sensorOutput = *(sensorFusion::GetSFOutput());

    if(input.sensorOutput.altitude>(takeoffRollStage::takeoffPoint.altitude + EXIT_TAKEOFF_ALTITUDE))
    {
        pathMgr->stage = CRUISING;
    }
    else
    {   
        takeoffRollStage::waypointStatus = takeoffRollStage::takeoffPath.get_next_directions(input.sensorOutput, &waypointOutput);
        output = LandingManager::translateWaypointCommands(waypointOutput);
        output.desiredAirspeed = TakeoffManager::desiredClimbSpeed(pathMgr->isPackage);
        
        //maxThrottle
        output.controlDetails.throttlePassby = true;
        output.controlDetails.throttlePercent = 80;
    }
}

pathManagerState& takeoffClimbStage::getInstance()
{
    static takeoffClimbStage singleton;
    return singleton;
}














