#pragma once

typedef enum StatusCode {
	STATUS_CODE_OK = 0,
	STATUS_CODE_UNKNOWN,
	STATUS_CODE_INVALID_ARGS,
	STATUS_CODE_RESOURCE_EXHAUSTED,
	STATUS_CODE_UNREACHABLE,
	STATUS_CODE_TIMEOUT,
	STATUS_CODE_EMPTY,
	STATUS_CODE_OUT_OF_RANGE,
	STATUS_CODE_UNIMPLEMENTED,
	STATUS_CODE_UNINITIALIZED,
	STATUS_CODE_INTERNAL_ERROR,
	NUM_STATUS_CODES,
} StatusCode;

typedef enum ResetState {
	RESET_STATE_WWDG = 0,
	RESET_STATE_IWDG,
	RESET_STATE_PWR,
	RESET_STATE_SOFTWARE,
	RESET_STATE_LOW_PWR,
	RESET_STATE_PIN,
	RESET_STATE_OPTION_BYTE_LOADER
} ResetState;

/**
 * Gets the cause of the last reset from the MCU, and clears the CSR register
 */
ResetState get_reset_cause();

/**
 * Prints the reset state onto a buffer using sprintf
 * @param buffer
 * @param state
 */
void print_reset_state(char* buffer, ResetState state);