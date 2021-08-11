#include "takeoffModeStages.hpp"
#include "cruisingMode.hpp"
#include "SensorFusion.hpp"

/***********************************************************************************************************************
 * Static Member Variable Declarations
 **********************************************************************************************************************/

_LandingTakeoffInput TakeoffRollStage::_input;
_LandingTakeoffOutput TakeoffRollStage::_output;
_WaypointManager_Data_Out TakeoffRollStage::_waypoint_output;
_WaypointStatus TakeoffRollStage::_waypoint_status;
_PathData* TakeoffRollStage::_current_location;
_PathData* TakeoffRollStage::_path_array[1];
WaypointManager TakeoffRollStage::_takeoff_path;
_PathData TakeoffRollStage::_takeoff_point;

_LandingTakeoffInput TakeoffClimbStage::_input;
_LandingTakeoffOutput TakeoffClimbStage::_output;
_WaypointManager_Data_In TakeoffClimbStage::_waypoint_input;
_WaypointManager_Data_Out TakeoffClimbStage::_waypoint_output;


/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void TakeoffRollStage::execute(TakeoffMode* takeoff_mode) {
    // Reset output data
    takeoff_mode->resetPassby(&_output.controlDetails);

    //load in sensor fusion data and telemtry data into input structure
    Telemetry_PIGO_t telem_data = takeoff_mode->getTelemetryData();
    _input.telemetryData = &telem_data;

    SFOutput_t sf_data;
    #ifdef UNIT_TESTING
        sf_data.airspeed = 15; // So we transition to climb stage
    #else
        sf_data = takeoff_mode->getSensorFusionData();
    #endif
    _input.sensorOutput = &sf_data;

    //max throttle
    _output.controlDetails.throttlePassby = true;
    _output.controlDetails.throttlePercent = 100;

    //making sure landing points are only made once
    if (!takeoff_mode->getMadeTakeoffPoints()) {
        //creating waypoints
        _takeoff_point = LandingTakeoffManager::createTakeoffWaypoint(_input.sensorOutput->latitude, _input.sensorOutput->longitude, _input.sensorOutput->altitude, _input.telemetryData->takeoffDirectionHeading);
        _path_array[0] = _takeoff_path.initialize_waypoint(_takeoff_point.longitude, _takeoff_point.latitude, _takeoff_point.altitude, PATH_FOLLOW);
        
        //10 meters is added to the altitude of the currentLocation waypoint so that is is not ground level
        _current_location = _takeoff_path.initialize_waypoint(_input.sensorOutput->longitude, _input.sensorOutput->latitude, (_input.sensorOutput->altitude + 10), HOLD_WAYPOINT);

        //initializing flight path
        _waypoint_status = _takeoff_path.initialize_flight_path(_path_array, 1, _current_location);

        //set made madelandingPoints to true
        takeoff_mode->setMadeTakeoffPoints(true);
    }

    //if the airspeed is greater than the current desired rotation speed, set stage to climb
    if (_input.sensorOutput->airspeed > (LandingTakeoffManager::desiredRotationSpeed(takeoff_mode->getIsPackage()))) {
        takeoff_mode->setCurrentStage(&TakeoffClimbStage::getInstance());
        takeoff_mode->setCurrentStageEnum(TAKEOFF_CLIMB);
    }

    if (TakeoffRollStage::_waypoint_status != WAYPOINT_SUCCESS) {
        takeoff_mode->getModeSelector()->setIsError(true);
    }

    // Set output structs
    CoordinatedTurnInput_t coord_turn_input {};
    AltitudeAirspeedInput_t alt_airspeed_input {};

    _LandingTakeoffOutput output_holder; 
    _LandingTakeoffOutput * landing_takeoff_output = &output_holder;
    landing_takeoff_output = getControlOutput();

    IMU_Data_t imu_data = takeoff_mode->getImuData(); // Gets raw IMU data

    LandingTakeoffManager::translateLTSFCommandsToCoordTurns(*landing_takeoff_output, sf_data, imu_data, coord_turn_input, alt_airspeed_input);

    takeoff_mode->getModeSelector()->setCoordinatedTurnInput(coord_turn_input);
    takeoff_mode->getModeSelector()->setAltitdeAirspeedInput(alt_airspeed_input);
    takeoff_mode->getModeSelector()->setPassbyControl(_output.controlDetails);

    // Set output telemetry data
    takeoff_mode->getModeSelector()->setEditingFlightPathErrorCode(0); // Set to default
    takeoff_mode->getModeSelector()->setPathFollowingErrorCode(0); // Set to default
    takeoff_mode->getModeSelector()->setCurrentWaypointId(0); // Set to default
    takeoff_mode->getModeSelector()->setCurrentWaypointIndex(0); // Set to default
    takeoff_mode->getModeSelector()->setHomeBaseInitialized(false); // Set to default
    takeoff_mode->getModeSelector()->setIsLanded(false);
}

