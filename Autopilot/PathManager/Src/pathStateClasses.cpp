#include "pathStateClasses.hpp"

/***********************************************************************************************************************
 * Static Member Variable Declarations
 **********************************************************************************************************************/

Telemetry_PIGO_t commsWithTelemetry::_incomingData;
SFOutput_t sensorFusion::_sfOutputData;
IMU_Data_t sensorFusion::_imudata;
CoordinatedTurnAttitudeManagerCommands_t coordinateTurnElevation::_rollandrudder;
AltitudeAirspeedCommands_t coordinateTurnElevation::_pitchandairspeed;
AttitudeData commsWithAttitude::_receivedData;

// For landing and takeoff states that use waypoint manager functions, the variables below will hold the I/O that is needed for those functions to work

_LandingTakeoffInput takeoffRollStage::input;
_LandingTakeoffOutput takeoffRollStage::output;
_WaypointManager_Data_Out takeoffRollStage::waypointOutput;
_PathData * takeoffRollStage::currentLocation;
_PathData * takeoffRollStage::pathArray[1];
WaypointManager takeoffRollStage::takeoffPath;
_PathData takeoffRollStage::takeoffPoint;
_WaypointStatus takeoffRollStage::waypointStatus;

_LandingTakeoffInput takeoffClimbStage::input;
_LandingTakeoffOutput takeoffClimbStage::output;
_WaypointManager_Data_In takeoffClimbStage::waypointInput;
_WaypointManager_Data_Out takeoffClimbStage::waypointOutput;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void commsWithAttitude::execute(pathManager* pathMgr)
{

    bool newDataAvailable = GetFromAMToPM(&_receivedData); // Gets attitude manager data

    // Gets data used to populate CommandsForAM struct
    CoordinatedTurnAttitudeManagerCommands_t * turnCommands = coordinateTurnElevation::GetRollAndRudder();
    AltitudeAirspeedCommands_t * altCommands = coordinateTurnElevation::GetPitchAndAirspeed();

    CommandsForAM toSend {};
    toSend.roll = turnCommands->requiredRoll;
    toSend.pitch = altCommands->requiredPitch;
    toSend.rudderPercent = turnCommands->requiredRudderPosition;
    toSend.throttlePercent = altCommands->requiredThrottlePercent;

    SendFromPMToAM(&toSend); // Sends commands to attitude manager

    pathMgr->setState(commsWithTelemetry::getInstance());
}

pathManagerState& commsWithAttitude::getInstance()
{
    static commsWithAttitude singleton;
    return singleton;
}

void commsWithTelemetry::execute(pathManager* pathMgr)
{
    GetTelemetryCommands(&_incomingData);

    if(pathMgr->isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(sensorFusion::getInstance());
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
        pathMgr->setState(modeExecutor::getInstance());
    }
}

pathManagerState& sensorFusion::getInstance()
{
    static sensorFusion singleton;
    return singleton;
}

#ifdef UNIT_TESTING

#include "cruisingMode.hpp"

static Telemetry_Waypoint_Data_t createTelemetryWaypoint(long double lon, long double lat, int alt, float turnRadius, int type) {
    Telemetry_Waypoint_Data_t waypoint {};

    waypoint.longitude = lon;
    waypoint.latitude = lat;
    waypoint.altitude = alt;
    waypoint.turnRadius = turnRadius;
    waypoint.waypointType = type;

    return waypoint;
}

#endif

