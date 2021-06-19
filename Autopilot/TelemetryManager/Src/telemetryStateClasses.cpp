/*
* Author: Nixon Chan
* Implementing Telem Comms: Gordon Fountain
*/

#include "telemetryStateClasses.hpp"



static Telemetry_PIGO_t _PMData;
static Telemetry_POGI_t _GSData;

void initialMode::execute(telemetryManager* telemetryMgr)
{
    //initial mode
    //Initialize mail queue to send data to Path Manager:
    TelemCommWithPMInit();
    //Initializa mail queue to receive Path Manager data

    //State change:
    telemetryMgr -> setState(obtainDataMode::getInstance());
}

telemetryState& initialMode::getInstance()
{
    static initialMode singleton;
    return singleton;
}

void obtainDataMode::execute(telemetryManager* telemetryMgr)
{
    //obtain data from ground
    ZPXBEbee.Receive_GS_Data(); //Receives data in MavLink form from the XBEE

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(decodeDataMode::getInstance());
    }
}

telemetryState& obtainDataMode::getInstance()
{
    static obtainDataMode singleton;
    return singleton;
}

void decodeDataMode::execute(telemetryManager* telemetryMgr)
{
    //decode data with Mavlink
    //Call Jingting's function here

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(passToPathMode::getInstance());
    }
}

telemetryState& decodeDataMode::getInstance()
{
    static decodeDataMode singleton;
    return singleton;
}

void passToPathMode::execute(telemetryManager* telemetryMgr)
{
    //pass decoded data to path manager
    SendCommandsForPM(*_PMData); //Send it off to the inbox for PathManager
    
    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(readFromPathMode::getInstance());
    }
}

telemetryState& passToPathMode::getInstance()
{
    static passToPathMode singleton;
    return singleton;
}

void readFromPathMode::execute(telemetryManager* telemetryMgr)
{
    //read data out of path manager
    //GetTelemData(data);

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(analyzeDataMode::getInstance());
    }
}

telemetryState& readFromPathMode::getInstance()
{
    static readFromPathMode singleton;
    return singleton;
}

void analyzeDataMode::execute(telemetryManager* telemetryMgr)
{
    
    telemetryMgr->dataValid = 1;
    telemetryMgr->dataValid &= _PMData.holdingAltitude >= 20;
    telemetryMgr->dataValid &= _PMData.holdingTurnDirection == 1 || _PMData.holdingTurnDirection == 0;
    telemetryMgr->dataValid &= _PMData.holdingTurnRadius > 10;
    telemetryMgr->dataValid &= _PMData.initializingHomeBase == true || _PMData.initializingHomeBase == false;
    if(_PMData.initializingHomeBase) {
        telemetryMgr->dataValid &= (_PMData.homebase.waypointType == 0 || _PMData.homebase.waypointType == 1 || _PMData.homebase.waypointType == 2);
    }
    telemetryMgr->dataValid &= _PMData.modifyId > 0;
    telemetryMgr->dataValid &= _PMData.nextId > 0;
    telemetryMgr->dataValid &= _PMData.prevId > 0;
    telemetryMgr->dataValid &= _PMData.stoppingAltitude >= 0;
    telemetryMgr->dataValid &= _PMData.stoppingDirectionHeading >= 0 && _PWData.stoppingDirectionHeading < 360;
    telemetryMgr->dataValid &= _PMData.stoppingLatitude >= 0;
    telemetryMgr->dataValid &= _PMData.stoppingLongitude >= 0;
    telemetryMgr->dataValid &= _PMData.takeoffDirectionHeading >= 0 && _PWData.takeoffDirectionHeading < 360;
    telemetryMgr->dataValid &= _PMData.waypointModifyFlightPathCommand > 0 && _PMData.waypointModifyFlightPathCommand <= 6;
    telemetryMgr->dataValid &= _PMData.waypointNextDirectionsCommand > 0 && _PMData.waypointNextDirectionsCommand <= 2;
    if((_PMData.numWaypoints == 0 && _PMData.waypoints != nullptr) || _PMData.waypoints < 0) {
        telemetryMgr->dataValid = false;
    }
    else if(_PMData.waypoints > 0 && _PMData.waypoints == nullptr) {
        telemetryMgr->dataValid = false;
    }

    
    if(telemetryMgr->dataValid) {
        telemetryMgr->failCycleCounter = 0;
    }
    else {
        telemetryMgr->failCycleCounter++;
        if(telemetryMgr->failCycleCounter++ > 50) {
            telemetryMgr -> fatalFail = true;
        }
    }

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(analyzeDataMode::getInstance());
    }
}

telemetryState& analyzeDataMode::getInstance()
{
    static analyzeDataMode singleton;
    return singleton;
}

void encodeDataMode::execute(telemetryManager* telemetryMgr)
{
    //encode data with mavlink (Jingting's function)

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(sendDataMode::getInstance());
    }
}

telemetryState& encodeDataMode::getInstance()
{
    static encodeDataMode singleton;
    return singleton;
}

void sendDataMode::execute(telemetryManager* telemetryMgr)
{
    //send data to ground
    //Not sure if this state is needed. From what I understand FREERTOS is calling the function to send down data.
    //Send_GS_Data(&_GSData);

    //State change:
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        if(telemetryMgr -> regularReport) //determines if the FSM should reinitialize
        {
            telemetryMgr -> setState(obtainDataMode::getInstance());
        }
        else
        {
            telemetryMgr -> setState(initialMode::getInstance());
        }
    }
}

telemetryState& sendDataMode::getInstance()
{
    static sendDataMode singleton;
    return singleton;
}

void failureMode::execute(telemetryManager* telemetryMgr)
{
    //State change:
    telemetryMgr -> setState(failureMode::getInstance());
}

telemetryState& failureMode::getInstance()
{
    static failureMode singleton;
    return singleton;
}
