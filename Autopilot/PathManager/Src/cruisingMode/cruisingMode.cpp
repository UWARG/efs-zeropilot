#include "cruisingMode.hpp"
#include "cruisingModeStages.hpp"

#include <iostream>
using namespace std;

CruisingMode::CruisingMode() {
    current_stage = &CruisingFlight::getInstance();
}

PathMode& CruisingMode::getInstance() {
    static CruisingMode singleton;
    return singleton;
}

void CruisingMode::execute() {
    mode_selector = &PathModeSelector::getInstance();

    current_stage->execute(this);
}