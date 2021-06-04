#include "telemetryManagerInterface.h"
#include "telemetryManager.hpp"

static telemetryManager telemMng;
telemMan::_Telemetry_Manager_Cycle_Status telemManStatus;

void TelemetryManagerInterfaceExecute(void) {

    do {
        telemMng.execute();
        telemManStatus = telemMng.getStatus();

        if (telemManStatus == telemMan::FAILURE_MODE) {
            // Something is quite wrong, need to switch over to safety
        }
    } while (telemManStatus != telemMan::COMPLETED_CYCLE);
}