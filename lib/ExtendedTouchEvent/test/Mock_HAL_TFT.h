#ifndef MOCK_HAL_TFT_H
#define MOCK_HAL_TFT_H

#include "HAL_TFT.h"
#include <gmock/gmock.h>

class Mock_TFT : public HAL_TFT {
public:
    MOCK_METHOD(void, setRotation, (ROTATION r), (override));
    MOCK_METHOD(ROTATION, getRotation, (), (const override));
    MOCK_METHOD(int16_t, width, (), (const override));
    MOCK_METHOD(int16_t, height, (), (const override));
};

#endif