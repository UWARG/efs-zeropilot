/*
*
* A header for an airspeed sensor mock
*
*/

#ifndef AIRSPEED_MOCK_H
#define AIRSPEED_MOCK_H

#include <gmock/gmock.h>

#include "airspeed.hpp"


class MockAirspeed : public airspeed {
	public:
		MOCK_METHOD(void, Init, ());
		MOCK_METHOD(void, Begin_Measuring, ());
		MOCK_METHOD(void, GetResult, (airspeedData_t &Data));
};

#endif
