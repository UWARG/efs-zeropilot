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
    first_instr = nullptr;
}

#else

PathModeSelector::PathModeSelector() : current_mode_enum {MODE_TAKEOFF} {
    current_mode = &TakeoffMode::getInstance(); 
    first_instr = nullptr;
}

#endif

void PathModeSelector::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in) {
    
    //Check whether the telemetry_in has an editflightPath command. If so, we enqueue it into our InstructionQueue
    if (telemetry_in.waypointModifyFlightPathCommand != NO_FLIGHT_PATH_EDIT) {        
        PathModeSelector::enqueueInstruction(telemetry_in);
    }

    current_mode->execute(telemetry_in, sensor_fusion_in, imu_data_in);
}

bool PathModeSelector::instructionQueueIsEmpty() {
    if (first_instr == nullptr) {
        return true;
    } else {
        return false;
    }
}

Telemetry_PIGO_t PathModeSelector::dequeueInstruction() {
    ///Assumes that Queue is non empty.
        instructionQueueNode* newFirstInstruction = first_instr->nextInstruction; //Store temporary new head
        Telemetry_PIGO_t instruction = first_instr->instruction;
        free(first_instr); //Not sure if this the right way to free variables
        first_instr = newFirstInstruction; //Update the head of the instructionQueue
        return instruction;
};

void PathModeSelector::enqueueInstruction(Telemetry_PIGO_t newInstruction) {
    if (first_instr == nullptr) { //If our Queue is empty, make it the first
        first_instr = new instructionQueueNode{};
        first_instr->instruction = newInstruction;
        first_instr->nextInstruction = nullptr;

    } else {
        instructionQueueNode* latestInstruction = first_instr;
        
        while (latestInstruction->nextInstruction != nullptr) {
            latestInstruction = latestInstruction->nextInstruction;
        };

        //Create new instructionQueueNode
        instructionQueueNode* newInstructionQueueNode = new instructionQueueNode{};
        newInstructionQueueNode->instruction = newInstruction;
        newInstructionQueueNode->nextInstruction = nullptr;
        
        //Add new instructionQueueNode to the entire queue
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
