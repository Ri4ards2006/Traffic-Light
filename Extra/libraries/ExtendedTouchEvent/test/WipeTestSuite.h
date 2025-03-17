#//
// Created by hst on 22.06.24.
//

#ifndef EXTENDEDTOUCHEVENT_WIPETESTSUITE_H
#define EXTENDEDTOUCHEVENT_WIPETESTSUITE_H

#include <gtest/gtest.h>
#include "HAL_Touchscreen.h"
#include "ExtendedTouchEvent.h"

// ESP8266 crashes with OOM - may be too complex?
#ifndef ESP8266

using ::testing::Values;

// maps ROTATION to expected WIPE_DIRECTION
struct Wipes {
    Wipes(ROTATION rot, WIPE_DIRECTION dir, 
          bool reverse = false, bool vertical = false) : rot(rot), dir(dir), reverse(reverse), vertical(vertical) {};
    ~Wipes() = default;

    ROTATION rot;
    WIPE_DIRECTION dir;
    bool reverse;
    bool vertical;
};

class WipeTestSuite : public testing::TestWithParam<Wipes> {
public:
    ~WipeTestSuite() override = default;
};

INSTANTIATE_TEST_SUITE_P(test_ExtendedTouchEvent,
                         WipeTestSuite,
                         Values(Wipes(ROTATION::LANDSCAPE_BR, WIPE_DIRECTION::WD_RIGHT_LEFT)
                               , Wipes(ROTATION::LANDSCAPE_TL, WIPE_DIRECTION::WD_RIGHT_LEFT)
                               , Wipes(ROTATION::PORTRAIT_BL, WIPE_DIRECTION::WD_RIGHT_LEFT)
                               , Wipes(ROTATION::PORTRAIT_TR, WIPE_DIRECTION::WD_RIGHT_LEFT)

                               , Wipes(ROTATION::LANDSCAPE_BR, WIPE_DIRECTION::WD_LEFT_RIGHT, true)
                               , Wipes(ROTATION::LANDSCAPE_TL, WIPE_DIRECTION::WD_LEFT_RIGHT, true)
                               , Wipes(ROTATION::PORTRAIT_BL, WIPE_DIRECTION::WD_LEFT_RIGHT, true)
                               , Wipes(ROTATION::PORTRAIT_TR, WIPE_DIRECTION::WD_LEFT_RIGHT, true)

                               , Wipes(ROTATION::LANDSCAPE_BR, WIPE_DIRECTION::WD_TOP_DOWN, false, true)
                            //    , Wipes(ROTATION::LANDSCAPE_TL, WIPE_DIRECTION::WD_TOP_DOWN, false, true)
                            //    , Wipes(ROTATION::PORTRAIT_BL, WIPE_DIRECTION::WD_TOP_DOWN, false, true)
                            //    , Wipes(ROTATION::PORTRAIT_TR, WIPE_DIRECTION::WD_TOP_DOWN, false, true)

                               , Wipes(ROTATION::LANDSCAPE_BR, WIPE_DIRECTION::WD_BOTTOM_UP, true, true)
                            //    , Wipes(ROTATION::LANDSCAPE_TL, WIPE_DIRECTION::WD_BOTTOM_UP, true, true)
                            //    , Wipes(ROTATION::PORTRAIT_BL, WIPE_DIRECTION::WD_BOTTOM_UP, true, true)
                            //    , Wipes(ROTATION::PORTRAIT_TR, WIPE_DIRECTION::WD_BOTTOM_UP, true, true)
                         ));

#endif
#endif //EXTENDEDTOUCHEVENT_WIPETESTSUITE_H
