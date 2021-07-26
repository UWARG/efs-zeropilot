#include "pathModeSelector.hpp"
#include "cruisingMode.hpp"

#include <iostream>
using namespace std;

PathModeSelector::PathModeSelector() : current_mode_enum {MODE_CRUISING} {

    current_mode = &CruisingMode::getInstance(); // Initialize to takeoff in real. Initialize to cruising for first test
}

PathModeSelector& PathModeSelector::getInstance() {
    static PathModeSelector singleton;
    return singleton;
} 



void PathModeSelector::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in) {

    cout << "Mode Selector: start" << endl;

    current_mode->execute(telemetry_in, sensor_fusion_in);

    cout << "Mode selector: end" << endl;
}


