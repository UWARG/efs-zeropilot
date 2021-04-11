#include "pathStateClasses.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

bool isError; 
Telemetry_PIGO_t commsWithTelemetry::_incomingData;
_CruisingState_Telemetry_Return cruisingState::_returnToGround;
_WaypointManager_Data_In cruisingState::_inputdata;
_WaypointManager_Data_Out cruisingState::_outputdata; 
SFOutput_t sensorFusion::sfOutputData;
CoordinatedTurnAttitudeManagerCommands_t coordinateTurnElevation::_rollandrudder;
AltitudeAirspeedCommands_t coordinateTurnElevation::_pitchandairspeed; 
AttitudeData commsWithAttitude::_receivedData;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void commsWithAttitude::execute(pathManager* pathMgr)
{
    
    bool newDataAvailable = GetAttitudeData(&_receivedData); // Gets attitude manager data

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
    //send data to telemetry

    // Get data from telemetry

    // Do any processing required (update struct that contains telemetry data and process it)

    // Store data inside of the Telemetry_PIGO_t struct that is a parameter of this child class

    if(isError)
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
    
    sfOutputData = SF_GetResult(); // Gets current Sensor fusion output struct

    if(isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(cruisingState::getInstance());
    }
}

pathManagerState& sensorFusion::getInstance()
{
    static sensorFusion singleton;
    return singleton;
}

void cruisingState::execute(pathManager* pathMgr)
{

    Telemetry_PIGO_t * telemetryData = commsWithTelemetry::GetTelemetryIncomingData(); // Get struct from telemetry state with all of the commands and values.
    SFOutput_t * pSensFusionOutput = sensorFusion::GetSFOutput(); // Get sensor fusion data
    
    // Waypoint manager expects to receive the track of the plane, not its heading, so we will use the data from the sensor fusion regarding the plane's x (longitude) and y (latitude) motion to get its track.
    double track = atan(pSensFusionOutput->latitudeSpeed / pSensFusionOutput->longitudeSpeed) * 180.0 / PI; // Gets angle of track in quadrant 1

    /* 
    
    Need a way to convert the value above to a good track value 
    
    */

    // if (pSensFusionOutput->yaw >= PI/2 && pSensFusionOutput->yaw <= PI) { // If in Quadrant 2
    //     track = 180.0 - track;
    // } else if (pSensFusionOutput->yaw > PI && pSensFusionOutput->yaw <= 3*PI/2) { // If in Quadrant 3
    //     track = 180.0 + track;
    // } else if (pSensFusionOutput->yaw > 3*PI/2 && pSensFusionOutput->yaw <= 2*PI) { // If in Quadrant 4
    //     track = 360.0 - track;
    // }

    // Set waypoint manager input struct
    _inputdata.track = track;
    _inputdata.longitude = pSensFusionOutput->longitude;
    _inputdata.latitude = pSensFusionOutput->latitude;
    _inputdata.altitude = pSensFusionOutput->altitude;

    // Call module functions
    int editError = editFlightPath(telemetryData, cruisingStateManager, waypointIDArray); // Edit flight path if applicable
    int pathError = pathFollow(telemetryData, cruisingStateManager, _inputdata, &_outputdata, goingHome, inHold); // Get next direction or modify flight behaviour pattern
    setReturnValues(&_returnToGround, cruisingStateManager, editError, pathError); // Set error codes

    if(isError)
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
    
    SFOutput_t * pSensFusionOutput = sensorFusion::GetSFOutput(); // Get sensor fusion data
    _WaypointManager_Data_Out * waypointOutput = cruisingState::GetOutputData(); // Get output data from waypoint manager
    
    // Set up the coordinated turn function input
    double track = atan(pSensFusionOutput->latitudeSpeed / pSensFusionOutput->longitudeSpeed) * 180.0 / PI; // Gets angle of track in quadrant 1

    /* 
    
    Need a way to convert the value above to a good track value 
    
    */

    CoordinatedTurnInput_t turnInput {};
    turnInput.currentHeadingTrack = track;
    turnInput.desiredHeadingTrack = waypointOutput->desiredTrack;
    turnInput.accY = pSensFusionOutput->pitch; // Is this actually the data that you want?
    
    // Set up the compute altitude and airspeed function input
    AltitudeAirspeedInput_t altAirspeedInput {};
    altAirspeedInput.currentAltitude = pSensFusionOutput->altitude;
    altAirspeedInput.desiredAltitude = waypointOutput->desiredAltitude;
    altAirspeedInput.currentAirspeed = pSensFusionOutput->airspeed;
    altAirspeedInput.desiredAirspeed = 30;                                      // Where am I supposed to get this?

    // Call module functions
    AutoSteer_ComputeCoordinatedTurn(&turnInput, &_rollandrudder);
    AutoSteer_ComputeAltitudeAndAirspeed(&altAirspeedInput, &_pitchandairspeed);

    if(isError)
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





