/*
*
* A header for an airspeed sensor mock
*
*/

#ifndef AIRSPEED_MOCK_H
#define AIRSPEED_MOCK_H

#include "airspeed.hpp"

#include <gmock/gmock.h>


class MockAirspeed : public airspeed {
	public:

		MOCK_METHOD(void, Init, ());
		MOCK_METHOD(void, BeginMeasuring, ());
		MOCK_METHOD(void, GetResult, (airspeedData_t *Data));
};

#endif
