#ifndef BATTERYSENSOR_MOCK_HPP
#define BATTERYSENSOR_MOCK_HPP

#include <gmock/gmock.h>

#include "batterysensor.hpp"

class MockBatterySensor : public batterySensor
{
    public:
    MOCK_METHOD(void, initBatterySensor, ());
    MOCK_METHOD(float, getBatteryVoltage, (int battery));
}

#endif
