#include "telemetryStateClasses.hpp"
#include <time.h>

//definitions
namespace counter
{
    int cycleCounter=0;
}
const float CLK_TIME_CONSTANT=1;

void initialMode::execute(telemetryManager* telemetryMgr)
{
    //initial mode
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

    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    telemetryMgr -> setState(decodeDataMode::getInstance());
}

telemetryState& obtainDataMode::getInstance()
{
    static obtainDataMode singleton;
    return singleton;
}

void decodeDataMode::execute(telemetryManager* telemetryMgr)
{
    //decode data with Mavlink
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    telemetryMgr -> setState(passToPathMode::getInstance());
}

telemetryState& decodeDataMode::getInstance()
{
    static decodeDataMode singleton;
    return singleton;
}

void passToPathMode::execute(telemetryManager* telemetryMgr)
{
    //pass data to path manager
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    telemetryMgr -> setState(readFromPathMode::getInstance());
}

telemetryState& passToPathMode::getInstance()
{
    static passToPathMode singleton;
    return singleton;
}

void readFromPathMode::execute(telemetryManager* telemetryMgr)
{
    //read data out of path manager
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    telemetryMgr -> setState(analyzeDataMode::getInstance());
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
<<<<<<< Updated upstream:Autopilot/TelemetryManager/telemetryStateClasses.cpp
        counter::cycleCounter=0;
=======
        telemetryMgr -> setState(failureMode::getInstance());
    }
    telemetryMgr -> setState(reportMode::getInstance());
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
>>>>>>> Stashed changes:TelemetryManager/telemetryStateClasses.cpp
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
<<<<<<< Updated upstream:Autopilot/TelemetryManager/telemetryStateClasses.cpp
        counter::cycleCounter++;
        if(counter::cycleCounter>=5)
=======
        telemetryMgr -> cycleCounter++;
        if(telemetryMgr -> cycleCounter >= 5) //if data invalid more than 5 times, send error report
>>>>>>> Stashed changes:TelemetryManager/telemetryStateClasses.cpp
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
    telemetryMgr -> setState(encodeDataMode::getInstance());
}

telemetryState& reportMode::getInstance()
{
    static reportMode singleton;
    return singleton;
}

void encodeDataMode::execute(telemetryManager* telemetryMgr)
{
<<<<<<< Updated upstream:Autopilot/TelemetryManager/telemetryStateClasses.cpp
    //form error report
    //encode data with Mavlink
    //report error to ground
    clock_t time = clock();
    while((clock()-time)<CLK_TIME_CONSTANT){}
    telemetryMgr->setState(initialMode::getInstance());
=======
    //encode data with mavlink
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }
    telemetryMgr -> setState(sendDataMode::getInstance());
>>>>>>> Stashed changes:TelemetryManager/telemetryStateClasses.cpp
}

telemetryState& encodeDataMode::getInstance()
{
    static encodeDataMode singleton;
    return singleton;
}

void sendDataMode::execute(telemetryManager* telemetryMgr)
{
    //send data to ground
    // wait for 1 clk time
    if(telemetryMgr -> fatalFail)
    {
        telemetryMgr -> setState(failureMode::getInstance());
    }

    clock_t time = clock();
<<<<<<< Updated upstream:Autopilot/TelemetryManager/telemetryStateClasses.cpp
    while((clock()-time)<CLK_TIME_CONSTANT){}
    telemetryMgr->setState(groundDataPathMode::getInstance());
=======
    while( (clock() - time) < telemetryMgr -> CLK_TIME_CONSTANT){} //clk time pass
    if(telemetryMgr -> regularReport)
    {
        telemetryMgr -> setState(obtainDataMode::getInstance());
    }
    else
    {
        telemetryMgr -> setState(initialMode::getInstance());
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
>>>>>>> Stashed changes:TelemetryManager/telemetryStateClasses.cpp
}

telemetryState& failureMode::getInstance()
{
    static failureMode singleton;
    return singleton;
}
