#include "cruisingModeStages.hpp"

#include <iostream>

using namespace std;

void CruisingFlight::execute(CruisingMode* cruiseMode) {

    Telemetry_PIGO_t telem = cruiseMode->getTelemetryData();

    cout << "Here | " << telem.numWaypoints << endl;

    AltitudeAirspeedInput_t a;
    a.currentAltitude = 10;
    a.desiredAirspeed = 11;
    a.currentAirspeed = 12;
    a.desiredAltitude = 14;

    cruiseMode->getModeSelector()->setAltitdeAirspeedInput(a);
}

CruisingModeStageManager& CruisingFlight::getInstance() {
    static CruisingFlight singleton;
    return singleton;
}

void PathModeSelector::setAltitdeAirspeedInput(AltitudeAirspeedInput_t alt_airspeed_input) { 
    cout << alt_airspeed_input.currentAltitude << endl;
    
    altitude_airspeed_input.currentAltitude = alt_airspeed_input.currentAltitude; 
    altitude_airspeed_input.desiredAltitude = alt_airspeed_input.desiredAltitude; 
    altitude_airspeed_input.currentAirspeed = alt_airspeed_input.currentAirspeed; 
    altitude_airspeed_input.desiredAirspeed = alt_airspeed_input.desiredAirspeed; 
}