#include "pathStateClasses.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

bool isError; 
Telemetry_PIGO_t commsWithTelemetry::_incomingData;
_CruisingState_Telemetry_Return cruisingState::_returnToGround;
_WaypointManager_Data_In cruisingState::_inputdata;
_WaypointManager_Data_Out cruisingState::_outputdata; 

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void commsWithAttitude::execute(pathManager* pathMgr)
{
    //initial mode
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
        pathMgr->setState(getSensorData::getInstance());
    }
}

pathManagerState& commsWithTelemetry::getInstance()
{
    static commsWithTelemetry singleton;
    return singleton;
}

void getSensorData::execute(pathManager* pathMgr)
{
    // Initializes the sensor data structures 
    SensorError_t errorStruct = SensorMeasurements_GetResult(&AltimeterSens, &GpsSens, &_altimeterdata, &_gpsdata); 
    
    //obtain sensor data
    if(isError)
    {
        pathMgr->setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr->setState(sensorFusion::getInstance());
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

    int editError = editFlightPath(telemetryData, cruisingStateManager, waypointIDArray); // Edit flight path if applicable

    // Set input data for getting next direction/altitude
    Gps_Data_t * gpsData = getSensorData::GetGPSOutput();
    // Get sensor fusion data
    // SFPositionOutput_t * sfData = sensorFusion::GetSFPositionOutput();

    _inputdata.heading = gpsData->heading;
    // Set input data parameters
    // _inputdata.longitude = sfData->longitude;
    // _inputdata.latitude = sfData->latitude;
    // _inputdata.altitude = sfData->altitude;

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
    //get elevation and turning data
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





