#include "landingMode.hpp"
#include "landingModeStages.hpp"

PathModeSelector* LandingMode::_mode_selector = PathModeSelector::getInstance();

LandingMode::LandingMode() : PathMode {}, landing_stage {LANDING_TRANSITION}, made_landing_points {false} {
    current_stage = &LandingTransitionStage::getInstance();
}

PathMode& LandingMode::getInstance() {
    static LandingMode singleton;
    return singleton;
}

void LandingMode::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in) {
    telemetry_data = telemetry_in;
    sf_data = sensor_fusion_in;
    imu_data = imu_data_in;

    _mode_selector->setCurrentModeEnum(MODE_LANDING);

    current_stage->execute(this);
}

PathModeSelector* LandingMode::getModeSelector() {
    return _mode_selector;
}

