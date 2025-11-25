#ifndef EXTENDEDTOUCHEVENT_HAL_XPT2046_TOUCHSCREEN_ADAPTER_H
#define EXTENDEDTOUCHEVENT_HAL_XPT2046_TOUCHSCREEN_ADAPTER_H

#include "XPT2046_Touchscreen.h"
#include "HAL_Touchscreen.h"
#include "Debug.h"

//
// Adapter between hardware abstraction layer and low level touchscreen API
//
class HAL_TouchscreenPoint_Adapter : public HAL_TouchscreenPoint {
public:
    HAL_TouchscreenPoint_Adapter() : adaptee(* (new TS_Point())) {}
    explicit HAL_TouchscreenPoint_Adapter(TS_Point & tsp) : adaptee(tsp) {}

    bool operator==(const HAL_TouchscreenPoint_Adapter &p) const {
        DEBUG_ETE("== in AL_TouchscreenPoint_Adapter\n");
        return static_cast<TS_Point>(adaptee) == p.adaptee;
    }

    bool operator!=(const HAL_TouchscreenPoint_Adapter &p) const {
        DEBUG_ETE("!= in AL_TouchscreenPoint_Adapter\n");
        return static_cast<TS_Point>(adaptee) != p.adaptee;
    }

private:
    TS_Point & adaptee;
};


class HAL_XPT2046_TS_Adapter : public HAL_Touchscreen {
public:
    explicit HAL_XPT2046_TS_Adapter(XPT2046_Touchscreen & ts) : adaptee(ts) {}

    bool begin() override {
        DEBUG_ETE("begin() in HAL_XPT2046_TS_Adapter\n");
        return adaptee.begin();
    }

    HAL_TouchscreenPoint getPoint() override {
        DEBUG_ETE("getPoint() in HAL_XPT2046_TS_Adapter\n");
        const TS_Point tsPoint = adaptee.getPoint();
        DEBUG_ETE("getPoint() in HAL_XPT2046_TS_Adapter tsPoint.x=%d, y=%d\n", tsPoint.x, tsPoint.y);
        return {tsPoint.x, tsPoint.y, tsPoint.z};
    }

    bool tirqTouched() override {
        DEBUG_ETE("tirqTouched() in HAL_XPT2046_TS_Adapter\n");
        return adaptee.tirqTouched();
    };

    bool touched() override {
        DEBUG_ETE("touched() in HAL_XPT2046_TS_Adapter\n");
        return adaptee.touched();
    };

    void readData(uint16_t *x, uint16_t *y, uint8_t *z) override {
        DEBUG_ETE("readData() in HAL_XPT2046_TS_Adapter\n");
        adaptee.readData(x, y, z);
    };

    bool bufferEmpty() override {
        DEBUG_ETE("bufferEmpty() in HAL_XPT2046_TS_Adapter\n");
        return adaptee.bufferEmpty();
    };

    uint8_t bufferSize() override {
        DEBUG_ETE("bufferSize() in HAL_XPT2046_TS_Adapter\n");
        return adaptee.bufferSize();
    }

    void setRotation(const ROTATION n) override {
        DEBUG_ETE("setRotation() in HAL_XPT2046_TS_Adapter\n");
        adaptee.setRotation(fromRotation(n));
    };

    virtual bool coordinatesAreRaw() {
        return true;
    }

private:
    XPT2046_Touchscreen & adaptee;

};

#endif // EXTENDEDTOUCHEVENT_HAL_XPT2046_TOUCHSCREEN_ADAPTER_H
