/*
* Author: Nixon Chan
*/

#include "telemetryStateClasses.hpp"
#include "UARTDriver.hpp"
#include"comms.hpp"
#include"FW_CV_Structs.hpp"
#include "CommsWithPathManager.hpp"

// Stuff that does compile: 
// UART DRIVER hpp include stm files
// Weird include reference to function error

struct fijo msg_from_jetson;

void initialMode::execute(telemetryManager* telemetryMgr)
{
    //initial mode
    startInterrupt();
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
    bool moveToDecode = doesFIJODataExist();
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else if(moveToDecode)
    {
        telemetryMgr -> setState(decodeDataMode::getInstance());
    }else
    {
        telemetryMgr -> setState(readFromPathMode::getInstance());
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
    msg_from_jetson = decodeFIJO();

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
    //pass data to path manager

    Telemetry_PIGO_t commsToPM; 
    commsToPM.FIJO = msg_from_jetson;
    SendCommandsForPM(&commsToPM);

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

bool newDataAvailable = true;

POGI msg_out;

void readFromPathMode::execute(telemetryManager* telemetryMgr)
{
    //read data out of path manager
    //If false no new data from PM is available
    newDataAvailable = GetTelemData(&msg_out);
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
    //set the dataValid here
    if(telemetryMgr ->dataValid)
    {
        //check for dataError, set it
    }
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(reportMode::getInstance());
    }
}

telemetryState& analyzeDataMode::getInstance()
{
    static analyzeDataMode singleton;
    return singleton;
}

void reportMode::execute(telemetryManager* telemetryMgr)
{
    //form report based on the the variables dataValid, dataError, and cycleCounter
    if(telemetryMgr -> dataValid)
    {
        telemetryMgr -> cycleCounter = 0;
        //detect if data contains error, load into bool dataError (error=true, no error=false)
        if(telemetryMgr -> dataError)
        {
            //form an error report
            telemetryMgr -> regularReport = false;
        }
        else
        {
            //form a regular report
            telemetryMgr -> regularReport = true;
        }
    }
    else
    {
        telemetryMgr -> cycleCounter++;
        if(telemetryMgr -> cycleCounter >= 5) //if data invalid more than 5 times, send error report
        {
            //form an error report
            telemetryMgr -> regularReport = false;
        }
        else
        {
            //form a regular report
            telemetryMgr -> regularReport = true;
        }
    }
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(encodeDataMode::getInstance());
    }
}

telemetryState& reportMode::getInstance()
{
    static reportMode singleton;
    return singleton;
}

void encodeDataMode::execute(telemetryManager* telemetryMgr)
{
    //encode data with mavlink
    sendFOJI(msg_out.FOJI);

    Comms::GetInstance()->transmitMessage(msg_out.POXI);

    
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
    telemetryMgr -> setState(failureMode::getInstance());
}

telemetryState& failureMode::getInstance()
{
    static failureMode singleton;
    return singleton;
}
