#include "cruisingMode.hpp"
#include "cruisingModeStages.hpp"

PathModeSelector* CruisingMode::_mode_selector = PathModeSelector::getInstance();

CruisingMode::CruisingMode() : PathMode {} {
    current_stage = &CruisingFlight::getInstance();
}

PathMode& CruisingMode::getInstance() {
    static CruisingMode singleton;
    return singleton;
}

void CruisingMode::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in) {
    telemetry_data = telemetry_in;
    sf_data = sensor_fusion_in;
    imu_data = imu_data_in;

    _mode_selector->setCurrentModeEnum(MODE_CRUISING);

    current_stage->execute(this);
}

PathModeSelector* CruisingMode::getModeSelector() { 
    return _mode_selector; 
}


