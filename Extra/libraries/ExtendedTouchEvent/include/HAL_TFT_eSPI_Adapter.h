//
//

#ifndef EXTENDEDTOUCHEVENT_HAL_TFT_ESPI_ADAPTER_H
#define EXTENDEDTOUCHEVENT_HAL_TFT_ESPI_ADAPTER_H

#include "HAL_TFT.h"
#include <TFT_eSPI.h> // Hardware-specific library


// Adapter between Hardware abstraction layer for TFT and real TFT_eSPI
class HAL_TFT_eSPI_Adapter : public HAL_TFT {
public:
    // Constructor
    explicit HAL_TFT_eSPI_Adapter(TFT_eSPI &gfx) : adaptee(gfx) {};

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
    TFT_eSPI &adaptee;

};

#endif //EXTENDEDTOUCHEVENT_HAL_TFT_ESPI_ADAPTER_H
