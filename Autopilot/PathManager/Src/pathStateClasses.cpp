#include "pathStateClasses.hpp"

bool isError; 

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
    if(isError)
    {
        pathMgr -> setState(FatalFailureMode::getInstance());
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
    if(isError)
    {
        pathMgr -> setState(FatalFailureMode::getInstance());
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
    if(isError)
    {
        pathMgr -> setState(FatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(cruisingState::getInstance());
    }
}

pathManagerState& sensorFusion::getInstance()
{
    static sensorFusion singleton;
    return singleton;
}

void cruisingState::execute(pathManager* pathMgr)
{
    //waypoint manager stuff
    if(isError)
    {
        pathMgr -> setState(FatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(coordinateTurnElevation::getInstance());
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
        pathMgr -> setState(FatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(commsWithAttitude::getInstance());
    }
}

pathManagerState& coordinateTurnElevation::getInstance()
{
    static coordinateTurnElevation singleton;
    return singleton;
}

pathManagerState& FatalFailureMode::getInstance()
{
    static FatalFailureMode singleton;
    return singleton;
}


