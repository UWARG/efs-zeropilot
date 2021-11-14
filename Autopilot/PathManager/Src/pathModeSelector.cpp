#include "pathModeSelector.hpp"
#include "takeoffMode.hpp"

PathModeSelector* PathModeSelector::singleton = nullptr;

PathModeSelector* PathModeSelector::getInstance() {
    if (!singleton) {
        singleton = new PathModeSelector();
    }
    return singleton;
} 

// We call a different constructor depending on if we are running unit tests or not
#ifdef UNIT_TESTING

PathModeSelector::PathModeSelector() : current_mode_enum {MODE_TESTING_NONE} {
    current_mode = nullptr; 
}

#else

PathModeSelector::PathModeSelector() : current_mode_enum {MODE_TAKEOFF} {
    current_mode = &TakeoffMode::getInstance(); 
}

#endif

void PathModeSelector::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in) {
    current_mode->execute(telemetry_in, sensor_fusion_in, imu_data_in);
}

void PathModeSelector::setAltitdeAirspeedInput(AltitudeAirspeedInput_t alt_airspeed_input) {     
    memcpy(&altitude_airspeed_input, &alt_airspeed_input, sizeof(AltitudeAirspeedInput_t));
}

void PathModeSelector::setCoordinatedTurnInput(CoordinatedTurnInput_t coord_turn_input) {     
    memcpy(&coordinated_turn_input, &coord_turn_input, sizeof(CoordinatedTurnInput_t));
}

void PathModeSelector::setPassbyControl(_PassbyControl passby) {
    memcpy(&passby_control_output, &passby, sizeof(_PassbyControl));
}

