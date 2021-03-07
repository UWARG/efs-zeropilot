#include "pathStateClasses.hpp"

void landingDecrabStage::execute(pathManager* pathMgr)
{
    if(sensorFusion::input.altitude<=(TOUCHDOWN_ALTITUDE+getFromTelemetry::telemetryInput.stoppingAltitude)) //altitude is 5 cm or less/ultrasonic sensor sensed 5cm or less
    {
        pathMgr->stage = TOUCHDOWN;
    }
    else
    {
        //maintain heading
        //maintain airspeed
        //throttle off
        cruisingState::_outputdata.desiredHeading = getFromTelemetry::telemetryInput.landingDirection;
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

pathManagerState& landingDecrabStage::getInstance()
{
    static landingDecrabStage singleton;
    return singleton;
}