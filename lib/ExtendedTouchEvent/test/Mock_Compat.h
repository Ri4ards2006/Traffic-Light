#ifndef MOCK_COMPAT_H
#define MOCK_COMPAT_H

#include "Compat.h"
#include <gmock/gmock.h>

class Mock_Compat : public Compat {
public:
    MOCK_METHOD(unsigned long, ll_millis, (), (override));
    MOCK_METHOD(long, ll_map, (long x, long in_min, long in_max, long out_min, long out_max), (override));
};

#endif