#if 0

/**
 * Utilities that'll let you profile your firmware and drivers
 *
 * You can call the start and stop method on a particular profiler multiple times,
 * and it'll gather statistics about the longest, shortest, and average running times
 * of whatever you put in between the start and stop methods. Everything is reported in us
 * @author Serj Babayan
 * @copyright Waterloo Aerial Robotics Group 2019
 *  https://raw.githubusercontent.com/UWARG/ZeroPilot-SW/devel/LICENSE.md
 */

#pragma once

//use C interface so that we can use these tools anywhere in the code, including ISRs

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct Profiler {
	int started_profile;
	uint64_t prev_time;
	uint64_t max_diff;
	uint64_t min_diff;
	uint64_t average_diff;
	uint64_t total_diff;
	uint64_t latest_diff;
	int num_profiles;
} Profiler;

void init_profiler(Profiler *p);

void start_profile(Profiler *p);

void stop_profile(Profiler *p);

/**
 * Prints profiler stats onto buffer
 * Returns number of characters written into buffer
 * @param buffer
 * @param p
 */
int print_profile_stats(const char *label, char *buffer, Profiler *p);

#ifdef __cplusplus
}
#endif

#endif
