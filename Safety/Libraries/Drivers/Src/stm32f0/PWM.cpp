#include "PWM.hpp"
#include "GPIO.hpp"
#include "Clock.hpp"
#include "stm32f0xx_hal.h"
#include <math.h>

#if 0
typedef struct PWMPinConfig {
	GPIOPinNum num;
	GPIOPort port;
	uint8_t pin_function;
	TIM_HandleTypeDef *timer;
	uint16_t timer_channel;
} PWMPinConfig;

struct PWMCounterSettings {
	uint16_t prescaler;
	uint16_t period;
};

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
// TIM_HandleTypeDef htim15; No !
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

static const PWMPinConfig PWM_CONFIG[12] = {
	{6, GPIO_PORT_A, GPIO_AF5_TIM16, &htim16, TIM_CHANNEL_1}, //pwm1
	{7, GPIO_PORT_A, GPIO_AF5_TIM17, &htim17, TIM_CHANNEL_1}, //pwm2
	{14, GPIO_PORT_B, GPIO_AF1_TIM15, &htim15, TIM_CHANNEL_1}, //pwm3
	{15, GPIO_PORT_B, GPIO_AF1_TIM15, &htim15, TIM_CHANNEL_2}, //pwm4
	{6, GPIO_PORT_C, GPIO_AF0_TIM3, &htim3, TIM_CHANNEL_1}, //pwm5
	{7, GPIO_PORT_C, GPIO_AF0_TIM3, &htim3, TIM_CHANNEL_2}, //pwm6
	{8, GPIO_PORT_C, GPIO_AF0_TIM3, &htim3, TIM_CHANNEL_3}, //pwm7
	{9, GPIO_PORT_C, GPIO_AF0_TIM3, &htim3, TIM_CHANNEL_4}, //pwm8
	{8, GPIO_PORT_A, GPIO_AF2_TIM1, &htim1, TIM_CHANNEL_1}, //pwm9
	{9, GPIO_PORT_A, GPIO_AF2_TIM1, &htim1, TIM_CHANNEL_2}, //pwm10
	{10, GPIO_PORT_A, GPIO_AF2_TIM1, &htim1, TIM_CHANNEL_3}, //pwm11
	{11, GPIO_PORT_A, GPIO_AF2_TIM1, &htim1, TIM_CHANNEL_4}, //pwm12
};

extern StatusCode get_status_code(HAL_StatusTypeDef status);

/**
 * Tries to retrieve the optimal prescaler and period values in order to meet the desired
 * period demand. Tries to achieve the lowest prescaler value so that we get the highest
 * precision in our duty cycles
 * @param period Desired period in us
 * @return {0,0} if a prescaler couldn't be found to meet the required period value (usually if period is way too long)
 */
static struct PWMCounterSettings getCounterSettings(uint32_t period);

static TIM_OC_InitTypeDef getChannelConfig(bool inverted = false);
static TIM_BreakDeadTimeConfigTypeDef getBreaktimeConfig();
static TIM_MasterConfigTypeDef getMasterConfig();
static StatusCode init_timer(TIM_HandleTypeDef *timer,
							 TIM_TypeDef *instance,
							 uint32_t period,
							 int32_t num_channels,
							 bool config_breaktime,
							 bool config_master,
							 bool inverted);

PWMChannel::PWMChannel(GPIOPort port, GPIOPinNum pin_num, uint8_t alternate_function, void *timer, uint16_t channel) {
	pin = GPIOPin(port, pin_num, GPIO_ALT_PP, GPIO_STATE_LOW, GPIO_RES_NONE, GPIO_FREQ_LOW, alternate_function);
	this->timer = timer;
	this->timer_channel = channel;
}

void PWMChannel::setLimits(uint32_t min, uint32_t max) {
	this->min_signal = min;
	this->max_signal = max;
}

void PWMChannel::set(uint8_t percent) {
	if (percent > 100) {
		percent = 100;
	}

	//in us
	uint32_t prescaler = (static_cast<TIM_HandleTypeDef *>(this->timer))->Init.Prescaler;
	uint32_t us = ((percent * (max_signal - min_signal)) / 100 + min_signal);
	uint32_t ticks = (us * (get_system_clock() / 1000000UL)) / (prescaler + 1);

	__HAL_TIM_SET_COMPARE((TIM_HandleTypeDef *) this->timer, this->timer_channel, (uint32_t) ticks);
}

StatusCode PWMChannel::setup() {
	return pin.setup();
}

StatusCode PWMChannel::reset() {
	return pin.reset();
}

PWMManager &PWMManager::getInstance() {
	static PWMManager instance;

	return instance;
}

