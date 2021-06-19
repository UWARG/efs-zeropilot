/*
* Author: Nixon Chan
* Implementing Telem Comms: Gordon Fountain, Dhruv Rawat
*/

#include "telemetryStateClasses.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

Telemetry_PIGO_t obtainDataMode::_rawPMData; // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW
Telemetry_PIGO_t decodeDataMode::_decodedPMData;

Telemetry_POGI_t readFromPathMode::_rawGSData;
Telemetry_POGI_t encodeDataMode::_encodedGSData; // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW

#ifdef UNIT_TESTING
    XBEE* obtainDataMode::ZPXbee;
#endif

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void initialMode::execute(telemetryManager* telemetryMgr)
{
    //Initialize mail queue to SEND data to Path Manager
    CommFromTMToPMInit();
    
    //Initialize mail queue to RECEIVE Path Manager data
    CommFromPMToTMInit();

    //State change
    telemetryMgr->setState(obtainDataMode::getInstance());
}

telemetryState& initialMode::getInstance()
{
    static initialMode singleton;
    return singleton;
}

void obtainDataMode::execute(telemetryManager* telemetryMgr)
{
    //obtain data from ground
    ZPXbee->Receive_GS_Data(); //Receives data in MavLink form from the XBEE

    // Set _rawPMData
    _rawPMData = {0};

    //State change:
    if(telemetryMgr->fatalFail)
    {
        telemetryMgr->setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr->setState(decodeDataMode::getInstance());
    }
}

telemetryState& obtainDataMode::getInstance()
{
    static obtainDataMode singleton;
    return singleton;
}

/**
 * 
 * THIS IS BLOCKED BY MAVLINK DECODER
 * 
 */ 
void decodeDataMode::execute(telemetryManager* telemetryMgr)
{
    // Get mavlink data from obtainDataMode state
    Telemetry_PIGO_t* rawPMData = obtainDataMode::getRawPMData(); // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW

    // Decode data with Mavlink
    _decodedPMData = {0};

    //State change:
    if(telemetryMgr->fatalFail)
    {
        telemetryMgr->setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr->setState(passToPathMode::getInstance());
    }
}

telemetryState& decodeDataMode::getInstance()
{
    static decodeDataMode singleton;
    return singleton;
}

void passToPathMode::execute(telemetryManager* telemetryMgr)
{
    // Get decoded data 
    Telemetry_PIGO_t* pmData = decodeDataMode::getDecodedPMData();

    //pass decoded data to path manager
    SendFromTMToPM(pmData); 

    //State change:
    if(telemetryMgr->fatalFail)
    {
        telemetryMgr->setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr->setState(readFromPathMode::getInstance());
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
    GetFromPMToTM(&_rawGSData);

    //State change:
    if(telemetryMgr->fatalFail)
    {
        telemetryMgr->setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr->setState(analyzeDataMode::getInstance());
    }
}

telemetryState& readFromPathMode::getInstance()
{
    static readFromPathMode singleton;
    return singleton;
}

void analyzeDataMode::execute(telemetryManager* telemetryMgr)
{
    // Get decoded data 
    Telemetry_PIGO_t* pmData = decodeDataMode::getDecodedPMData(); 

    // Evaluate if the received data is valid
    telemetryMgr->dataValid = 1;
    telemetryMgr->dataValid &= pmData->holdingAltitude >= 20;
    telemetryMgr->dataValid &= pmData->holdingTurnDirection == 1 || pmData->holdingTurnDirection == 0;
    telemetryMgr->dataValid &= pmData->holdingTurnRadius > 10;
    telemetryMgr->dataValid &= pmData->initializingHomeBase == true || pmData->initializingHomeBase == false;
    if(pmData->initializingHomeBase) {
        telemetryMgr->dataValid &= (pmData->homebase.waypointType == 0 || pmData->homebase.waypointType == 1 || pmData->homebase.waypointType == 2);
    }
    telemetryMgr->dataValid &= pmData->modifyId > 0;
    telemetryMgr->dataValid &= pmData->nextId > 0;
    telemetryMgr->dataValid &= pmData->prevId > 0;
    telemetryMgr->dataValid &= pmData->stoppingAltitude >= 0;
    telemetryMgr->dataValid &= pmData->stoppingDirectionHeading >= 0 && pmData->stoppingDirectionHeading < 360;
    telemetryMgr->dataValid &= pmData->stoppingLatitude >= 0;
    telemetryMgr->dataValid &= pmData->stoppingLongitude >= 0;
    telemetryMgr->dataValid &= pmData->takeoffDirectionHeading >= 0 && pmData->takeoffDirectionHeading < 360;
    telemetryMgr->dataValid &= pmData->waypointModifyFlightPathCommand > 0 && pmData->waypointModifyFlightPathCommand <= 6;
    telemetryMgr->dataValid &= pmData->waypointNextDirectionsCommand > 0 && pmData->waypointNextDirectionsCommand <= 2;
    if((pmData->numWaypoints == 0 && pmData->waypoints != nullptr) || pmData->numWaypoints < 0 || (pmData->numWaypoints > 0 && pmData->waypoints == nullptr)) {
        telemetryMgr->dataValid = false;
    }
    
    if(telemetryMgr->dataValid) {
        telemetryMgr->failCycleCounter = 0;
    }
    else {
        telemetryMgr->failCycleCounter++;
        if(telemetryMgr->failCycleCounter++ > 50) {
            telemetryMgr->fatalFail = true;
        }
    }

    //State change:
    if(telemetryMgr->fatalFail)
    {
        telemetryMgr->setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr->setState(analyzeDataMode::getInstance());
    }
}

telemetryState& analyzeDataMode::getInstance()
{
    static analyzeDataMode singleton;
    return singleton;
}

/**
 * 
 * THIS IS BLOCKED BY MAVLINK ENCODER
 * 
 */ 
void encodeDataMode::execute(telemetryManager* telemetryMgr)
{
    // Get raw GS data
    Telemetry_POGI_t* rawGSData = readFromPathMode::getRawGSData();

    //encode data with mavlink (Jingting's function)
    _encodedGSData = {0};

    //State change:
    if(telemetryMgr->fatalFail)
    {
        telemetryMgr->setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr->setState(sendDataMode::getInstance());
    }
}

telemetryState& encodeDataMode::getInstance()
{
    static encodeDataMode singleton;
    return singleton;
}

void sendDataMode::execute(telemetryManager* telemetryMgr)
{
    // Get encoded data
    Telemetry_POGI_t* encodedGSData = encodeDataMode::getEncodedGSData(); // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW

    //send data to ground
    //Not sure if this state is needed. From what I understand FREERTOS is calling the function to send down data.
    ZPXbee->Send_GS_Data(); // PROBABLY NEED A PARAMETER HERE @GORDON

    //State change:
    if(telemetryMgr->fatalFail)
    {
        telemetryMgr->setState(failureMode::getInstance());
    }
    else
    {
        if(telemetryMgr->regularReport) //determines if the FSM should reinitialize
        {
            telemetryMgr->setState(obtainDataMode::getInstance());
        }
        else
        {
            telemetryMgr->setState(initialMode::getInstance());
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
    telemetryMgr->setState(failureMode::getInstance());
}

telemetryState& failureMode::getInstance()
{
    static failureMode singleton;
    return singleton;
}
