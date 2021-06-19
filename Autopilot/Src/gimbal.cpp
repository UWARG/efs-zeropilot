//
// Created by anthony Luo on 6/19/2021.
//

#include "Interchip_A.h"
#include <gimbal.hpp>

static const float gimbalMaxPitch = 3.14159;
static const float gimbalMaxYaw = 3.14159/2;

float getPitchPercent(float pitchTarget) {
    // returns target pitch as a percentage of the gimbal
    float gimbalPercentPitch = 100 * pitchTarget / gimbalMaxPitch;
    if(gimbalPercentPitch > 100) {
        gimbalPercentPitch = 100;
    }
    return gimbalPercentPitch
}

float getYawPercent(float yawTarget){
    // returns target yaw as a percentage of the gimbal
    float gimbalPercentYaw = 50 + 50 * yawTarget / gimbalMaxYaw;
    if(gimbalPercentYaw < 0){
        gimbalPercentYaw = 0;
    }else if(gimbalPercentYaw > 100){
        gimbalPercentYaw = 100;
    }
    return gimbalPercentYaw
}