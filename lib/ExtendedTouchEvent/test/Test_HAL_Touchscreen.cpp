//
// Test for HAL_TouchscreenPoint
//

#include "HAL_Touchscreen.h"
#include <gtest/gtest.h>

TEST(test_HAL_TouchscreenPoint, test_equals) {
    const HAL_TouchscreenPoint allzero;
    const HAL_TouchscreenPoint znonzero = HAL_TouchscreenPoint(0, 0, 1);
    const HAL_TouchscreenPoint yznonzero = HAL_TouchscreenPoint(0, 1, 2);
    const HAL_TouchscreenPoint allnonzero = HAL_TouchscreenPoint(1, 2, 3);

    ASSERT_FALSE(allzero == allnonzero);
    ASSERT_FALSE(allzero == yznonzero);
    ASSERT_FALSE(allzero == znonzero);

    ASSERT_TRUE(allnonzero == HAL_TouchscreenPoint(1, 2, 3));
}
