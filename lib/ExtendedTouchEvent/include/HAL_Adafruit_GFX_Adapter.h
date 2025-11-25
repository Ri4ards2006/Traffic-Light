#ifndef EXTENDEDTOUCHEVENT_HAL_TFT_ADAFRUIT_GFX_ADAPTER_H
#define EXTENDEDTOUCHEVENT_HAL_TFT_ADAFRUIT_GFX_ADAPTER_H

#include "HAL_TFT.h"
#include "Adafruit_GFX.h"
#include <iostream>

// Adapter between Hardware abstraction layer for TFT and real Adafruit_GFX
class HAL_Adafruit_GFX_Adapter : public HAL_TFT {
public:
    // Constructor
    explicit HAL_Adafruit_GFX_Adapter(Adafruit_GFX & gfx) : adaptee(gfx) {};

    void setRotation(ROTATION r) override {
        adaptee.setRotation(fromRotation(r));
    };

    ROTATION getRotation() const override {
        return toRotation(adaptee.getRotation());
    };

    int16_t width() const override {
        return adaptee.width();
    };


    int16_t height() const override {
        return adaptee.height();
    }

private:
    Adafruit_GFX & adaptee;
};

#endif //EXTENDEDTOUCHEVENT_HAL_TFT_ADAFRUIT_GFX_ADAPTER_H
