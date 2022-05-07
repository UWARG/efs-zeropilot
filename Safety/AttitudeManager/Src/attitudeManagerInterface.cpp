
#include "attitudeManagerInterface.h"
#include "attitudeManager.hpp"
#include "PPM.hpp"
#include "PWM.hpp"
#include "main.h"

attitudeManager *attMng;
PPMChannel *ppm;
PWMChannel *pwm;

constexpr static uint8_t HORIZONTAL_GIMBAL_OUTPUT = 4;
constexpr static uint8_t VERTICAL_GIMBAL_OUTPUT = 5;
// constexpr static uint8_t GRABBER_CRANE_OUTPUT = 7;
// constexpr static uint8_t GRABBER_CLAW_OUTPUT = 8;

constexpr static uint8_t HORIZONTAL_GIMBAL_INPUT = 1;
constexpr static uint8_t VERTICAL_GIMBAL_INPUT = 4;
constexpr static uint8_t GRABBER_CRANE_INPUT = 2;
constexpr static uint8_t GRABBER_CLAW_INPUT = 3
;


void AttitudeManagerInterfaceInit(void) {
    ppm = new PPMChannel(MAX_PPM_CHANNELS);
    pwm = new PWMChannel();
    // attMng = new attitudeManager(ppm, pwm);
}

void AttitudeManagerInterfaceExecute(void)
{

    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

    pwm->set(HORIZONTAL_GIMBAL_OUTPUT, ppm->get(HORIZONTAL_GIMBAL_INPUT));
    pwm->set(VERTICAL_GIMBAL_OUTPUT, ppm->get(VERTICAL_GIMBAL_INPUT));
    
    int craneState = 0;
    int grabberState = 0;
    if (ppm->get(GRABBER_CRANE_INPUT) < 33) {
        craneState = -1; // crane lowering
    }
    else if (ppm->get(GRABBER_CRANE_INPUT) <= 66) {
        craneState = 0; // crane braked
    }

    else if (ppm->get(GRABBER_CRANE_INPUT) > 66) {
        craneState = 1; // crane raising
    }
    
    if (ppm->get(GRABBER_CLAW_INPUT) < 33) {
        grabberState = -1; // grabber closing
    }
    else if (ppm->get(GRABBER_CLAW_INPUT) <= 66) {
        grabberState = 0; // grabber braked
    }

    else if (ppm->get(GRABBER_CLAW_INPUT) > 66) {
        grabberState = 1; // grabber opening
    }

    if (craneState == -1) {
        HAL_GPIO_WritePin(GRABBER_M2A_Port, GRABBER_Pin_9_M2A, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GRABBER_M2B_Port, GRABBER_Pin_5_M2B, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(GRABBER_M1A_Port, GRABBER_Pin_11_M1A, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GRABBER_Pin_12_M1B, GPIO_PIN_RESET);
    }
    
    else if (craneState == 0) {
        HAL_GPIO_WritePin(GRABBER_M2A_Port, GRABBER_Pin_9_M2A, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GRABBER_Pin_5_M2B, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(GRABBER_M1A_Port, GRABBER_Pin_11_M1A, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GRABBER_Pin_12_M1B, GPIO_PIN_RESET);
    }

    else {
        HAL_GPIO_WritePin(GRABBER_M2A_Port, GRABBER_Pin_9_M2A, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GRABBER_M2B_Port, GRABBER_Pin_5_M2B, GPIO_PIN_SET);

        HAL_GPIO_WritePin(GRABBER_M1A_Port, GRABBER_Pin_11_M1A, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GRABBER_Pin_12_M1B, GPIO_PIN_SET);
    }

    if (grabberState == -1) {

        HAL_GPIO_WritePin(CRANE_M1A_Port, CRANE_Pin_7_M1A, GPIO_PIN_SET);
        HAL_GPIO_WritePin(CRANE_M1B_Port, CRANE_Pin_8_M1B, GPIO_PIN_RESET);
    }

    else if (grabberState == 0) {

        HAL_GPIO_WritePin(CRANE_M1A_Port, CRANE_Pin_7_M1A, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(CRANE_M1B_Port, CRANE_Pin_8_M1B, GPIO_PIN_RESET);
    }

    else {
        HAL_GPIO_WritePin(CRANE_M1A_Port, CRANE_Pin_7_M1A, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(CRANE_M1B_Port, CRANE_Pin_8_M1B, GPIO_PIN_SET);
    }

    // attMng->execute();
}
