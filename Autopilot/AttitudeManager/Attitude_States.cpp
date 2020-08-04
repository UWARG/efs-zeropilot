/*
* Attitude State CPP file
*/
#include "Attitude_States.hpp"

enum attitudeManagerState {
    sensorInput,
    pathManagerInput,
    PIDloop,
    sendToSafetyMode,
    failed
};
