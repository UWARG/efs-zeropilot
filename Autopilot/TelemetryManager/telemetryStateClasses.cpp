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
    telemetryMgr->setState(groundDataPathMode::getInstance());
}

telemetryState& initialMode::getInstance()
{
    static initialMode singleton;
    return singleton;
}

void groundDataPathMode::execute(telemetryManager* telemetryMgr)
{
    //obtain data from ground
    //decode data with Mavlink
    //pass data to path manager
    //read data out of path manager
    //detect if data is valid, load into bool dataValid (valid=true, invalid=false)
    if(telemetryMgr->dataValid)
    {
        counter::cycleCounter=0;
        //detect if data contains error, load into bool dataError (error=true, no error=false)
        if(telemetryMgr->dataError)
        {
            telemetryMgr->setState(errorReportMode::getInstance());
        }
        else
        {
            telemetryMgr->setState(regularReportMode::getInstance());
        }
    }
    else
    {
        counter::cycleCounter++;
        if(counter::cycleCounter>=5)
        {
            telemetryMgr->setState(errorReportMode::getInstance());
        }
        else
        {
            telemetryMgr->setState(regularReportMode::getInstance());
        }
    }
}

telemetryState& groundDataPathMode::getInstance()
{
    static groundDataPathMode singleton;
    return singleton;
}

void errorReportMode::execute(telemetryManager* telemetryMgr)
{
    //form error report
    //encode data with Mavlink
    //report error to ground
    clock_t time = clock();
    while((clock()-time)<CLK_TIME_CONSTANT){}
    telemetryMgr->setState(initialMode::getInstance());
}

telemetryState& errorReportMode::getInstance()
{
    static errorReportMode singleton;
    return singleton;
}

void regularReportMode:: execute(telemetryManager* telemetryMgr)
{
    //form regular report
    //encode data with Mavlink
    //send data to ground
    clock_t time = clock();
    while((clock()-time)<CLK_TIME_CONSTANT){}
    telemetryMgr->setState(groundDataPathMode::getInstance());
}

telemetryState& regularReportMode::getInstance()
{
    static regularReportMode singleton;
    return singleton;
}
