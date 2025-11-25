#ifndef EXTENDEDTOUCHEVENT_HAL_TFT_ESPI_TOUCHSCREEN_ADAPTER_H
#define EXTENDEDTOUCHEVENT_HAL_TFT_ESPI_TOUCHSCREEN_ADAPTER_H

#include <TFT_eSPI.h>
#include "HAL_Touchscreen.h"
#include "Debug.h"

#define Z_THRESHOLD     500

class TS_Point {
public:
	TS_Point(void) : x(0), y(0), z(0) {}
	TS_Point(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z) {}
	bool operator==(TS_Point p) { return ((p.x == x) && (p.y == y) && (p.z == z)); }
	bool operator!=(TS_Point p) { return ((p.x != x) || (p.y != y) || (p.z != z)); }
	int16_t x, y, z;
};

//
// Adapter between hardware abstraction layer and low level touchscreen API
//
class HAL_TFT_eSPI_TouchscreenPoint_Adapter : public HAL_TouchscreenPoint {
public:
    HAL_TFT_eSPI_TouchscreenPoint_Adapter() : adaptee(TS_Point(x, y, z)) {}
    explicit HAL_TFT_eSPI_TouchscreenPoint_Adapter(TS_Point tsp) : adaptee(tsp) {}

    bool operator==(const HAL_TFT_eSPI_TouchscreenPoint_Adapter &p) const {
        DEBUG_ETE("== in HAL_TFT_eSPI_TouchscreenPoint_Adapter\n");
        return static_cast<TS_Point>(adaptee) == p.adaptee;
    }

    bool operator!=(const HAL_TFT_eSPI_TouchscreenPoint_Adapter &p) const {
        DEBUG_ETE("!= in HAL_TFT_eSPI_TouchscreenPoint_Adapter\n");
        return static_cast<TS_Point>(adaptee) != p.adaptee;
    }

private:
    TS_Point adaptee;
};


class HAL_TFT_eSPI_XPT2046_TS_Adapter : public HAL_Touchscreen {
public:
    explicit HAL_TFT_eSPI_XPT2046_TS_Adapter(TFT_eSPI & ts) : adaptee(ts) {}

    bool begin() override {
        DEBUG_ETE("begin() in HAL_XPT2046_TS_Adapter\n");
        adaptee.begin();
        return true;
    }

    HAL_TouchscreenPoint getPoint() override {
        DEBUG_ETE("getPoint() in HAL_XPT2046_TS_Adapter\n");
        uint16_t x, y, z;

        if (adaptee.getTouch(&x, &y)) {
            lastKnownPoint = HAL_TouchscreenPoint(x, y, Z_THRESHOLD);
        } else if (lastKnownPoint.z) {
            lastKnownPoint.z = 0;
        }

        DEBUG_ETE("getPoint() in HAL_TFT_eSPI_XPT2046_TS_Adapter tsPoint.x=%4d, y=%4d, z=%4d\n", lastKnownPoint.x, lastKnownPoint.y, lastKnownPoint.z);
        return lastKnownPoint;
    }

    bool tirqTouched() override {
        DEBUG_ETE("tirqTouched() in HAL_XPT2046_TS_Adapter\n");
        // TODO
        return false;
    };

    bool touched() override {
        DEBUG_ETE("touched() in HAL_XPT2046_TS_Adapter\n");
        return adaptee.getTouchRawZ() >= Z_THRESHOLD;
    };

    void readData(uint16_t *x, uint16_t *y, uint8_t *z) override {
        DEBUG_ETE("readData() in HAL_XPT2046_TS_Adapter\n");
        adaptee.getTouchRaw(x, y);
        *z = adaptee.getTouchRawZ();

    };

    bool bufferEmpty() override {
        DEBUG_ETE("bufferEmpty() in HAL_XPT2046_TS_Adapter\n");
        // TODO ?
        return true;
    };

    uint8_t bufferSize() override {
        DEBUG_ETE("bufferSize() in HAL_XPT2046_TS_Adapter\n");
        // TODO ?
        return 1;
    }

    void setRotation(const ROTATION n) override {
        DEBUG_ETE("setRotation() in HAL_XPT2046_TS_Adapter\n");
        adaptee.setRotation(fromRotation(n));
        // TFT_eSPI needs different sets of calibration data for the different rotations
        adaptee.setTouch(calData[fromRotation(n)]);
    };

     bool coordinatesAreRaw() override {
         return false;
     }

     /* set calibration data for a specific rotation
      * @param r ROTATION
      * @param cd desired calibration data for specific rotation
      */
     void setCalibrationData(const ROTATION r, const uint16_t cd[5]) {
         memcpy(calData[fromRotation(r)], cd, sizeof(uint16_t) * 5);
     }

     /* set calibration data for all rotations
      * @param cd desired calibration data for all rotations
      */
     void setCalibrationData(const uint16_t cd[4][5]) {
         memcpy(calData, cd, sizeof(uint16_t) * 20);
     }

private:
    TFT_eSPI & adaptee;
    HAL_TouchscreenPoint lastKnownPoint;
    // used to manage the different sets of calibration data for the rotations
    uint16_t calData[4][5];

};

#endif // EXTENDEDTOUCHEVENT_HAL_TFT_ESPI_TOUCHSCREEN_ADAPTER_H
