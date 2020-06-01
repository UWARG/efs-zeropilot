#ifndef ALTIMETER_MOCK_HPP
#define ALTIMETER_MOCK_HPP

#include <gmock/gmock.h> // Brings in gMock

#include "altimeter.hpp"


class MockAltimeter : public Altimeter {
    public: 
        MOCK_METHOD(void, Init, ());
        MOCK_METHOD(void, Begin_Measuring, ());
        MOCK_METHOD(void, GetResult, (AltimeterData_t *Data));

};

#endif
