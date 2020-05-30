#ifndef IMU_MOCK_HPP
#define IMU_MOCK_HPP

#include <gmock/gmock.h> // Brings in gMock

#include "IMU.hpp"


class MockIMU : public IMU {
    public: 
        MOCK_METHOD(void, Init, ());
        MOCK_METHOD(void, Begin_Measuring, ());
        MOCK_METHOD(void, GetResult, (IMUData_t *Data));

};

#endif
