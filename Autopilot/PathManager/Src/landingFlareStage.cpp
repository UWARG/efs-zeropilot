#include "pathStateClasses.hpp"
#include "landingManager.cpp"

void landingFlareStage::execute(pathManager* pathMgr)
{
    if(sensorFusion::input.altitude<=(DECRAB_ALTITUDE+getFromTelemetry::telemetryInput.stoppingAltitude)) //altitude is below 70 cm
    {
        pathMgr->stage = DECRAB;
    }
    else
    {
        //should maintain heading 
        landingTransitionStage::waypointStatus = landingTransitionStage::landingPath.get_next_directions(getFromTelemetry::telemetryInput, &cruisingState::_outputdata);
        //throttleOff()
        cruisingState::_outputdata.desiredSpeed = LandingManager::slowFlightSpeed(getFromTelemetry::telemetryInput.windSpeed, getFromTelemetry::telemetryInput.ifPackage);
    }   

    if(landingTransitionStage::waypointStatus == INVALID_PARAMETERS)
    {
        pathMgr->isError = true;
    }
    

    if(pathMgr->isError)
    {
        pathMgr -> setState(fatalFailureMode::getInstance());
    }
    else
    {
        pathMgr -> setState(coordinateTurnElevation::getInstance());
    }
}

pathManagerState& landingFlareStage::getInstance()
{
    static landingFlareStage singleton;
    return singleton;
}