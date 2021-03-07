#include "pathStateClasses.hpp"

void landingTouchdownStage::execute(pathManager* pathMgr)
{
    //throttleOff()
    cruisingState::_outputdata.desiredHeading = getFromTelemetry::telemetryInput.landingDirection;
    cruisingState::_outputdata.desiredAltitude = LandingManager:throttleOff();
    if(pathMgr->isError)
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