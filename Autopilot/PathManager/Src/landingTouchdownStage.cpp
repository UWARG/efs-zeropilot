#include "pathStateClasses.hpp"

void landingTouchdownStage::execute(pathManager* pathMgr)
{
    //throttleOff()
    //aligning heading
    cruisingState::_outputdata.desiredHeading = getFromTelemetry::telemetryInput.landingDirection;
    if(pathMgr -> isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& landingTouchdownStage::getInstance()
{
    static landingTouchdownStage singleton;
    return singleton;
}