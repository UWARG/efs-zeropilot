#include "cruisingModeStages.hpp"
#include "SensorFusion.hpp"
#include "landingMode.hpp"

/***********************************************************************************************************************
 * Static Member Variable Declarations
 **********************************************************************************************************************/

_WaypointManager_Data_In CruisingFlight::_input_data;
_WaypointManager_Data_Out CruisingFlight::_output_data;
_CruisingState_Telemetry_Return CruisingFlight::_return_to_ground;

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

void CruisingFlight::execute(CruisingMode* cruise_mode) {
    Telemetry_PIGO_t telem_data = cruise_mode->getTelemetryData();
    SFOutput_t sf_data = cruise_mode->getSensorFusionData();
    

    // Set waypoint manager input struct
    _input_data.track = sf_data.track; // Gets track
    _input_data.longitude = sf_data.longitude;
    _input_data.latitude = sf_data.latitude;
    _input_data.altitude = sf_data.altitude;


    Telemetry_PIGO_t instr;
    //Go over all instructions on the instruction queue
    while (!cruise_mode->getModeSelector()->instructionQueueIsEmpty()) {
        instr = cruise_mode->getModeSelector()->dequeueInstruction();

        _ModifyFlightPathErrorCode edit_error = editFlightPath(&instr, cruising_state_manager, waypoint_id_array); // Edit flight path if applicable
        _GetNextDirectionsErrorCode path_error = pathFollow(&instr, cruising_state_manager, _input_data, &_output_data, going_home, in_hold); // Get next direction or modify flight behaviour pattern
        setReturnValues(&_return_to_ground, cruising_state_manager, edit_error, path_error); // Set error codes
    }


    // Set output data to be sent back to PM coordinatedTurnsElevation state
    CoordinatedTurnInput_t coord_turn_input;
    coord_turn_input.currentHeadingTrack = sf_data.track;
    coord_turn_input.desiredHeadingTrack = _output_data.desiredTrack;
    coord_turn_input.accY = 0; // This will be set by coordinateTurnElevation state in PM

    AltitudeAirspeedInput_t alt_airspeed_input;
    alt_airspeed_input.currentAltitude = sf_data.altitude;
    alt_airspeed_input.desiredAltitude = _output_data.desiredAltitude;
    alt_airspeed_input.currentAirspeed = sf_data.airspeed;
    alt_airspeed_input.desiredAirspeed = _output_data.desiredAirspeed;

    _PassbyControl passby_control;
    passby_control.rollPercent = 0;
    passby_control.rollPassby = false;
    passby_control.rudderPercent = 0;
    passby_control.rudderPassby = false;
    passby_control.pitchPercent = 0;
    passby_control.pitchPassby = false;
    passby_control.throttlePercent = 0;
    passby_control.throttlePassby = false;

    cruise_mode->getModeSelector()->setCoordinatedTurnInput(coord_turn_input);
    cruise_mode->getModeSelector()->setAltitudeAirspeedInput(alt_airspeed_input);
    cruise_mode->getModeSelector()->setPassbyControl(passby_control);

    // Set output data to be sent back to PM commsWithTelemetry state
    cruise_mode->getModeSelector()->setEditingFlightPathErrorCode(_return_to_ground.editingFlightPathErrorCode);
    cruise_mode->getModeSelector()->setPathFollowingErrorCode(_return_to_ground.pathFollowingErrorCode);
    cruise_mode->getModeSelector()->setCurrentWaypointId(_return_to_ground.currentWaypointId);
    cruise_mode->getModeSelector()->setCurrentWaypointIndex(_return_to_ground.currentWaypointIndex);
    cruise_mode->getModeSelector()->setHomeBaseInitialized(_return_to_ground.homeBaseInitialized);
    cruise_mode->getModeSelector()->setIsLanded(false); // Set to default

    // Set mode to landing if appropriate
    if (telem_data.beginLanding) {
        cruise_mode->getModeSelector()->setCurrentMode(LandingMode::getInstance());
        cruise_mode->getModeSelector()->setCurrentModeEnum(MODE_LANDING);        
    }

    cruise_mode->getModeSelector()->setIsError(false); // Set error value
}

CruisingModeStageManager& CruisingFlight::getInstance() {
    static CruisingFlight singleton;
    return singleton;
}
