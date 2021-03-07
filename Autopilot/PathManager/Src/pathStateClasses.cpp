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
    //retrieve LANDING DATA
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
        //if the enums for landing state, set to each landing state
        if(pathMgr->stage == TRANSITION)
        {
            pathMgr->setState(landingTransitionStage::getInstance());
        }
        else if(pathMgr->stage == SLOPE)
        {
            pathMgr->setState(landingSlopeStage::getInstance());
        }
        else if(pathMgr->stage == FLARE)
        {
            pathMgr->setState(landingFlareStage::getInstance());
        }
        else if(pathMgr->stage == DECRAB)
        {
            pathMgr->setState(landingDecrabStage::getInstance());
        }
        else if(pathMgr->stage == TOUCHDOWN)
        {
            pathMgr->setState(landingTouchdownStage::getInstance());
        }
        else if(pathMgr->stage == NOT_LANDING)
        {
            pathMgr->setState(cruisingState::getInstance());
        }
        else
        {
            pathMgr->setState(cruisingState::getInstance());
        }
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
    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
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
    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
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

void fatalFailureMode::execute(pathManager* pathMgr)
{
    pathMgr->setState(fatalFailureMode::getInstance());
}

pathManagerState& fatalFailureMode::getInstance()
{
    static fatalFailureMode singleton;
    return singleton;
}


