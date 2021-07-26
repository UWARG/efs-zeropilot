#include "cruisingMode.hpp"
#include "cruisingModeStages.hpp"

#include <iostream>
using namespace std;

CruisingMode::CruisingMode() : PathMode {} {
    current_stage = &CruisingFlight::getInstance();
}

PathMode& CruisingMode::getInstance() {
    static CruisingMode singleton;
    return singleton;
}

void CruisingMode::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in) {
    mode_selector = &PathModeSelector::getInstance();

    telemetry_data = telemetry_in;
    sensor_fusion_output = sensor_fusion_in;

    current_stage->execute(this);
}