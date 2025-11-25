#//
// Created by hst on 22.06.24.
//

#ifndef EXTENDEDTOUCHEVENT_UNRAWTESTSUITE_H
#define EXTENDEDTOUCHEVENT_UNRAWTESTSUITE_H

#include <gtest/gtest.h>
#include "HAL_Touchscreen.h"
#include "ExtendedTouchEvent.h"

// ESP8266 crashes with OOM - may be too complex?
#ifndef ESP8266

using ::testing::Values;

// maps ROTATION to expected WIPE_DIRECTION
struct Touches {
    Touches(const ROTATION rot, const uint16_t x, const uint16_t y) : rot(rot), x(x), y(y) {};
    ~Touches() = default;

    const ROTATION rot;
    const uint16_t x;
    const uint16_t y;
};

class UnrawTestSuite : public testing::TestWithParam<Touches> {
public:
    ~UnrawTestSuite() override = default;
};

INSTANTIATE_TEST_SUITE_P(test_ExtendedTouchEvent,
                         UnrawTestSuite,
                         Values(Touches(ROTATION::PORTRAIT_TR,  20, 30)
                              , Touches(ROTATION::LANDSCAPE_BR, 20, 30)
                              , Touches(ROTATION::PORTRAIT_BL,  20, 30)
                              , Touches(ROTATION::LANDSCAPE_TL, 20, 30)

                              , Touches(ROTATION::PORTRAIT_TR,  220, 30)
                              , Touches(ROTATION::LANDSCAPE_BR, 300, 30)
                              , Touches(ROTATION::PORTRAIT_BL,  220, 30)
                              , Touches(ROTATION::LANDSCAPE_TL, 300, 30)

                              , Touches(ROTATION::PORTRAIT_TR,  220, 290)
                              , Touches(ROTATION::LANDSCAPE_BR, 300, 210)
                              , Touches(ROTATION::PORTRAIT_BL,  220, 290)
                              , Touches(ROTATION::LANDSCAPE_TL, 300, 210)

                              , Touches(ROTATION::PORTRAIT_TR,   20, 290)
                              , Touches(ROTATION::LANDSCAPE_BR,  20, 210)
                              , Touches(ROTATION::PORTRAIT_BL,   20, 290)
                              , Touches(ROTATION::LANDSCAPE_TL,  20, 210)
                         ));

#endif
#endif //EXTENDEDTOUCHEVENT_UNRAWTESTSUITE_H
