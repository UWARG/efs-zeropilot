#include "PPM.hpp"
#include "tim.h"
#include <stdint.h>

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

const float SEC_TO_MICROSEC = 1000000.0f;

const float BASE_FREQUENCY = 48000000.0f;

const float PULSE_WIDTH = 310.0f; // in us
const float MIN_WIDTH_OF_RESET_PULSE = 5000.0f; // not really a pulse, this is slightly smaller than the difference in time between sequential PPM packets
const float MIN_PULSE_WIDTH = 700.0f;
const float MAX_PULSE_WIDTH = 1670.0f;


/***********************************************************************************************************************
 * Variables
 **********************************************************************************************************************/

static volatile float ppm_values[MAX_PPM_CHANNELS]; // cannot be private because it's used by the ISR

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static float counter_to_time(uint32_t count, uint32_t psc);
static uint8_t time_to_percentage(uint32_t max, uint32_t min, float time);


/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

PPMChannel::PPMChannel(uint8_t channels, uint32_t timeout) {
	if (channels > MAX_PPM_CHANNELS || channels <= 0) {
		num_channels = 8;
	}

	this->disconnect_timeout = timeout;

	for (int i = 0; i < MAX_PPM_CHANNELS; i++) {
		ppm_values[i] = 0;
		min_values[i] = MIN_PULSE_WIDTH;
		max_values[i] = MAX_PULSE_WIDTH;
	}

	HAL_TIM_IC_Start_IT(&htim15, TIM_CHANNEL_1);
}

StatusCode PPMChannel::setLimits(uint8_t channel, uint32_t min, uint32_t max, uint32_t deadzone) {
	if (channel <= 0 || channel > num_channels || min >= max) {
		return STATUS_CODE_INVALID_ARGS;
	}

	this->min_values[channel - 1] = min;
	this->max_values[channel - 1] = max;

	return STATUS_CODE_OK;
}

StatusCode PPMChannel::setNumChannels(uint8_t num) {
	if (num <= 0 || num > MAX_PPM_CHANNELS) {
		return STATUS_CODE_INVALID_ARGS;
	}
	num_channels = num;
	return STATUS_CODE_OK;
}

uint32_t PPMChannel::get_us(PWMChannelNum num)
{
	if(num >= num_channels || num >= MAX_PPM_CHANNELS) {
		return 0;
	}
	return static_cast<uint32_t>(ppm_values[num]);
}



uint8_t PPMChannel::get(PWMChannelNum num)
{
	if(num >= num_channels || num >= MAX_PPM_CHANNELS) {
		return 0;
	}

	uint8_t percentage = time_to_percentage(max_values[num], min_values[num], ppm_values[num]);
	return percentage;
}

static float counter_to_time(uint32_t count, uint32_t psc)
{
	float ticksPerSecond = (BASE_FREQUENCY / (psc + 1.0f));
	float ticksPerMicroSecond = ticksPerSecond / SEC_TO_MICROSEC;

	float lengthOfPulse = count / ticksPerMicroSecond;

	return lengthOfPulse;
}


static uint8_t time_to_percentage(uint32_t max, uint32_t min, float time) {

	float percentage = ((time - min) / (max - min)) * 100.0f;

	if (percentage > 100.0f)
	{
		percentage = 100.0f;
	}
	else if (percentage < 0.0f)
	{
		percentage = 0.0f;
	}

	return static_cast<uint8_t>(percentage);
}

//our interrupt callback for when we get a pulse capture
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == TIM15)
	{

		static volatile uint8_t index = 0;

		volatile uint32_t time_diff = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COUNTER(htim, 0);

		float pulseLength = counter_to_time(time_diff, htim->Init.Prescaler) - PULSE_WIDTH;

		if (pulseLength > MIN_WIDTH_OF_RESET_PULSE)
		{
			index = 0;
		}
		else
		{
			ppm_values[index] = pulseLength;
			index++;
		}
	}
}
