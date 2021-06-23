/**
 * Interface header from telemetry manager (written in C++) to freertos (C).
 * Author(s): Dhruv Upadhyay
 */

#include "telemetryManagerInterface.h"
#include "telemetryManager.hpp"

static telemetryManager telemMng;
telemMan::_Telemetry_Manager_Cycle_Status telemManStatus;

bool TelemetryManagerInterfaceExecute(void) {

    do {
        telemMng.execute();
        telemManStatus = telemMng.getStatus();

        if (telemManStatus == telemMan::FAILURE_MODE) {
            // Something is quite wrong, need to switch over to safety
            return false;
        }
    } while (telemManStatus != telemMan::COMPLETED_CYCLE);
    return true;
}