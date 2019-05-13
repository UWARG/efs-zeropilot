#include "PPM.hpp"
#include "GPIO.hpp"
#include "Clock.hpp"
#include <stdint.h>
#include "stm32f0xx_hal_tim.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_gpio_ex.h"
#include "stm32f0xx_hal_rcc.h"
#include "stm32f0xx_hal_cortex.h"

TIM_HandleTypeDef htim14;

extern StatusCode get_status_code(HAL_StatusTypeDef status);

//if modifying the below pin numbers, also modify the ISR routine down below to match
static const GPIOPinNum PPM_PIN_NUM = 1;
static const GPIOPort PPM_PORT = GPIO_PORT_B;

static const uint16_t TIMER_PRESCALER = 2; // since we're capturing 2000us max signal
static const uint16_t TIMER_PERIOD = 0xFFFF;

static uint8_t num_channels = 0;
static volatile uint8_t ppm_index = 0;
static volatile uint16_t capture_value[MAX_PPM_CHANNELS] = {0};

// 1 tick is prescaler / 48000000Hz (internal clock)
//therefore capture in us = capture * prescaler * 1E6 / 8E6
inline static uint32_t convert_ticks_to_us(int32_t ticks){
	return (ticks * (TIMER_PRESCALER + 1)) / (get_system_clock() / (1000000UL));
}

PPMChannel::PPMChannel(uint8_t channels) {
	if (channels > MAX_PPM_CHANNELS || channels <= 0) {
		num_channels = 8;
	}

	ppm_index = 0;

	for (int i = 0; i < MAX_PPM_CHANNELS; i++) {
		capture_value[i] = 0;
		min_values[i] = 1000;
		max_values[i] = 2000;
		deadzones[i] = 0;
	}

	ppm_pin =
		GPIOPin(PPM_PORT, PPM_PIN_NUM, GPIO_ALT_PP, GPIO_STATE_LOW, GPIO_RES_NONE, GPIO_SPEED_LOW, GPIO_AF0_TIM14);
}

StatusCode PPMChannel::setLimits(uint8_t channel, uint32_t min, uint32_t max, uint32_t deadzone) {
	if (channel <= 0 || channel > num_channels || min >= max) {
		return STATUS_CODE_INVALID_ARGS;
	}

	this->min_values[channel - 1] = min;
	this->max_values[channel - 1] = max;
	this->deadzones[channel - 1] = deadzone;

	return STATUS_CODE_OK;
}

StatusCode PPMChannel::setNumChannels(uint8_t num) {
	if (num <= 0 || num > MAX_PPM_CHANNELS) {
		return STATUS_CODE_INVALID_ARGS;
	}
	num_channels = num;
	return STATUS_CODE_OK;
}

uint8_t PPMChannel::get(PWMChannelNum num) {
	if (num <= 0 || num > num_channels) {
		return 0;
	}

	int32_t capture = capture_value[num - 1];

	// 1 tick is prescaler / 48000000Hz (internal clock)
	//therefore capture in us = capture * prescaler * 1E6 / 8E6
	uint32_t num_us = convert_ticks_to_us(capture);

	int32_t min_diff = num_us - this->min_values[num - 1];

	//if the input signal is lower than the minimum signal, or if we're not outside the deadzone, return 0%
	if (min_diff < 0 || min_diff < deadzones[num - 1]){
		return 0;
	}

	uint32_t percent =  (num_us - this->min_values[num - 1])*100/ (this->max_values[num - 1] - this->min_values[num - 1]);

	if (percent > 100){
		return 100;
	}
	return (uint8_t)percent;
}

uint32_t PPMChannel::get_us(PWMChannelNum num){
	if (num <= 0 || num > num_channels) {
		return 0;
	}

	int32_t capture = capture_value[num - 1];
	return convert_ticks_to_us(capture);
}

StatusCode PPMChannel::setup() {
	if (is_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}

	__HAL_RCC_TIM14_CLK_ENABLE();

	StatusCode status = ppm_pin.setup();
	if (status != STATUS_CODE_OK) return status;

	//enable timer14 interrupts
	HAL_NVIC_SetPriority(TIM14_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM14_IRQn);

	TIM_IC_InitTypeDef sConfigIC = {0, 0, 0, 0};

	htim14.Instance = TIM14;
	htim14.Init.Prescaler = TIMER_PRESCALER;
	htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim14.Init.Period = TIMER_PERIOD;
	htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	status = get_status_code(HAL_TIM_Base_Init(&htim14));
	if (status != STATUS_CODE_OK) return status;

	status = get_status_code(HAL_TIM_IC_Init(&htim14));
	if (status != STATUS_CODE_OK) return status;

	sConfigIC.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;

	status = get_status_code(HAL_TIM_IC_ConfigChannel(&htim14, &sConfigIC, TIM_CHANNEL_1));
	if (status != STATUS_CODE_OK) return status;

	status = get_status_code(HAL_TIM_Base_Start_IT(&htim14));
	if (status != STATUS_CODE_OK) return status;

	status = get_status_code(HAL_TIM_IC_Start_IT(&htim14, TIM_CHANNEL_1));
	if (status != STATUS_CODE_OK) return status;

	is_setup = true;

	return STATUS_CODE_OK;
}

StatusCode PPMChannel::reset() {
	if (!is_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}

	__HAL_RCC_TIM14_CLK_DISABLE();
	StatusCode status = ppm_pin.reset();
	HAL_NVIC_DisableIRQ(TIM14_IRQn);

	is_setup = false;

	return status;
}

//our interrupt callback for when we get a pulse capture
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM14) {
		bool high = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1); //do direct HAL read since we're in an ISR

		// by the time we're reading it, the pulse should be at its low
		//the reason for doing this is because the HAL will actually capture both the lengths of the positive and negative pulses!
		if (!high){
			auto time_diff = (uint16_t) HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

			capture_value[ppm_index] = time_diff;
			ppm_index = (uint8_t) (ppm_index + 1) % num_channels;
		}
		__HAL_TIM_SET_COUNTER(htim, 0);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM14) { // only timer we care about
		ppm_index = 0;
	}
}