TakeoffModeStageManager& TakeoffRollStage::getInstance() {
    static TakeoffRollStage singleton;
    return singleton;
}

void TakeoffClimbStage::execute(TakeoffMode* takeoff_mode) {
    // Reset output data
    takeoff_mode->resetPassby(&_output.controlDetails);
    //load in sensor fusion data and telemtry data into input structure
    Telemetry_PIGO_t telem_data = takeoff_mode->getTelemetryData();
    _input.telemetryData = &telem_data;

       SFOutput_t sf_data;
    #ifdef UNIT_TESTING
        sf_data.altitude = 50; // So we transition to cruising mode
        TakeoffRollStage::_takeoff_point.altitude = 0; // Should initialize this to prevent undefined behaviour
    #else
        sf_data = takeoff_mode->getSensorFusionData();
    #endif
    _input.sensorOutput = &sf_data;

    if (_input.sensorOutput->altitude > (TakeoffRollStage::_takeoff_point.altitude + EXIT_TAKEOFF_ALTITUDE)) {
        takeoff_mode->getModeSelector()->setCurrentMode(CruisingMode::getInstance());
        takeoff_mode->getModeSelector()->setCurrentModeEnum(MODE_CRUISING); 
    } else {
        //setting sensorFusion input to waypoint data in
        _waypoint_input.latitude = _input.sensorOutput->latitude;
        _waypoint_input.longitude = _input.sensorOutput->longitude;
        _waypoint_input.altitude = _input.sensorOutput->altitude;
        _waypoint_input.track = _input.sensorOutput->track;

        TakeoffRollStage::_waypoint_status = TakeoffRollStage::_takeoff_path.get_next_directions(_waypoint_input, &_waypoint_output);
        _output = LandingTakeoffManager::translateWaypointCommands(_waypoint_output);

        _output.desiredAirspeed = LandingTakeoffManager::desiredClimbSpeed(takeoff_mode->getIsPackage());
        
        //pitching up
        _output.controlDetails.pitchPassby = true;
        _output.controlDetails.pitchPercent =  0.3;
    }

    //ensuring made takeoff points is reset
    takeoff_mode->setMadeTakeoffPoints(false);

    if (TakeoffRollStage::_waypoint_status != WAYPOINT_SUCCESS) {
        takeoff_mode->getModeSelector()->setIsError(true);
    }

    // Set output structs
    CoordinatedTurnInput_t coord_turn_input {};
    AltitudeAirspeedInput_t alt_airspeed_input {};

    _LandingTakeoffOutput output_holder; 
    _LandingTakeoffOutput * landing_takeoff_output = &output_holder;
    landing_takeoff_output = getControlOutput();

    IMU_Data_t imu_data = takeoff_mode->getImuData(); // Gets raw IMU data

    LandingTakeoffManager::translateLTSFCommandsToCoordTurns(*landing_takeoff_output, sf_data, imu_data, coord_turn_input, alt_airspeed_input);

    takeoff_mode->getModeSelector()->setCoordinatedTurnInput(coord_turn_input);
    takeoff_mode->getModeSelector()->setAltitdeAirspeedInput(alt_airspeed_input);
    takeoff_mode->getModeSelector()->setPassbyControl(_output.controlDetails);

    // Set output telemetry data
    takeoff_mode->getModeSelector()->setEditingFlightPathErrorCode(0); // Set to default
    takeoff_mode->getModeSelector()->setPathFollowingErrorCode(0); // Set to default
    takeoff_mode->getModeSelector()->setCurrentWaypointId(0); // Set to default
    takeoff_mode->getModeSelector()->setCurrentWaypointIndex(0); // Set to default
    takeoff_mode->getModeSelector()->setHomeBaseInitialized(false); // Set to default
    takeoff_mode->getModeSelector()->setIsLanded(false);

}

TakeoffModeStageManager& TakeoffClimbStage::getInstance() {
    static TakeoffClimbStage singleton;
    return singleton;
}





