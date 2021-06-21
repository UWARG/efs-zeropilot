/*
* Author: Nixon Chan, Gordon Fountain, Dhruv Rawat
*/

#include "telemetryStateClasses.hpp"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

Telemetry_PIGO_t obtainDataMode::_decodedPMData; 
// Telemetry_PIGO_t decodeDataMode::_decodedPMData;

Telemetry_POGI_t readFromPathMode::_rawGSData;
// mavlink_message_t encodeDataMode::_encodedGSData; // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void initialMode::execute(telemetryManager* telemetryMgr)
{
    //Initialize mail queue to communicate with Path Manager
    CommFromTMToPMInit();

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

    // Set _decodedPMData
    

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

telemetryState& obtainDataMode::getInstance()
{
    static obtainDataMode singleton;
    return singleton;
}


// void decodeDataMode::execute(telemetryManager* telemetryMgr)
// {
//     // Get mavlink data from obtainDataMode state
//     std::vector<uint8_t> rawData = obtainDataMode::getRawPMData(); // CHANGE DATATYPE. ONLY USING THIS SO I CAN GET FLOW

//     // Decode data with Mavlink
//     _AirsideMavlinkDecodingEncoding errorCode = decode(rawData, &_decodedPMData);

//     if (MAVLINK_DECODING_ENCODING_FAILED == errorCode) {
//         decodingErrorCount++;
//         if (decodingErrorCount > 10) {
//             telemetryMgr->fatalFail = true;
//         }
//     } else {
//         decodingErrorCount = 0;
//     }

//     //State change:
//     if(telemetryMgr->fatalFail)
//     {
//         telemetryMgr->setState(failureMode::getInstance());
//     }
//     else
//     {
//         telemetryMgr->setState(passToPathMode::getInstance());
//     }
// }

// telemetryState& decodeDataMode::getInstance()
// {
//     static decodeDataMode singleton;
//     return singleton;
// }

void passToPathMode::execute(telemetryManager* telemetryMgr)
{
    // Get decoded data 
    Telemetry_PIGO_t* pmData = obtainDataMode::getDecodedPMData();

    // Pass decoded data to path manager
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
    Telemetry_PIGO_t* pmData = obtainDataMode::getDecodedPMData(); 

    /*********
     *  TODO, CLEAN THIS UP
     *********/ 

    // Evaluate if the received data is valid
    telemetryMgr->dataValid = true;
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
    if((pmData->numWaypoints == 0 && pmData->waypoints.size() == 0) || pmData->numWaypoints < 0 || (pmData->numWaypoints > 0 && pmData->waypoints.size() == 0)) {
        telemetryMgr->dataValid = false;
    }
    telemetryMgr->dataValid &= pmData->gimbalPitch >= 0 && pmData->gimbalPitch <= ZP_PI;
    telemetryMgr->dataValid &= (pmData->gimbalYaw >= -1 * ZP_PI/2) && pmData->gimbalYaw <= ZP_PI/2;
    telemetryMgr->dataValid &= pmData->groundCommandsHeading >= 0 && pmData->groundCommandsHeading < 360;
    telemetryMgr->dataValid &= pmData->latestDistance >= 0;

    if(telemetryMgr->dataValid) {
        telemetryMgr->failCycleCounter = 0;
        if (telemetryMgr->dataError) {
            telemetryMgr->regularReport = false;
        } else {
            telemetryMgr->regularReport = true;
        }
    }
    else {
        telemetryMgr->failCycleCounter++;
        telemetryMgr->regularReport = false;

        if (telemetryMgr->failCycleCounter < 5) {
            telemetryMgr->regularReport = true;
        }

        if(telemetryMgr->failCycleCounter > 50) {
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
        telemetryMgr->setState(sendDataMode::getInstance());
    }
}

telemetryState& analyzeDataMode::getInstance()
{
    static analyzeDataMode singleton;
    return singleton;
}

// void encodeDataMode::execute(telemetryManager* telemetryMgr)
// {
//     // Get raw GS data
//     Telemetry_POGI_t* rawGSData = readFromPathMode::getRawGSData();

//     //encode data with mavlink (Jingting's function)
//     encode(rawGSData, &_encodedGSData); 

//     //State change:
//     if(telemetryMgr->fatalFail)
//     {
//         telemetryMgr->setState(failureMode::getInstance());
//     }
//     else
//     {
//         telemetryMgr->setState(sendDataMode::getInstance());
//     }
// }

// telemetryState& encodeDataMode::getInstance()
// {
//     static encodeDataMode singleton;
//     return singleton;
// }

void sendDataMode::execute(telemetryManager* telemetryMgr)
{
    // Get encoded data
    Telemetry_POGI_t* encodedGSData = readFromPathMode::getRawGSData(); 

    #ifdef UNIT_TESTING
        encodedGSData->errorCode = 0;
        encodedGSData->timeStamp = 100;
        encodedGSData->gpsLatitude = 1.03928;
        encodedGSData->gpsLongitude = 2.03928;
        encodedGSData->curAltitude = 20;
        encodedGSData->curAirspeed = 10;
        encodedGSData->roll = 90;
        encodedGSData->pitch = 1;
        encodedGSData->yaw = 2;
        encodedGSData->camRoll = 2.1;
        encodedGSData->camPitch = 2.2;
        encodedGSData->camYaw = 2.3;
        encodedGSData->isLanded = true;
        encodedGSData->editingFlightPathErrorCode = 1;
        encodedGSData->flightPathFollowingErrorCode = 2;
        encodedGSData->currentWaypointId = 69;
        encodedGSData->currentWaypointIndex = 70;
        encodedGSData->homeBaseInit = true;
    #endif

    //send data to ground
    _AirsideMavlinkDecodingEncoding result = ZPXbee->SendResult(*encodedGSData);

    // If sending fails 10 times in a row, then diconnect
    if (result != MAVLINK_DECODING_ENCODING_OK) {
        sendingErrorCount++;
        if (sendingErrorCount > 10) {
            telemetryMgr->fatalFail = true;
        }
    } else {
        sendingErrorCount = 0;
    }

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