void modeExecutor::execute(pathManager* pathMgr) 
{
    PathModeSelector* mode_selector = PathModeSelector::getInstance();

    #ifdef UNIT_TESTING

        if (!mode_selector->getCurrentMode()) {
            mode_selector->setCurrentMode(CruisingMode::getInstance());
        }

        Telemetry_PIGO_t telem_data;

        telem_data.waypoints[0] = createTelemetryWaypoint(0.0, 0.0, 6, 0.0, 0);
        telem_data.waypoints[1] = createTelemetryWaypoint(0.0, 0.0, 7, 0.0, 0);
        telem_data.waypoints[2] = createTelemetryWaypoint(0.0, 0.0, 8, 0.0, 0);
        telem_data.waypoints[3] = createTelemetryWaypoint(0.0, 0.0, 9, 0.0, 0);

        telem_data.numWaypoints = 4;
        telem_data.waypointModifyFlightPathCommand = INITIALIZE_FLIGHT_PATH;
        telem_data.initializingHomeBase = 1;
        telem_data.waypointNextDirectionsCommand = REGULAR_PATH_FOLLOWING;
        telem_data.holdingAltitude = 0;
        telem_data.holdingTurnRadius = 0;
        telem_data.holdingTurnDirection = 0;
        telem_data.nextId = 0;
        telem_data.prevId = 0;
        telem_data.modifyId = 0;
        telem_data.homebase = createTelemetryWaypoint(0.0, 0.0, 100, 0.0, 0);
        telem_data.stoppingDirectionHeading = 10;

        SFOutput_t sf_data;
        sf_data.track = 100;
        sf_data.altitude = -2;
        sf_data.airspeed = -3;

        IMU_Data_t imu_data {};
    #else 
        Telemetry_PIGO_t telem_data = *(commsWithTelemetry::GetTelemetryIncomingData());
        SFOutput_t sf_data = *(sensorFusion::GetSFOutput());
        IMU_Data_t imu_data = *(sensorFusion::GetIMUData());
    #endif

    mode_selector->execute(telem_data, sf_data, imu_data);

    if (mode_selector->getIsError()) {
        pathMgr->setState(fatalFailureMode::getInstance());
    } else {
        pathMgr->setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& modeExecutor::getInstance() {
    static modeExecutor singleton;
    return singleton;
}

void coordinateTurnElevation::execute(pathManager* pathMgr)
{
    IMU_Data_t * imudata = sensorFusion::GetIMUData(); // Gets raw IMU data
    PathModeSelector* mode_selector = PathModeSelector::getInstance(); // Get PathModeSelector for input structs

    // Get input from PathModeSelector
    CoordinatedTurnInput_t turnInput = mode_selector->getCoordinatedTurnInput();
    turnInput.accY = imudata->accy;
    AltitudeAirspeedInput_t altAirspeedInput = mode_selector->getAltitudeAirspeedInput();

    // Call module functions
    AutoSteer_ComputeCoordinatedTurn(&turnInput, &_rollandrudder);
    AutoSteer_ComputeAltitudeAndAirspeed(&altAirspeedInput, &_pitchandairspeed);

    // passby functionality
    _PassbyControl passby_output = mode_selector->getPassbyControl();
    if(passby_output.pitchPassby)
    {
        _pitchandairspeed.requiredPitch = passby_output.pitchPercent;
    }

    if(passby_output.throttlePassby)
    {
        _pitchandairspeed.requiredThrottlePercent = passby_output.throttlePercent;
    }

    if(passby_output.rollPassby)
    {
        _rollandrudder.requiredRoll = passby_output.rollPercent;
    }

    if(passby_output.rudderPassby)
    {
        _rollandrudder.requiredRudderPosition = passby_output.rudderPercent;
    }
    
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
TAKEOFF STATE FUNCTIONS
****************************************************************************************************/

void takeoffRollStage::execute(pathManager* pathMgr)
{
    pathMgr->stage = ROLL;
    //load in sensor fusion data and telemtry data into input structure
    input.telemetryData = commsWithTelemetry::GetTelemetryIncomingData();
    input.sensorOutput = sensorFusion::GetSFOutput();

    //max throttle
    output.controlDetails.throttlePassby = true;
    output.controlDetails.throttlePercent = 100;

    if(!pathMgr->madeTakeoffPoints)
    {
        takeoffPoint = LandingTakeoffManager::createTakeoffWaypoint(input.sensorOutput->latitude,input.sensorOutput->longitude, input.sensorOutput->altitude, input.telemetryData->takeoffDirectionHeading);
        pathArray[0] = takeoffPath.initialize_waypoint(takeoffPoint.longitude, takeoffPoint.latitude, takeoffPoint.altitude, PATH_FOLLOW);

        //10 meters is added to the altitude of the currentLocation waypoint so that is is not ground level
        currentLocation = takeoffPath.initialize_waypoint(input.sensorOutput->longitude, input.sensorOutput->latitude, (input.sensorOutput->altitude + 10), HOLD_WAYPOINT);
        waypointStatus = takeoffPath.initialize_flight_path(pathArray, 1, currentLocation);
        pathMgr->madeTakeoffPoints = true;
    }

    if(input.sensorOutput->airspeed > (LandingTakeoffManager::desiredRotationSpeed(pathMgr->isPackage)))
    {
        pathMgr->stage = CLIMB;
    }

    if(takeoffRollStage::waypointStatus != WAYPOINT_SUCCESS)
    {
        pathMgr->isError = true;
    }

    if(pathMgr->isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(coordinateTurnElevation::getInstance());
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
    input.telemetryData = commsWithTelemetry::GetTelemetryIncomingData();
    input.sensorOutput = sensorFusion::GetSFOutput();

    if(input.sensorOutput->altitude > (takeoffRollStage::takeoffPoint.altitude + EXIT_TAKEOFF_ALTITUDE))
    {
        pathMgr->stage = CRUISING;
    }
    else
    {
        //setting sensorFusion input to waypoint data in
        waypointInput.latitude = input.sensorOutput->latitude;
        waypointInput.longitude = input.sensorOutput->longitude;
        waypointInput.altitude = input.sensorOutput->altitude;
        waypointInput.track = input.sensorOutput->track;

        takeoffRollStage::waypointStatus = takeoffRollStage::takeoffPath.get_next_directions(waypointInput, &waypointOutput);
        output = LandingTakeoffManager::translateWaypointCommands(waypointOutput);

        output.desiredAirspeed = LandingTakeoffManager::desiredClimbSpeed(pathMgr->isPackage);
        
        //pitching up
        output.controlDetails.pitchPassby = true;
        output.controlDetails.pitchPercent =  0.3;
    }

    //ensuring made takeoff points is reset
    pathMgr->madeTakeoffPoints = false;

    if(takeoffRollStage::waypointStatus != WAYPOINT_SUCCESS)
    {
        pathMgr->isError = true;
    }

    if(pathMgr->isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& takeoffClimbStage::getInstance()
{
    static takeoffClimbStage singleton;
    return singleton;
}

