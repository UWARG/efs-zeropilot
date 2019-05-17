/**
 * Abstracts watchdog timer implementations. There are 2 timers that exist. The independent watchdog timer
 * runs off the LSI clock. It can be reset at any time before the timeout (or the mcu will reset). The window
 * watchdog timer runs off the system clock, and must be reset at a particular window (cant be too early or too late)
 * or the processor will reset. Use both, as the independent watchdog timer can help in case the external pll clock has
 * failed to startup. These should usually be reset upon the completion of the main loop
 *
 * Upon triggering the timers will attempt to write to the debug port, before resetting the MCU
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#include "Status.hpp"
#include <stdint.h>

#pragma once

class WindowedWatchdog {
 public:
	/**
	 * Initializes the watchdog timer and its interrupts
	 * You cannot disable it dynamically after calling this method
	 * Set the time window after a reset that the timer must be reset again to not cause a reset
	 * @param min_ms
	 * @param max_ms
	 * @return
	 */
	StatusCode setup(uint32_t min_ms, uint32_t max_ms);

	/**
	 * Call this to reset the timeout of the timer. Should be called at the end of the main loop
	 * @return
	 */
	StatusCode reset_timer();

 private:
	bool is_setup = false;
};

class IndependentWatchdog {
 public:
	/**
	 * Set timeout after a reset that timer must be triggered to not cause a reset
	 * @param max_ms
	 * @return
	 */
	StatusCode setup(uint32_t max_ms);

	/**
	 * Call this to reset the timeout of the timer. Should be called at the end of the main loop
	 * @return
	 */
	StatusCode reset_timer();

 private:
	bool is_setup = false;
};