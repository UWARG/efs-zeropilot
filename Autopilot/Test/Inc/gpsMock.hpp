#ifndef GPS_MOCK_H
#define GPS_MOCK_H

#include "gps.hpp"

#include <gmock/gmock.h>


class MockGps : public Gps {
	public:

		MOCK_METHOD(void, Init, ());
		MOCK_METHOD(void, BeginMeasuring, ());
		MOCK_METHOD(void, GetResult, (GpsData_t *Data));
};

#endif
