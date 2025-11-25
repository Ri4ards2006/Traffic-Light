#ifndef EXTENDEDTOUCHEVENT_HAL_TFT_H
#define EXTENDEDTOUCHEVENT_HAL_TFT_H

#include "HAL_Touchscreen.h"

//
// Hardware abstraction layer for TFT with reduced function set
class HAL_TFT {
public:
    // Constructor
    HAL_TFT() = default;
    virtual ~HAL_TFT() = default;

   
     /************************************************************************/
    /*!
      @brief      Set rotation setting for display
      @param      r choosen ROTATION
    */
    /************************************************************************/
    virtual void setRotation(ROTATION r) = 0;

    /************************************************************************/
    /*!
      @brief      Get rotation setting for display
      @returns    ROTATION corresponding to the previous choosen orientation
    */
    /************************************************************************/
    virtual ROTATION getRotation() const = 0;

    /************************************************************************/
    /*!
      @brief      Get width of the display, accounting for current rotation
      @returns    Width in pixels
    */
    /************************************************************************/
    virtual int16_t width() const = 0;
    /************************************************************************/
    /*!
      @brief      Get height of the display, accounting for current rotation
      @returns    Height in pixels
    */
    /************************************************************************/
    virtual int16_t height() const = 0;

};
#endif //EXTENDEDTOUCHEVENT_HAL_TFT_H
