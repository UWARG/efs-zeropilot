#include "takeoffMode.hpp"
#include "takeoffModeStages.hpp"

PathModeSelector* TakeoffMode::_mode_selector = PathModeSelector::getInstance();

TakeoffMode::TakeoffMode() : PathMode {}, takeoff_stage {TAKEOFF_ROLL}, made_takeoff_points {false} {
    current_stage = &TakeoffRollStage::getInstance();
}

PathMode& TakeoffMode::getInstance() {
    static TakeoffMode singleton;
    return singleton;
}

void TakeoffMode::execute(Telemetry_PIGO_t telemetry_in, SFOutput_t sensor_fusion_in, IMU_Data_t imu_data_in) {
    telemetry_data = telemetry_in;
    sf_data = sensor_fusion_in;
    imu_data = imu_data_in;

    _mode_selector->setCurrentModeEnum(MODE_TAKEOFF);

    current_stage->execute(this);
}

PathModeSelector* TakeoffMode::getModeSelector() {
    return _mode_selector;
}

