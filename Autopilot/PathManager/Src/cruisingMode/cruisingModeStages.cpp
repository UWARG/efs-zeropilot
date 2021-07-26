#include "cruisingModeStages.hpp"
#include <iostream>

using namespace std;

void CruisingFlight::execute(CruisingMode* cruiseMode) {

    Telemetry_PIGO_t telem = cruiseMode->getTelemetryData();

    cout << "Here | " << telem.numWaypoints << endl;
}

CruisingModeStageManager& CruisingFlight::getInstance() {
    static CruisingFlight singleton;
    return singleton;
}