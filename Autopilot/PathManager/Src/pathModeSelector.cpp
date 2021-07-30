#include "pathModeSelector.hpp"
#include "cruisingMode.hpp"

PathModeSelector* PathModeSelector::singleton = nullptr;

PathModeSelector* PathModeSelector::getInstance() {
    if (!singleton) {
        singleton = new PathModeSelector();
    }
    return singleton;
} 

#ifdef UNIT_TESTING

PathModeSelector::PathModeSelector() : current_mode_enum {MODE_TESTING_NONE} {
    current_mode = nullptr; // Initialize to takeoff in real. Initialize to cruising for first test
}

#else

PathModeSelector::PathModeSelector() : current_mode_enum {MODE_CRUISING} {
    current_mode = &CruisingMode::getInstance(); // Initialize to takeoff in real. Initialize to cruising for first test
}

#endif

void PathModeSelector::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in) {
    current_mode->execute(telemetry_in, sensor_fusion_in, imu_data_in);
}

void PathModeSelector::setAltitdeAirspeedInput(AltitudeAirspeedInput_t alt_airspeed_input) {     
    altitude_airspeed_input.currentAltitude = alt_airspeed_input.currentAltitude; 
    altitude_airspeed_input.desiredAltitude = alt_airspeed_input.desiredAltitude; 
    altitude_airspeed_input.currentAirspeed = alt_airspeed_input.currentAirspeed; 
    altitude_airspeed_input.desiredAirspeed = alt_airspeed_input.desiredAirspeed; 
}

void PathModeSelector::setCoordinatedTurnInput(CoordinatedTurnInput_t coord_turn_input) {     
    coordinated_turn_input.currentHeadingTrack = coord_turn_input.currentHeadingTrack; 
    coordinated_turn_input.desiredHeadingTrack = coord_turn_input.desiredHeadingTrack; 
    coordinated_turn_input.accY = coord_turn_input.accY; 
}

void PathModeSelector::setPassbyControl(_PassbyControl passby) {
    passby_control_output.rollPercent = passby.rollPercent;
    passby_control_output.rollPassby = passby.rollPassby;

    passby_control_output.rudderPercent = passby.rudderPercent;
    passby_control_output.rudderPassby = passby.rudderPassby;

    passby_control_output.pitchPercent = passby.pitchPercent;
    passby_control_output.pitchPassby = passby.pitchPassby;

    passby_control_output.throttlePercent = passby.throttlePercent;
    passby_control_output.throttlePassby = passby.throttlePassby;
}

