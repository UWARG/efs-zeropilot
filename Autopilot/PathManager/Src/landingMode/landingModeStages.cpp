#include "landingModeStages.hpp"
#include "SensorFusion.hpp"

/***********************************************************************************************************************
 * Static Member Variable Declarations
 **********************************************************************************************************************/

_LandingTakeoffInput LandingTransitionStage::_input;
_LandingTakeoffOutput LandingTransitionStage::_output;
_WaypointManager_Data_In LandingTransitionStage::_waypoint_input;
_WaypointManager_Data_Out LandingTransitionStage::_waypoint_output;
_WaypointStatus LandingTransitionStage::_waypoint_status;
_PathData* LandingTransitionStage::_current_location;
_PathData* LandingTransitionStage::_path_array[3];
WaypointManager LandingTransitionStage::_landing_path;
_LandingPath LandingTransitionStage::_path;

_LandingTakeoffInput LandingSlopeStage::_input;
_LandingTakeoffOutput LandingSlopeStage::_output;
_WaypointManager_Data_In LandingSlopeStage::_waypoint_input;
_WaypointManager_Data_Out LandingSlopeStage::_waypoint_output;

_LandingTakeoffInput LandingFlareStage::_input;
_LandingTakeoffOutput LandingFlareStage::_output;
_WaypointManager_Data_In LandingFlareStage::_waypoint_input;
_WaypointManager_Data_Out LandingFlareStage::_waypoint_output;

_LandingTakeoffInput LandingDecrabStage::_input;
_LandingTakeoffOutput LandingDecrabStage::_output;
_WaypointManager_Data_Out LandingDecrabStage::_waypoint_output;

