#include "PPM.hpp"
#include "GPIO.hpp"
#include "Clock.hpp"
#include "tim.h"
#include <stdint.h>
#include "stm32f0xx_hal.h"

//number of milliseconds expected between subsequent PPM packets
uint32_t PPM_PACKET_TIMEOUT = 3;

extern StatusCode get_status_code(HAL_StatusTypeDef status);

//if modifying the below pin numbers, also modify the ISR routine down below to match
static const GPIOPinNum PPM_PIN_NUM = 14;
static const GPIOPort PPM_PORT = GPIO_PORT_B;

static const uint16_t TIMER_PRESCALER = 2; // since we're capturing 2000us max signal
static const uint16_t TIMER_PERIOD = 0xFFFF;

static uint8_t num_channels = 0;
static volatile uint16_t capture_value[MAX_PPM_CHANNELS] = {0};
static volatile uint8_t ppm_index = 0;

//these variables are used in the systick interrupt routine
volatile uint32_t ppm_last_received_time = 0;
volatile uint8_t ppm_packet_timeout_reached = 1;

// 1 tick is prescaler / 48000000Hz (internal clock)
//therefore capture in us = capture * prescaler * 1E6 / 8E6
inline static uint32_t convert_ticks_to_us(int32_t ticks) {
	return (ticks * (TIMER_PRESCALER + 1)) / (get_system_clock() / (1000000UL));
}

PPMChannel::PPMChannel(uint8_t channels, uint32_t timeout) {
	if (channels > MAX_PPM_CHANNELS || channels <= 0) {
		num_channels = 8;
	}

	ppm_index = 0;
	this->disconnect_timeout = timeout;

	for (int i = 0; i < MAX_PPM_CHANNELS; i++) {
		capture_value[i] = 0;
		min_values[i] = 1000;
		max_values[i] = 2000;
		deadzones[i] = 0;
	}

	ppm_pin =
		GPIOPin(PPM_PORT, PPM_PIN_NUM, GPIO_ALT_PP, GPIO_STATE_LOW, GPIO_RES_NONE, GPIO_FREQ_LOW, GPIO_AF0_TIM15);
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

StatusCode PPMChannel::setTimeout(uint32_t timeout) {
	if (timeout > 0) {
		this->disconnect_timeout = timeout;
		return STATUS_CODE_OK;
	}
	return STATUS_CODE_INVALID_ARGS;
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
	if (min_diff < 0 || min_diff < deadzones[num - 1]) {
		return 0;
	}

	uint32_t
		percent = (num_us - this->min_values[num - 1]) * 100 / (this->max_values[num - 1] - this->min_values[num - 1]);

	if (percent > 100) {
		return 100;
	}
	return (uint8_t) percent;
}

uint32_t PPMChannel::get_us(PWMChannelNum num) {
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
	__HAL_RCC_TIM15_CLK_ENABLE();

  	MX_TIM15_Init();


	return STATUS_CODE_OK;
}

StatusCode PPMChannel::reset() {
	if (!is_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}

	__HAL_RCC_TIM15_CLK_DISABLE();
	StatusCode status = ppm_pin.reset();
	HAL_NVIC_DisableIRQ(TIM15_IRQn);

	is_setup = false;

	return status;
}

bool PPMChannel::is_disconnected(uint32_t sys_time) {
	bool disconnected = (sys_time - ppm_last_received_time) >= this->disconnect_timeout;

	if (disconnected) { //reset capture states if we get a disconnect timeout
		for (int i = 0; i < num_channels; i++) {
			capture_value[i] = 0;
		}
		ppm_index = 0;
	}

	return disconnected;
}

//our interrupt callback for when we get a pulse capture
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM15) {
		ppm_last_received_time = get_system_time();
		auto time_diff = (uint16_t) HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

		if (ppm_packet_timeout_reached) {
			ppm_packet_timeout_reached = 0;
			ppm_index = 0;
		}

		capture_value[ppm_index] = time_diff;
		ppm_index = (uint8_t) (ppm_index + 1) % num_channels;
		__HAL_TIM_SET_COUNTER(htim, 0);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM15) { // only timer we care about
		ppm_index = 0;
	}
}
