#ifndef MOCK_HAL_XPT2046_TOUCHSCREEN_H
#define MOCK_HAL_XPT2046_TOUCHSCREEN_H

#include "HAL_Touchscreen.h"
#include <gmock/gmock.h> 


class Mock_XPT2046_Touchscreen : public HAL_Touchscreen {
public:
    MOCK_METHOD(HAL_TouchscreenPoint, getPoint, (), (override));
	MOCK_METHOD(bool, begin, (), (override));
    MOCK_METHOD(bool, tirqTouched, (), (override));
    MOCK_METHOD(bool, touched, (), (override));
    MOCK_METHOD(void, readData, (uint16_t *x, uint16_t *y, uint8_t *z), (override));
    MOCK_METHOD(bool, bufferEmpty, (), (override));

	MOCK_METHOD(void, setRotation, (ROTATION n ), (override));
	MOCK_METHOD(uint8_t, bufferSize, (), (override));

    MOCK_METHOD(bool, coordinatesAreRaw, (), (override));
};

#endif