_LandingTakeoffInput LandingTouchdownStage::_input;
_LandingTakeoffOutput LandingTouchdownStage::_output;
_WaypointManager_Data_Out LandingTouchdownStage::_waypoint_output;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void LandingTransitionStage::execute(LandingMode* land_mode) {
    // Reset output data
    land_mode->resetPassby(&_output.controlDetails);

    //load in sensor fusion data and telemtry data into input structure
    Telemetry_PIGO_t telem_data = land_mode->getTelemetryData();
    _input.telemetryData = &telem_data;

    SFOutput_t sf_data;
    #ifdef UNIT_TESTING
        sf_data.track = 1;
    #else
        sf_data = land_mode->getSensorFusionData();
    #endif
    _input.sensorOutput = &sf_data;

    //making sure landing points are only made once
    if(!land_mode->getMadeLandingPoints()) {
        //requires data structure that dhruv wants to use
        _path = LandingTakeoffManager::createSlopeWaypoints(*_input.telemetryData, _input.sensorOutput->altitude);

        //creating waypoints
        _path_array[0] = _landing_path.initialize_waypoint(_input.sensorOutput->longitude, _input.sensorOutput->latitude, _input.sensorOutput->altitude, PATH_FOLLOW);
        _path_array[1] = _landing_path.initialize_waypoint(_path.intersectionPoint.longitude, _path.intersectionPoint.latitude, _path.intersectionPoint.altitude, PATH_FOLLOW);
        _path_array[2] = _landing_path.initialize_waypoint(_path.aimingPoint.longitude, _path.aimingPoint.latitude, _path.aimingPoint.altitude, PATH_FOLLOW);
        _path_array[3] = _landing_path.initialize_waypoint(_path.stoppingPoint.longitude, _path.stoppingPoint.latitude, _path.stoppingPoint.altitude, PATH_FOLLOW);
        _current_location = _landing_path.initialize_waypoint(_input.sensorOutput->longitude, _input.sensorOutput->latitude, _input.sensorOutput->altitude, HOLD_WAYPOINT, 20);

        //initializing flight path
        _waypoint_status = _landing_path.initialize_flight_path(_path_array, 4, _current_location);

        //set made madelandingPoints to true
        land_mode->setMadeLandingPoints(true);
    }

    //translate sensor data to waypoint data in struct
    _waypoint_input.latitude = _input.sensorOutput->latitude;
    _waypoint_input.longitude = _input.sensorOutput->longitude;
    _waypoint_input.altitude = _input.sensorOutput->altitude;
    _waypoint_input.track = _input.sensorOutput->track;

    //follow the landing waypoints
    _waypoint_status = _landing_path.get_next_directions(_waypoint_input, &_waypoint_output);

    //translate waypoint commands into landing output structure
    _output = LandingTakeoffManager::translateWaypointCommands(_waypoint_output);

    //maintaining altitude
    _output.desiredAirspeed = CRUISING_AIRSPEED;

    //calculating the difference in heading to detect if finished turning (2 differences possible)
    double difference_in_heading_1 = _input.telemetryData->stoppingDirectionHeading - _input.sensorOutput->track;
    double difference_in_heading_2 = _input.sensorOutput->track - _input.telemetryData->stoppingDirectionHeading;

    //making sure both headings are positive
    if(difference_in_heading_1 < 0) {difference_in_heading_1 += 360;}
    if(difference_in_heading_2 < 0) {difference_in_heading_2 += 360;}

    //if the smaller heading is less than 30 degrees, set stage to slope
    if((fabs(difference_in_heading_1) <= 80) || (fabs(difference_in_heading_2) <= 80)) {
        land_mode->setCurrentStage(&LandingSlopeStage::getInstance());
        land_mode->setCurrentStageEnum(LANDING_SLOPE);
    }

    if(LandingTransitionStage::_waypoint_status != WAYPOINT_SUCCESS) {
        land_mode->getModeSelector()->setIsError(true);
    }

    // Set output structs
    CoordinatedTurnInput_t coord_turn_input {};
    AltitudeAirspeedInput_t alt_airspeed_input {};

    _LandingTakeoffOutput output_holder; //had some problem with initializing the passby details, defining the pointer like this fixed it
    _LandingTakeoffOutput * landing_takeoff_output = &output_holder;
    landing_takeoff_output = getControlOutput();

    IMU_Data_t imu_data = land_mode->getImuData(); // Gets raw IMU data

    LandingTakeoffManager::translateLTSFCommandsToCoordTurns(*landing_takeoff_output, sf_data, imu_data, coord_turn_input, alt_airspeed_input);

    land_mode->getModeSelector()->setCoordinatedTurnInput(coord_turn_input);
    land_mode->getModeSelector()->setAltitudeAirspeedInput(alt_airspeed_input);
    land_mode->getModeSelector()->setPassbyControl(_output.controlDetails);

    // Set output telemetry data
    land_mode->getModeSelector()->setEditingFlightPathErrorCode(0); // Set to default
    land_mode->getModeSelector()->setPathFollowingErrorCode(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointId(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointIndex(0); // Set to default
    land_mode->getModeSelector()->setHomeBaseInitialized(false); // Set to default
    land_mode->getModeSelector()->setIsLanded(false);
}

LandingModeStageManager& LandingTransitionStage::getInstance() {
    static LandingTransitionStage singleton;
    return singleton;
}

void LandingSlopeStage::execute(LandingMode* land_mode) {
    // Reset output data
    land_mode->resetPassby(&_output.controlDetails);

    //load in sensor fusion data and telemtry data into input structure
    Telemetry_PIGO_t telem_data = land_mode->getTelemetryData();
    _input.telemetryData = &telem_data;

    SFOutput_t sf_data;
    sf_data = land_mode->getSensorFusionData();
    _input.sensorOutput = &sf_data;

    if(_input.sensorOutput->altitude <= (FLARE_ALTITUDE + _input.telemetryData->stoppingAltitude)) { //if less than flare altitude
        land_mode->setCurrentStage(&LandingFlareStage::getInstance());
        land_mode->setCurrentStageEnum(LANDING_FLARE);
    }
    else {
        //setting sensorFusion input to waypoint data in
        _waypoint_input.latitude = _input.sensorOutput->latitude;
        _waypoint_input.longitude = _input.sensorOutput->longitude;
        _waypoint_input.altitude = _input.sensorOutput->altitude;
        _waypoint_input.track = _input.sensorOutput->track;

        //aligning horizontal position using waypointManager get_next_directions
        LandingTransitionStage::_waypoint_status = LandingTransitionStage::_landing_path.get_next_directions(_waypoint_input, &_waypoint_output);

        //translate waypoint ouput data into landing data
        _output = LandingTakeoffManager::translateWaypointCommands(_waypoint_output);

        //retrieving desired altitude for slope state and setting it
        _output.desiredAltitude = LandingTakeoffManager::changingAltitude(*_input.sensorOutput, LandingTransitionStage::_path.aimingPoint, LandingTransitionStage::_path.intersectionPoint, LandingTransitionStage::_path.stoppingPoint);

        //retrieving desired speed for approach speed and setting it
        _output.desiredAirspeed = LandingTakeoffManager::approachSpeed(land_mode->getIsPackage());
    }

    if(LandingTransitionStage::_waypoint_status != WAYPOINT_SUCCESS) {
        land_mode->getModeSelector()->setIsError(true);
    }

    // Set output structs
    CoordinatedTurnInput_t coord_turn_input {};
    AltitudeAirspeedInput_t alt_airspeed_input {};

    _LandingTakeoffOutput output_holder; //had some problem with initializing the passby details, defining the pointer like this fixed it
    _LandingTakeoffOutput * landing_takeoff_output = &output_holder;
    landing_takeoff_output = getControlOutput();

    IMU_Data_t imu_data = land_mode->getImuData(); // Gets raw IMU data

    LandingTakeoffManager::translateLTSFCommandsToCoordTurns(*landing_takeoff_output, sf_data, imu_data, coord_turn_input, alt_airspeed_input);

    land_mode->getModeSelector()->setCoordinatedTurnInput(coord_turn_input);
    land_mode->getModeSelector()->setAltitudeAirspeedInput(alt_airspeed_input);
    land_mode->getModeSelector()->setPassbyControl(_output.controlDetails);

    // Set output telemetry data
    land_mode->getModeSelector()->setEditingFlightPathErrorCode(0); // Set to default
    land_mode->getModeSelector()->setPathFollowingErrorCode(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointId(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointIndex(0); // Set to default
    land_mode->getModeSelector()->setHomeBaseInitialized(false); // Set to default
    land_mode->getModeSelector()->setIsLanded(false);
}

LandingModeStageManager& LandingSlopeStage::getInstance() {
    static LandingSlopeStage singleton;
    return singleton;
}

void LandingFlareStage::execute(LandingMode* land_mode) {
    // Reset output data
    land_mode->resetPassby(&_output.controlDetails);

    //load in sensor fusion data and telemtry data into input structure
    Telemetry_PIGO_t telem_data = land_mode->getTelemetryData();
    _input.telemetryData = &telem_data;

    SFOutput_t sf_data;
    sf_data = land_mode->getSensorFusionData();
    _input.sensorOutput = &sf_data;

    if(_input.sensorOutput->altitude <= (DECRAB_ALTITUDE + _input.telemetryData->stoppingAltitude)) //altitude is below 70 cm
    {
        land_mode->setCurrentStage(&LandingDecrabStage::getInstance());
        land_mode->setCurrentStageEnum(LANDING_DECRAB);
    }
    else
    {
        //setting sensorFusion input to waypoint data in
        _waypoint_input.latitude = _input.sensorOutput->latitude;
        _waypoint_input.longitude = _input.sensorOutput->longitude;
        _waypoint_input.altitude = _input.sensorOutput->altitude;
        _waypoint_input.track = _input.sensorOutput->track;

        //maintaining horizontal position
        LandingTransitionStage::_waypoint_status = LandingTransitionStage::_landing_path.get_next_directions(_waypoint_input, &_waypoint_output);

        //translating waypoint data into landing data
        _output = LandingTakeoffManager::translateWaypointCommands(_waypoint_output);

        //throttle off
        _output.controlDetails.throttlePassby = true;
        _output.controlDetails.throttlePercent = 0;

        //maintaing speed for flare attitude
        _output.desiredAirspeed = LandingTakeoffManager::slowFlightSpeed(land_mode->getIsPackage());
    }

    if(LandingTransitionStage::_waypoint_status != WAYPOINT_SUCCESS)
    {
        land_mode->getModeSelector()->setIsError(true);
    }

    // Set output structs
    CoordinatedTurnInput_t coord_turn_input {};
    AltitudeAirspeedInput_t alt_airspeed_input {};

    _LandingTakeoffOutput output_holder; //had some problem with initializing the passby details, defining the pointer like this fixed it
    _LandingTakeoffOutput * landing_takeoff_output = &output_holder;
    landing_takeoff_output = getControlOutput();

    IMU_Data_t imu_data = land_mode->getImuData(); // Gets raw IMU data

    LandingTakeoffManager::translateLTSFCommandsToCoordTurns(*landing_takeoff_output, sf_data, imu_data, coord_turn_input, alt_airspeed_input);

    land_mode->getModeSelector()->setCoordinatedTurnInput(coord_turn_input);
    land_mode->getModeSelector()->setAltitudeAirspeedInput(alt_airspeed_input);
    land_mode->getModeSelector()->setPassbyControl(_output.controlDetails);

    // Set output telemetry data
    land_mode->getModeSelector()->setEditingFlightPathErrorCode(0); // Set to default
    land_mode->getModeSelector()->setPathFollowingErrorCode(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointId(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointIndex(0); // Set to default
    land_mode->getModeSelector()->setHomeBaseInitialized(false); // Set to default
    land_mode->getModeSelector()->setIsLanded(false);
}

LandingModeStageManager& LandingFlareStage::getInstance() {
    static LandingFlareStage singleton;
    return singleton;
}

void LandingDecrabStage::execute(LandingMode* land_mode) {
    // Reset output data
    land_mode->resetPassby(&_output.controlDetails);

    //load in sensor fusion data and telemtry data into input structure
    Telemetry_PIGO_t telem_data = land_mode->getTelemetryData();
    _input.telemetryData = &telem_data;

    SFOutput_t sf_data;
    sf_data = land_mode->getSensorFusionData();
    _input.sensorOutput = &sf_data;

    if(_input.sensorOutput->altitude <= (TOUCHDOWN_ALTITUDE + _input.telemetryData->stoppingAltitude)) { //altitude is 5 cm or less/ultrasonic sensor sensed 5cm or less 
        land_mode->setCurrentStage(&LandingTouchdownStage::getInstance());
        land_mode->setCurrentStageEnum(LANDING_TOUCHDOWN);
    } else {
        //align heading with landing direction
        _output.desiredHeading = _input.telemetryData->stoppingDirectionHeading;
        _output.useHeading = true;

        //retrieving desired slow flight speed
        _output.desiredAirspeed = LandingTakeoffManager::slowFlightSpeed(land_mode->getIsPackage());

        //throttle off
        _output.controlDetails.throttlePassby = true;
        _output.controlDetails.throttlePercent = 0;
    }

    // Set output structs
    CoordinatedTurnInput_t coord_turn_input {};
    AltitudeAirspeedInput_t alt_airspeed_input {};

    _LandingTakeoffOutput output_holder; //had some problem with initializing the passby details, defining the pointer like this fixed it
    _LandingTakeoffOutput * landing_takeoff_output = &output_holder;
    landing_takeoff_output = getControlOutput();

    IMU_Data_t imu_data = land_mode->getImuData(); // Gets raw IMU data

    LandingTakeoffManager::translateLTSFCommandsToCoordTurns(*landing_takeoff_output, sf_data, imu_data, coord_turn_input, alt_airspeed_input);

    land_mode->getModeSelector()->setCoordinatedTurnInput(coord_turn_input);
    land_mode->getModeSelector()->setAltitudeAirspeedInput(alt_airspeed_input);
    land_mode->getModeSelector()->setPassbyControl(_output.controlDetails);

    land_mode->getModeSelector()->setIsError(false); // Set error value

    // Set output telemetry data
    land_mode->getModeSelector()->setEditingFlightPathErrorCode(0); // Set to default
    land_mode->getModeSelector()->setPathFollowingErrorCode(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointId(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointIndex(0); // Set to default
    land_mode->getModeSelector()->setHomeBaseInitialized(false); // Set to default
    land_mode->getModeSelector()->setIsLanded(false);
}

LandingModeStageManager& LandingDecrabStage::getInstance() {
    static LandingDecrabStage singleton;
    return singleton;
}

void LandingTouchdownStage::execute(LandingMode* land_mode) {
    // Reset output data
    land_mode->resetPassby(&_output.controlDetails);

    //load in sensor fusion data and telemtry data into input structure
    Telemetry_PIGO_t telem_data = land_mode->getTelemetryData();
    _input.telemetryData = &telem_data;

    SFOutput_t sf_data;
    sf_data = land_mode->getSensorFusionData();
    _input.sensorOutput = &sf_data;

    //throttle off
    _output.controlDetails.throttlePassby = true;
    _output.controlDetails.throttlePercent = 0;

    //aligning heading
    _output.desiredHeading = _input.telemetryData->stoppingDirectionHeading;
    _output.useHeading = true;

    //ensure made landing points is reset
    land_mode->setMadeLandingPoints(false);

    // Set output structs
    CoordinatedTurnInput_t coord_turn_input {};
    AltitudeAirspeedInput_t alt_airspeed_input {};

    _LandingTakeoffOutput output_holder; //had some problem with initializing the passby details, defining the pointer like this fixed it
    _LandingTakeoffOutput * landing_takeoff_output = &output_holder;
    landing_takeoff_output = getControlOutput();

    IMU_Data_t imu_data = land_mode->getImuData(); // Gets raw IMU data

    LandingTakeoffManager::translateLTSFCommandsToCoordTurns(*landing_takeoff_output, sf_data, imu_data, coord_turn_input, alt_airspeed_input);

    land_mode->getModeSelector()->setCoordinatedTurnInput(coord_turn_input);
    land_mode->getModeSelector()->setAltitudeAirspeedInput(alt_airspeed_input);
    land_mode->getModeSelector()->setPassbyControl(_output.controlDetails);

    land_mode->getModeSelector()->setIsError(false); // Set error value

    /*
     
     
     TRANSITION TO TAXI STATE CONDITION


    */

    // Set output telemetry data
    land_mode->getModeSelector()->setEditingFlightPathErrorCode(0); // Set to default
    land_mode->getModeSelector()->setPathFollowingErrorCode(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointId(0); // Set to default
    land_mode->getModeSelector()->setCurrentWaypointIndex(0); // Set to default
    land_mode->getModeSelector()->setHomeBaseInitialized(false); // Set to default
    
    /*
     
     
     FIND CONDITION WHERE THIS SHOULD BE TRUE


    */
    land_mode->getModeSelector()->setIsLanded(false);
}

LandingModeStageManager& LandingTouchdownStage::getInstance() {
    static LandingTouchdownStage singleton;
    return singleton;
}
