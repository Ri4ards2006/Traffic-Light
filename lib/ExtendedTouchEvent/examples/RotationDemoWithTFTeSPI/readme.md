This example demonstrates how to use the ExtendedTouchEvent library, especially usage of the methods 
**setRotation()**, **isInRectangle()** and **lastTouchEventTime()**. It also implements a simple screensaver which 
dims screen and sets processor into light sleep in case of longer inactivity. 

Orientation of the screen is changed (cycled through all 4 possibilities) if you touch the small green rectangle. 
After that the srceen is redrawn with the new orientation.

If **USE_PWM** is defined, brightness of display will be controlled by y-coordinate of last click (the higher the brighter) 
using PWM. Otherwise the screensaver simply switches on and off.

It uses a 2.4" TFT display (with an ILI9341 controller) with resistive touch screen (XPT2046) and uses the 
[TFT_eSPI library](https://github.com/Bodmer/TFT_eSPI) for accessing both TFT and touchpanel. If **USE_ILI9488** is 
defined a 3,5" TFT display (with an ILI9488 controller) is used instead.

The module is used together with an ESP32 but any other arduino compatible processor should work.

For the used pins / configuration of TFT_eSPI see definitions in the code (board_setup.h) (**don't forget to to check whether they fit to your board**)
