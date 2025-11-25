# API Documentation

To init the class you need to pass references of instances of XPT2046_Touchscreen and Adafruit_GFX.

Put a call to the **pollTouchScreen()** function of this library into the main loop. This function keeps track about touch position and touch state. For all registered callbacks, the corresponding functions will be called on the following [events](/doc/API-documentation.md#event):

- **EVT_DOWN** Touch screen has been touched. The callback function gets the position in the displays coordinates.
- **EVT_UP**   Touch screen is no longer touched. The callback function gets the position in the displays coordinates.
- **EVT_CLICK** Touch screen was touched and released. The callback function gets the position in the displays coordinates.
- **EVT_DBLCLICK** Touch screen was touched and released twice in a short time. The callback function gets the position in the displays coordinates.
- **EVT_LONG** Touch screen was touched for a longer time before release. The callback function gets the position in the displays coordinates.
- **EVT_DRAW** Touchpoint was moved while touched. This event is only active if draw mode is *true*. The callback function gets the position in the displays coordinates.
- **EVT_WIPE** Touch was moved a certain distance while touched. This event is only active if draw mode is *false*. The callback function gets the direction of the movement.

All required parameters will be initialized with common valid default values and can be changed at run time.

## types used in API
### classes
#### **ExtendedTouchEvent**
The main class of this library - doing most of the stuff.

---
### helper classes
#### **HAL_TouchscreenPoint**
Represents a point on the screen - mostly used in the callbacks.
 
#### **HAL_Touchscreen**
  Hardware abstraction layer for [XPT2046_Touchscreen](https://github.com/PaulStoffregen/XPT2046_Touchscreen)
  
#### **HAL_TFT**
  Hardware abstraction layer for TFT-API. Based on [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library) with a reduced subset of the functions defined in Adafruit_GFX (those used inside this library).

#### **Compat**
  Internal class - not intended for use in sketches.

---
### typedefs
#### **void (TS_Point_callback)(HAL_TouchscreenPoint & p)**
  signature for most of the callbacks called when the display is touched, wiped and so on.

---
### scoped enums
#### **ROTATION**
Represents the orientation of d´the touchscreen and has the following allowed values
   - **PORTRAIT_TR**
     display to portrait mode, with the USB jack at the top right
   - **LANDSCAPE_BR**
     display to landscape mode, with the USB jack at the bottom right
   - **PORTRAIT_BL** 
     display to portrait mode, with the USB jack at the bottom left
   - **LANDSCAPE_TL**
     display to landscape mode, with the USB jack at the top left

[see also](https://learn.adafruit.com/adafruit-gfx-graphics-library/rotating-the-display)

#### **EVENT**
Distinguishes the the different type of events
  - EVT_UP
  - EVT_DOWN
  - EVT_CLICK
  - EVT_LONG
  - EVT_WIPE
  - EVT_DRAW
  - EVT_DBLCLICK

#### **WIPE_DIRECTION**
  - WD_LEFT_RIGHT
  - WD_RIGHT_LEFT
  - WD_BOTTOM_UP
  - WD_TOP_DOWN

## Wrapperclasses for low level drivers for TFT and touchpanel access
Current there are four wrapperclasses which are used as adapters to the low level
device drivers
- **HAL_Adafruit_GFX_Adapter**
  wraps the [Adafruit_GFX](https://registry.platformio.org/libraries/adafruit/Adafruit%20GFX%20Library) class
- **HAL_XPT2046_TS_Adapter**
  wraps [XPT2046_Touchscreen](https://registry.platformio.org/libraries/paulstoffregen/XPT2046_Touchscreen)

suitable for TFTs with ILI9341 controller and XPT2046 Touchscreenpanel

- **HAL_TFT_eSPI_Adapter**
  wraps [TFT_eSPI](https://registry.platformio.org/libraries/bodmer/TFT_eSPI)
- **HAL_TFT_eSPI_TouchscreenPoint_Adapter**
  wraps [TFT_eSPI](https://registry.platformio.org/libraries/bodmer/TFT_eSPI)

suitable for TFTs with ILI9341 or ILI9488 controller and XPT2046 Touchscreenpanel when using
TFT_eSPI as graphic lib. Other TFTs supported by TFT_eSPI should work - **but are untested, so use is own risk**



## Reference to all defined functions

#### **void pollTouchScreen()**
This is the main function which should be called in the main loop
positions will be checked and under certain conditions events will be triggered

### Initialization and customizing 
  can be achieved with the following methods

#### **void setResolution(int16_t xResolution, int16_t yResolution)**
  the number of pixels for the used TFT display default is w=240 h=320, call once during setup()

#### **void calibrate(uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax)**
  values to define the top left and the bottom right values returned from the touch screen. 
  default values are for for a 2.4" TFT display with 320 x 240 pixels

#### **void setDrawMode(boolean drawMode)**
  Switches draw mode on or off. Move events will be triggered only if draw mode is true, wipe events if it is false

#### **void setMoveTreshold(uint16_t threshold)**
  define the minimal difference to the last point to detect a move
  default = 10

#### **void setWipe(uint16_t wipeX, uint16_t wipeY)**
  define the minimal distance between touch start- and endpoint to detect a wipe event defaults = 500 and 700

#### **void setLongClick(uint16_t clickLong)**
  define the minimal time in ms to interpret a click as a long click, default = 1000 ms

#### **void setDblClick(uint16_t dblclick)**
  sets the minimal time (in ms) between two clicks to be recognized as a doubleclick, defaults to 500ms 

---
### Useful functions

#### **boolean isInRectangle(HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t w, int16_t h)**
  function to detect if a position is inside a rectangle starting at (x, y) with width w and height h. Helps to detect that a touch hits a button

#### **boolean isInCircle(HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t r)** 
  checks whether a Point *p* is inside a circle with centre at (*x, y*) and radius *r*

#### **boolean isInPolygon(HAL_TouchscreenPoint & p, int16_t nvertices, int16_t vertices[][2])**
  checks whether a Point *p* is inside a polygon, which is defined by the number vertices *nvertices* and a corresponding 
  array with the pairs of x- and y-coordinates of each vertex. Only simple *convex* or *concave* polygons are supported, 
  for an explanation of these terms please have a look at https://en.wikipedia.org/wiki/Polygon. Order of the vertices 
  in the array **must** be clockwise or counterclockwise.

#### **int16_t isInPieSlice(const HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t r, int16_t nangles, const int16_t angles[])**
  Checks whether a point is inside a slice of a piechart. The piechart is defined by its centerpoint(x, y) and radius r 
  and the angles forming the slices ordered counterclockwise 

#### **int16_t isInPieSlice(const HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t r, int16_t npercent, const float percent[])**
  Checks whether a point is inside a slice of a piechart. The piechart is defined by its centerpoint(x, y) and radius r 
  and the percentages (0 < percentage <= 1) setting the size of the slices ordered counterclockwise

#### **unsigned long lastTouchEventTime()**
  returns the time (in ms) since the last touch event occured. Use for decisions like starting screensaver or sleep of controler 

#### **void setRotation(ROTATION n)**
  sets the orientation of tft and touchscreen (turns the display logically). Each call ist delegated to the methods of the same name from  XPT2046\_Touchscreen and Adafruit\_GFX. The screen isn't redrawn.

---
### callbacks for event handling
Most callback functions have a signature of **void (TS\_Point\_callback)(HAL_TouchscreenPoint & p)** (accept a **HAL_TouchscreenPoint**, return nothing). Before use they have to be registered - typically once during setup of the sketch although it's possible too to register or unregister later.

#### void registerOnTouchDown(TS_Point_callback * callback)
  register a callback function for touch down start event

#### void registerOnTouchUp(TS_Point_callback * callback)
  register a callback function for touch up event

#### void registerOnTouchClick(TS_Point_callback * callback)
  register a callback function for click event

#### void registerOnTouchDblClick(TS_Point_callback * callback))
  register a callback function for doubleclick event

#### void registerOnTouchLong(TS_Point_callback * callback)
  register a callback function for touch long click event

#### void registerOnTouchDraw(TS_Point_callback * callback)
  register a callback function for touch draw event

#### void registerOnTouchWipe(void (\* callback)(WIPE_DIRECTION direction))
  register a callback for wipe events (accept a **WIPE_DIRECTION**, return nothing). **[WIPE_DIRECTION](/doc/API-documentation.md#wipe_direction)** is a typesafe enum representing the four possible movements

#### void registerOnAllEvents(void (\* callback)(HAL_TouchscreenPoint p, EVENT event))
  register a callback for any event - if you prefer a more centralized handler. **[EVENT](/doc/API-documentation.md#event)** is a typesafe enum describing the type of the event that occured

