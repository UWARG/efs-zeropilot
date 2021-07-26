#include "pathModeSelector.hpp"
#include "cruisingMode.hpp"

#include <iostream>
using namespace std;

PathModeSelector::PathModeSelector() : current_mode_enum {MODE_CRUISING},
                                       telemetry_data {},
                                       sensor_fusion_output {} {

    cout << "Mode Selector: constructor start" << endl;
    current_mode = &CruisingMode::getInstance(); // Initialize to takeoff in real. Initialize to cruising for first test
    cout << "Mode Selector: constructor end" << endl;
}

PathModeSelector& PathModeSelector::getInstance() {
    static PathModeSelector singleton;
    return singleton;
} 



void PathModeSelector::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in) {

    cout << "Mode Selector: start" << endl;


    telemetry_data = telemetry_in;
    sensor_fusion_output = sensor_fusion_in;

    current_mode->execute();

    cout << "Mode selector: end" << endl;
}


