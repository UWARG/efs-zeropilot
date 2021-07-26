#include "cruisingModeStages.hpp"
#include <iostream>

using namespace std;

void CruisingFlight::execute(CruisingMode* cruiseMode) {
    cout << "Here" << endl;
}

CruisingModeStageManager& CruisingFlight::getInstance() {
    static CruisingFlight singleton;
    return singleton;
}