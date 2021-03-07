#include "pathStateClasses.hpp"
#include "landingManager.cpp"

void landingSlopeStage::execute(pathManager* pathMgr)
{
    if(sensorFusion::input.altitude <= (FLARE_ALTITUDE+getFromTelemetry::telemetryInput.stoppingAltitude)) //if less than flare altitude
    {
        pathMgr->stage = FLARE;

    }
    else
    {
        //aligning horizontal position using waypointManager get_next_directions
        landingTransitionStage::waypointStatus = landingTransitionStage::landingPath.get_next_directions(getFromTelemetry::telemetryInput, &cruisingState::_outputdata);
        //retrieving desired altitude for slope state and setting it 
        cruisingState::_outputdata.desiredAltitude = LandingManager::changingAltitude(getFromTelemetry::telemetryInput,landingTransitionStage::path.aimingPoint, landingTransitionStage::path.intersectionPoint, landingTransitionStage::path.stoppingPoint);
        //retrieving desired speed for approach speed and setting it
        crusingState::_outputdata.desiredSpeed = LandingManager::approachSpeed(getFromTelemetry::telemetryInput.windSpeed, getFromTelemetry::telemetryInput.ifPackage);
    }

    if(landingTransitionStage::waypointStatus == INVALID_PARAMETERS)
    {
        pathMgr -> isError = true;
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