StatusCode PWMManager::setup() {
	if (is_setup) {
		return STATUS_CODE_INVALID_ARGS;
	}

	//default 50hz PWM pulse
	PWMGroupSetting default_settings = {
		20000, //20ms period
		1000, //1ms min pulse length
		1500, //2ms max pulse length
		false //not inverted
	};

	StatusCode status;

	//configure the timers. Abort if something goes wrong
	status = configure(PWM_GROUP_1, default_settings);
	if (status != STATUS_CODE_OK) return status;
	status = configure(PWM_GROUP_2, default_settings);
	if (status != STATUS_CODE_OK) return status;
	status = configure(PWM_GROUP_3_4, default_settings);
	if (status != STATUS_CODE_OK) return status;
	status = configure(PWM_GROUP_5_8, default_settings);
	if (status != STATUS_CODE_OK) return status;
	status = configure(PWM_GROUP_9_12, default_settings);
	if (status != STATUS_CODE_OK) return status;

	//init the GPIO for all the channels
	for (int i = 0; i < 12; i++) {
		channels[i] = PWMChannel(PWM_CONFIG[i].port,
								 PWM_CONFIG[i].num,
								 PWM_CONFIG[i].pin_function,
								 (void *) PWM_CONFIG[i].timer,
								 PWM_CONFIG[i].timer_channel);
		status = channels[i].setup();

		if (status != STATUS_CODE_OK) {
			return status;
		}

		status = get_status_code(HAL_TIM_PWM_Start(PWM_CONFIG[i].timer, PWM_CONFIG[i].timer_channel));
		if (status != STATUS_CODE_OK) return status;
	}

	is_setup = true;
	return STATUS_CODE_OK;
}

StatusCode PWMManager::configure(PWMGroup group, PWMGroupSetting setting) {
	switch (group) {
		case PWM_GROUP_1: __HAL_RCC_TIM16_CLK_ENABLE();
			init_timer(&htim16, TIM16, setting.period, 1, true, false, setting.inverted);
			channels[0].setLimits(setting.min_length, setting.max_length);

			if (is_setup) { //if we're reconfiguring the channel, rather than completly setting it up again
				return get_status_code(HAL_TIM_PWM_Start(PWM_CONFIG[0].timer, PWM_CONFIG[0].timer_channel));
			}
			break;
		case PWM_GROUP_2: __HAL_RCC_TIM17_CLK_ENABLE();
			init_timer(&htim17, TIM17, setting.period, 1, true, false, setting.inverted);
			channels[1].setLimits(setting.min_length, setting.max_length);

			if (is_setup) {
				return get_status_code(HAL_TIM_PWM_Start(PWM_CONFIG[1].timer, PWM_CONFIG[1].timer_channel));
			}
			break;
		case PWM_GROUP_3_4: __HAL_RCC_TIM15_CLK_ENABLE();
			init_timer(&htim15, TIM15, setting.period, 2, true, true, setting.inverted);
			channels[2].setLimits(setting.min_length, setting.max_length);
			channels[3].setLimits(setting.min_length, setting.max_length);

			if (is_setup) {
				HAL_TIM_PWM_Start(PWM_CONFIG[2].timer, PWM_CONFIG[2].timer_channel);
				return get_status_code(HAL_TIM_PWM_Start(PWM_CONFIG[3].timer, PWM_CONFIG[3].timer_channel));
			}
			break;
		case PWM_GROUP_5_8: __HAL_RCC_TIM3_CLK_ENABLE();
			init_timer(&htim3, TIM3, setting.period, 4, false, true, setting.inverted);
			channels[4].setLimits(setting.min_length, setting.max_length);
			channels[5].setLimits(setting.min_length, setting.max_length);
			channels[6].setLimits(setting.min_length, setting.max_length);
			channels[7].setLimits(setting.min_length, setting.max_length);

			if (is_setup) {
				HAL_TIM_PWM_Start(PWM_CONFIG[4].timer, PWM_CONFIG[4].timer_channel);
				HAL_TIM_PWM_Start(PWM_CONFIG[5].timer, PWM_CONFIG[5].timer_channel);
				HAL_TIM_PWM_Start(PWM_CONFIG[6].timer, PWM_CONFIG[6].timer_channel);
				return get_status_code(HAL_TIM_PWM_Start(PWM_CONFIG[7].timer, PWM_CONFIG[7].timer_channel));
			}
			break;
		case PWM_GROUP_9_12: __HAL_RCC_TIM1_CLK_ENABLE();
			init_timer(&htim1, TIM1, setting.period, 4, true, true, setting.inverted);
			channels[8].setLimits(setting.min_length, setting.max_length);
			channels[9].setLimits(setting.min_length, setting.max_length);
			channels[10].setLimits(setting.min_length, setting.max_length);
			channels[11].setLimits(setting.min_length, setting.max_length);

			if (is_setup) {
				HAL_TIM_PWM_Start(PWM_CONFIG[8].timer, PWM_CONFIG[8].timer_channel);
				HAL_TIM_PWM_Start(PWM_CONFIG[9].timer, PWM_CONFIG[9].timer_channel);
				HAL_TIM_PWM_Start(PWM_CONFIG[10].timer, PWM_CONFIG[10].timer_channel);
				return get_status_code(HAL_TIM_PWM_Start(PWM_CONFIG[11].timer, PWM_CONFIG[11].timer_channel));
			}
			break;
		default: return STATUS_CODE_INVALID_ARGS;
	}
	return STATUS_CODE_OK;
}

