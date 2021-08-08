//
// Created by anthony Luo on 6/19/2021.
//

#include "Interchip_A.h"
#include "gimbal.hpp"
#include "MathConstants.hpp"

static constexpr float gimbalMaxPitch = ZP_PI;
static constexpr float gimbalMaxYaw = ZP_PI/2;

void setPitchPercent(float pitchTarget) {
    // returns target pitch as a percentage of the gimbal
    float gimbalPercentPitch = 100 * pitchTarget / gimbalMaxPitch;
    if(gimbalPercentPitch > 100) {
        gimbalPercentPitch = 100;
    }
    Interchip_SetPWM(3, gimbalPercentPitch);
}

void setYawPercent(float yawTarget){
    // returns target yaw as a percentage of the gimbal
    float gimbalPercentYaw = 50 + 50 * yawTarget / gimbalMaxYaw;
    if(gimbalPercentYaw < 0){
        gimbalPercentYaw = 0;
    }else if(gimbalPercentYaw > 100){
        gimbalPercentYaw = 100;
    }
    Interchip_SetPWM(7, gimbalPercentYaw);
}