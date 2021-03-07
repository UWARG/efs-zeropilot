#include "pathStateClasses.hpp"
#include "landingManager.cpp"

void landingSlopeStage::execute(pathManager* pathMgr)
{
    if(sensorFusion::input.altitude<=(FLARE_ALTITUDE+getFromTelemetry::telemetryInput.stoppingAltitude))//altitude<=(FLARE_ALTITUDE+the ground altitude
    {
        pathMgr->stage = FLARE;

    }
    else
    {
        landingTransitionStage::waypointStatus = landingTransitionStage::landingPath.get_next_directions(getFromTelemetry::telemetryInput, &cruisingState::_outputdata);
        cruisingState::_outputdata.desiredAltitude = LandingManager::changingAltitude(getFromTelemetry::telemetryInput,landingTransitionStage::path.aimingPoint, landingTransitionStage::path.intersectionPoint, landingTransitionStage::path.stoppingPoint);
        crusingState::_outputdata.desiredSpeed = LandingManager::approachSpeed(getFromTelemetry::telemetryInput.windSpeed, getFromTelemetry::telemetryInput.ifPackage);
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

pathManagerState& landingSlopeStage::getInstance()
{
    static landingSlopeStage singleton;
    return singleton;
};