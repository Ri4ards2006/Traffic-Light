#ifndef EXTENDEDTOUCHEVENT_HAL_TOUCHSCREEN_H
#define EXTENDEDTOUCHEVENT_HAL_TOUCHSCREEN_H

#include <cstdint>
#include "Debug.h"

// Hardware abstraction layer for TS_Point with platform independent low level Interface to touchscreen
class HAL_TouchscreenPoint {
public:
    HAL_TouchscreenPoint() : x(0), y(0), z(0) {}
    HAL_TouchscreenPoint(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z) {}
    virtual ~HAL_TouchscreenPoint() = default;

    virtual bool operator==(const HAL_TouchscreenPoint &p) const {
        DEBUG_ETE("== in Point_on_TS\n")
        return ((p.x == x) && (p.y == y) && (p.z == z));
    }

    int16_t x, y, z;
};

// see https://learn.adafruit.com/adafruit-gfx-graphics-library/rotating-the-display
enum class ROTATION : uint8_t {
    PORTRAIT_TR = 0,  // display to portrait mode, with the USB jack at the top right
    LANDSCAPE_BR = 1, // display to landscape mode, with the USB jack at the bottom right
    PORTRAIT_BL = 2,  // display to portrait mode, with the USB jack at the bottom left
    LANDSCAPE_TL = 3  // display to landscape mode, with the USB jack at the top left
};

inline uint8_t fromRotation(const ROTATION n) {
   return static_cast<uint8_t>(n);
}

inline ROTATION toRotation(const uint8_t n) {
   return static_cast<ROTATION>(n % 4);
}


// Hardware abstraction layer for Touchscreen
class HAL_Touchscreen {
public:
    HAL_Touchscreen() = default;
    virtual ~HAL_Touchscreen() = default;

    virtual bool begin() = 0;

    virtual HAL_TouchscreenPoint getPoint() = 0;
    virtual bool tirqTouched() = 0;
    virtual bool touched() = 0;
    virtual void readData(uint16_t *x, uint16_t *y, uint8_t *z) = 0;
    virtual bool bufferEmpty() = 0;
    virtual uint8_t bufferSize() = 0;

    virtual void setRotation(ROTATION n) = 0;

    // implementor returns raw coordinates
    virtual bool coordinatesAreRaw() = 0;
};

#endif //EXTENDEDTOUCHEVENT_HAL_TOUCHSCREEN_H
