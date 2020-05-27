#ifndef IMU_MOCK_HPP
#define IMU_MOCK_HPP

#include "gmock/gmock.h" // Brings in gMock

#include "IMU.hpp"


class MockIMU : public IMU {
    public: 
        MOCK_METHOD0(Init, void());
        MOCK_METHOD0(Begin_Measuring, void());
        MOCK_METHOD1(GetResult, void(IMUData_t *Data));

};

#endif