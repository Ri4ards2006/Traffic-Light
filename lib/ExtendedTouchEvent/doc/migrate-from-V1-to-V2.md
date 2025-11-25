# Short guide how to migrate from V1 to V2

- bump library dependency to V2 (either in platformio.ini or in arduinoIDE)
- add the two new header files
  ```C++
  #include "HAL_Adafruit_GFX_Adapter.h" // adapter for graphics driver
  #include "HAL_XPT2046_TouchscreenAdapter.h"   // adapter for touchscreen driver
  ```
- wrap *Adafruit_ILI9341* **and** *XPT2046_Touchscreen* objects with the corresponding wrapper classes *HAL_Adafruit_GFX_Adapter* resp. *HAL_XPT2046_TS_Adapter* and use these  wrapper objects to instantiate the *ExtendedTouchEvent* object
  ```C++
  HAL_Adafruit_GFX_Adapter tftadapter = HAL_Adafruit_GFX_Adapter(tft);
  HAL_XPT2046_TS_Adapter touchadapter = HAL_XPT2046_TS_Adapter(touch);
  ExtendedTouchEvent tevent(touchadapter, tftadapter);
  ```
- replace in **all** calls in your code using the *XPT2046_Touchscreen* object that object with the *HAL_XPT2046_TS_Adapter* - for example ~~touch.begin();~~ touchadapter.begin();

- replace in **those** calls in your code using the methods of *Adafruit_ILI9341* covered by *HAL_Adafruit_GFX_Adapter* [^1] that object with the *HAL_Adafruit_GFX_Adapter* object - for example ~~touch.height();~~ touchadapter.height(); or ~~tft.width();~~ tftadapter.width();

- replace all occurences of *TS_Point* by *HAL_TouchscreenPoint* and the signatures of your callback methods 
  used to react on the events raised by the library from either

  -  ~~void (TS_Point_callback)(**TS_Point** p);~~ to  void (TS_Point_callback)(**HAL_TouchscreenPoint &** p);

    or

  -  ~~void onAllEvents(**TS_Point**, EVENT)~~ to void onAllEvents(**HAL_TouchscreenPoint**, EVENT)
- *setRotation()* and *getRotation()* no longer use uint8_t as argument or return value. Instead the scoped enum **ROTATION** is used and your code must be adjusted accordingly

---
[^1]: *HAL_Adafruit_GFX_Adapter* covers only a subset of the methods offered by *Adafruit_ILI9341* - those needed inside this library. The rest (the vast majority) especially all functions used for drawing must be still accessed  with the *Adafruit_ILI9341* object. The methods that have to be changed can be found in **HAL_Adafruit_GFX_Adapter.h**