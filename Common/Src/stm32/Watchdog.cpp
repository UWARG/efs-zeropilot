#include "Watchdog.hpp"
#include "Debug.hpp"
#include "Clock.hpp"
#include <math.h>

#if STM32F030xC
#include "stm32f0xx_hal.h"
#elif STM32F7xx
#include "stm32f7xx_hal.h"
#endif

WWDG_HandleTypeDef hwwdg;
IWDG_HandleTypeDef hiwdg;

extern uint8_t MAX_WWDG_VAL;
extern uint8_t MIN_WWDG_VAL;
extern uint16_t MAX_IWDG_VAL;

extern StatusCode get_status_code(HAL_StatusTypeDef status);

StatusCode IndependentWatchdog::setup(uint32_t max_ms) {
	if (is_setup) return STATUS_CODE_INVALID_ARGS;

	hiwdg.Instance = IWDG;
	hiwdg.Init.Window = IWDG_WINDOW_DISABLE;

	uint32_t prescaler = 4;

	auto ticks = (uint32_t) round(max_ms * (get_lsi_clock() / (1000.0 * prescaler)) - 1);

	while (ticks > MAX_IWDG_VAL && prescaler <= 0xFF) {
		prescaler = prescaler << 1;
		ticks = (uint32_t) round(max_ms * (get_lsi_clock() / (1000.0 * prescaler)) - 1);
	}

	//we couldn't find a prescaler, constraints too hard
	if (ticks > MAX_IWDG_VAL) {
		__HAL_RCC_WWDG_CLK_DISABLE();
		return STATUS_CODE_OUT_OF_RANGE;
	}

	hiwdg.Init.Reload = ticks;

	switch (prescaler) {
		case 4: hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
			break;
		case 8: hiwdg.Init.Prescaler = IWDG_PRESCALER_8;
			break;
		case 16: hiwdg.Init.Prescaler = IWDG_PRESCALER_16;
			break;
		case 32: hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
			break;
		case 64: hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
			break;
		case 128: hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
			break;
		case 256: hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
			break;
		default: break;
	}
#if STM32F030xC
	__HAL_RCC_DBGMCU_CLK_ENABLE(); //enable debug clock
#endif
	__HAL_DBGMCU_FREEZE_IWDG(); //freeze watchdog timer during debugging sessions

	StatusCode status = get_status_code(HAL_IWDG_Init(&hiwdg));

	if (status == STATUS_CODE_OK) is_setup = true;

	return status;
}

StatusCode IndependentWatchdog::reset_timer() {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	StatusCode status = get_status_code(HAL_IWDG_Refresh(&hiwdg));
	return status;
}

StatusCode WindowedWatchdog::setup(uint32_t min_ms, uint32_t max_ms) {
	if (is_setup || max_ms <= min_ms)return STATUS_CODE_INVALID_ARGS;

	__HAL_RCC_WWDG_CLK_ENABLE();

	uint8_t max_diff = MAX_WWDG_VAL - MIN_WWDG_VAL;

	uint8_t prescaler = 1;

	auto ticks = (uint32_t) round(max_ms * (get_system_clock() / (1000.0 * 4096 * prescaler)) - 1);

	while (ticks > max_diff && prescaler <= 8) {
		prescaler = prescaler << 1;
		ticks = (uint32_t) round(max_ms * (get_system_clock() / (1000.0 * 4096 * prescaler)) - 1);
	}

	//we couldn't find a prescaler, constraints too hard
	if (ticks > max_diff) {
		__HAL_RCC_WWDG_CLK_DISABLE();
		return STATUS_CODE_OUT_OF_RANGE;
	}

	auto min_ticks = (uint32_t) round(min_ms * (get_system_clock() / (1000.0 * 4096 * prescaler)) - 1);
	min_ticks = ticks - min_ticks;

	hwwdg.Instance = WWDG;
	hwwdg.Init.Window = MIN_WWDG_VAL | min_ticks;
	hwwdg.Init.Counter = MIN_WWDG_VAL | ticks;
	hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;

	switch (prescaler) {
		case 1: hwwdg.Init.Prescaler = WWDG_PRESCALER_1;
			break;
		case 2: hwwdg.Init.Prescaler = WWDG_PRESCALER_2;
			break;
		case 4: hwwdg.Init.Prescaler = WWDG_PRESCALER_4;
			break;
		case 8: hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
			break;
		default: break;
	}

#if STM32F030xC
	__HAL_RCC_DBGMCU_CLK_ENABLE(); //enable debug clock
#endif
	__HAL_DBGMCU_FREEZE_WWDG(); //stop from being triggered in debugging session

	StatusCode status = get_status_code(HAL_WWDG_Init(&hwwdg));

	if (status == STATUS_CODE_OK) {
		is_setup = true;
	}

	return status;
}

StatusCode WindowedWatchdog::reset_timer() {
	if (!is_setup) return STATUS_CODE_INVALID_ARGS;

	StatusCode status = get_status_code(HAL_WWDG_Refresh(&hwwdg));
	return status;
}