StatusCode PWMManager::reset() {
	if (!is_setup) { // if already reset
		return STATUS_CODE_INVALID_ARGS;
	}

	for (int i = 0; i < 12; i++) {
		channels[i].reset();
	}

	HAL_TIM_Base_DeInit(&htim16);
	HAL_TIM_Base_DeInit(&htim17);
	HAL_TIM_Base_DeInit(&htim15);
	HAL_TIM_Base_DeInit(&htim3);
	HAL_TIM_Base_DeInit(&htim1);

	__HAL_RCC_TIM16_CLK_DISABLE();
	__HAL_RCC_TIM17_CLK_DISABLE();
	__HAL_RCC_TIM15_CLK_DISABLE();
	__HAL_RCC_TIM3_CLK_DISABLE();
	__HAL_RCC_TIM1_CLK_DISABLE();

	is_setup = false;
	return STATUS_CODE_OK;
}

StatusCode PWMManager::set_all(uint8_t percent) {
	for (int i = 0; i < 12; i++) {
		channels[i].set(percent);
	}
	return STATUS_CODE_OK;
}

PWMChannel &PWMManager::channel(PWMChannelNum num) {
	if (num <= 12 && num > 0) {
		return channels[num - 1];
	}
	return channels[0];
}

static struct PWMCounterSettings getCounterSettings(uint32_t period) {
	struct PWMCounterSettings settings = {0, 0};
	settings.prescaler = 0;
	uint32_t ticks;

	do {
		settings.prescaler++;

		if (settings.prescaler == 0xFFFF) { // if we couldn't find a prescaler value
			return {0, 0};
		}
		double tmp = (get_system_clock() / 1000000.0) * period / (settings.prescaler + 1);
		ticks = (uint32_t) round(tmp);
	} while (ticks > 0xFFFF);
	settings.period = (uint16_t) ticks;

	return settings;
}

static TIM_OC_InitTypeDef getChannelConfig(bool inverted) {
	TIM_OC_InitTypeDef sConfigOC = {0, 0, 0, 0, 0, 0, 0};

	if (inverted) {
		sConfigOC.OCMode = TIM_OCMODE_PWM2;
	} else {
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
	}
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	return sConfigOC;
}

static TIM_BreakDeadTimeConfigTypeDef getBreaktimeConfig() {
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0, 0, 0, 0, 0, 0, 0};
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;

	return sBreakDeadTimeConfig;
}

static TIM_MasterConfigTypeDef getMasterConfig() {
	TIM_MasterConfigTypeDef sMasterConfig = {0, 0};

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	return sMasterConfig;
}

static StatusCode init_timer(TIM_HandleTypeDef *timer,
							 TIM_TypeDef *instance,
							 uint32_t period,
							 int32_t num_channels,
							 bool config_breaktime,
							 bool config_master,
							 bool inverted) {
	TIM_ClockConfigTypeDef sClockSourceConfig = {0, 0, 0, 0};
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

	TIM_MasterConfigTypeDef sMasterConfig = getMasterConfig();
	TIM_OC_InitTypeDef sConfigOC = getChannelConfig(inverted);
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = getBreaktimeConfig();

	struct PWMCounterSettings counter = getCounterSettings(period);

	timer->Instance = instance;
	timer->Init.Prescaler = counter.prescaler;
	timer->Init.Period = counter.period;
	timer->Init.CounterMode = TIM_COUNTERMODE_UP;
	timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timer->Init.RepetitionCounter = 0;
	timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	HAL_StatusTypeDef status;

	status = HAL_TIM_Base_Init(timer);

	if (status != HAL_OK) {
		return get_status_code(status);
	}

	status = HAL_TIM_ConfigClockSource(timer, &sClockSourceConfig);

	if (status != HAL_OK) {
		return get_status_code(status);
	}

	status = HAL_TIM_PWM_Init(timer);

	if (status != HAL_OK) {
		return get_status_code(status);
	}

	if (config_master) {
		status = HAL_TIMEx_MasterConfigSynchronization(timer, &sMasterConfig);

		if (status != HAL_OK) {
			return get_status_code(status);
		}
	}

	status = HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, TIM_CHANNEL_1);

	if (status != HAL_OK) {
		return get_status_code(status);
	}

	switch (num_channels) {
		case 4: status = HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, TIM_CHANNEL_4);
			if (status != HAL_OK) return get_status_code(status);
		case 3: status = HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, TIM_CHANNEL_3);
			if (status != HAL_OK) return get_status_code(status);
		case 2: status = HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, TIM_CHANNEL_2);
			if (status != HAL_OK) return get_status_code(status);
		default: break;
	}

	if (config_breaktime) {
		status = HAL_TIMEx_ConfigBreakDeadTime(timer, &sBreakDeadTimeConfig);
		if (status != HAL_OK) {
			return get_status_code(status);
		}
	}
	return STATUS_CODE_OK;
}

#endif
