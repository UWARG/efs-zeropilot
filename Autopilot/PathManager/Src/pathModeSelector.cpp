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
    first_flight_path_edit_instr = nullptr;
}

#else

PathModeSelector::PathModeSelector() : current_mode_enum {MODE_TAKEOFF} {
    current_mode = &TakeoffMode::getInstance(); 
    first_flight_path_edit_instr = nullptr;
}

#endif

void PathModeSelector::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in) {
    
    // Check whether the telemetry_in has an editflightPath command. If so, we enqueue it into our InstructionQueue
    if (telemetry_in.waypointModifyFlightPathCommand != NO_FLIGHT_PATH_EDIT) {        
        enqueueFlightPathEditInstructions(telemetry_in);
    }

    current_mode->execute(telemetry_in, sensor_fusion_in, imu_data_in);
}

bool PathModeSelector::flightPathEditInstructionsIsEmpty() {
    if (first_flight_path_edit_instr == nullptr) {
        return true;
    } else {
        return false;
    }
}

int PathModeSelector::checkflightPathEditInstructionsLength() {
    int count = 0;
    if (flightPathEditInstructionsIsEmpty()) {
        return count;
    } else {
        flightPathEditInstructionNode* latestInstruction = first_flight_path_edit_instr;
        
        while (latestInstruction != nullptr) {
            latestInstruction = latestInstruction->nextInstruction;
            count += 1;
        };
        return count;
    }

}

Telemetry_PIGO_t PathModeSelector::dequeueflightPathEditInstructions() {
    // Assumes that Queue is non empty.
    flightPathEditInstructionNode* old_first_instruction = first_flight_path_edit_instr;
    Telemetry_PIGO_t instruction = old_first_instruction->instruction;
    // old_first_instruction->nextInstruction = nullptr;
    first_flight_path_edit_instr = first_flight_path_edit_instr->nextInstruction; //Update the head of the queue
    delete old_first_instruction; //Delete the old node
    return instruction;
};

void PathModeSelector::enqueueFlightPathEditInstructions(Telemetry_PIGO_t newInstruction) {
    if (flightPathEditInstructionsIsEmpty()) { //If our Queue is empty (i.e. there are no editflightpath instructions), make it the first
        first_flight_path_edit_instr = new flightPathEditInstructionNode{};
        first_flight_path_edit_instr->instruction = newInstruction;
        first_flight_path_edit_instr->nextInstruction = nullptr;
    } else {
        flightPathEditInstructionNode* latestInstruction = first_flight_path_edit_instr;
        
        while (latestInstruction->nextInstruction != nullptr) {
            latestInstruction = latestInstruction->nextInstruction;
        };

        // Create new instructionQueueNode
        flightPathEditInstructionNode* newInstructionQueueNode = new flightPathEditInstructionNode{};
        newInstructionQueueNode->instruction = newInstruction;
        newInstructionQueueNode->nextInstruction = nullptr;
        
        // Add new instructionQueueNode to the entire queue
        latestInstruction->nextInstruction = newInstructionQueueNode;
    }
};

void PathModeSelector::setAltitudeAirspeedInput(AltitudeAirspeedInput_t alt_airspeed_input) {     
    memcpy(&altitude_airspeed_input, &alt_airspeed_input, sizeof(AltitudeAirspeedInput_t));
}

void PathModeSelector::setCoordinatedTurnInput(CoordinatedTurnInput_t coord_turn_input) {     
    memcpy(&coordinated_turn_input, &coord_turn_input, sizeof(CoordinatedTurnInput_t));
}

void PathModeSelector::setPassbyControl(_PassbyControl passby) {
    memcpy(&passby_control_output, &passby, sizeof(_PassbyControl));
}
