

#if 0

#include "Profiler.h"
#include "Clock.hpp"
#include <stdint.h>
#include <stdio.h>

void init_profiler(Profiler *p) {
	p->started_profile = 0;
	p->prev_time = 0;
	p->max_diff = 0;
	p->latest_diff = 0;
	p->min_diff = INT64_MAX;
	p->average_diff = 0;
	p->total_diff = 0;
	p->num_profiles = 0;
}

void start_profile(Profiler *p) {
	p->started_profile = 1;
	p->prev_time = get_system_time_us();
}

void stop_profile(Profiler *p) {
	if (!p->started_profile) return;
	p->started_profile = 0;
	p->latest_diff = get_system_time_us() - p->prev_time;
	p->num_profiles++;
	p->total_diff += p->latest_diff;
	p->average_diff = p->total_diff / p->num_profiles;
	p->max_diff = (p->latest_diff > p->max_diff) ? p->latest_diff : p->max_diff;
	p->min_diff = (p->latest_diff < p->min_diff) ? p->latest_diff : p->min_diff;
}

int print_profile_stats(const char *label, char *buffer, Profiler *p) {
	if (p->num_profiles == 0) {
		return 0;
	}

	return sprintf(buffer, "Profile for: %s \r\n"
						   "Latest: %lu \r\n"
						   "Average: %lu \r\n"
						   "Max: %lu \r\n"
						   "Min: %lu \r\n"
						   "Num Profiles: %d \r\n",
				   label,
				   (uint32_t) p->latest_diff,
				   (uint32_t) p->average_diff,
				   (uint32_t) p->max_diff,
				   (uint32_t) p->min_diff,
				   p->num_profiles);
}

#endif
