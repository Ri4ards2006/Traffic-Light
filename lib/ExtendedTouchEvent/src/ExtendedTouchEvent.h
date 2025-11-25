/*
 * ExtendedTouchEvent.h
 *
 *  Created on: 17.05.2020
 *      Author: arduhe
 */

#ifndef EXTENDEDTOUCHEVENT_H_
#define EXTENDEDTOUCHEVENT_H_

#include "HAL_TFT.h"
#include "HAL_Touchscreen.h"
#include "Compat.h"

#define MOVETRESHOLD 10     // minimal movement to detect a move
#define WIPEX 500           // minimal X-movement to detect x wipe
#define WIPEY 700           // minimal Y-movement to detect y wipe
#define CLICKLONG 1000      // minimal time (in ms) to detect a long click
#define DOUBLECLICK 500     // minimal time (in ms) between two clicks to be recognized as a doubleclick
#define SCREENX 240         // screen resolution in pixels (smaller side)
#define SCREENY 320         // screen resolution in pixels (longer side)


// parameters for Touchscreen
#define MINPRESSURE 10	    // pressure to detect touch
#define TFT_SMALL_MIN 230 	// minimal return value for smaller side of panel
#define TFT_SMALL_MAX 3700  // maximal return value for smaller side of panel
#define TFT_LONG_MIN 350    // minimal return value for longer side of panel
#define TFT_LONG_MAX 3900   // maximal return value for longer side of panel
#define BLACKEDGE 200       // height of the dark area at the lower edge of longer side (touchsensitive but unlit area)

enum class EVENT : uint8_t {
    EVT_UP = 0,
    EVT_DOWN = 1,
    EVT_CLICK = 2,
    EVT_LONG = 3,
    EVT_WIPE = 4,
    EVT_DRAW = 5,
    EVT_DBLCLICK = 6
};

enum class WIPE_DIRECTION : uint8_t {
    WD_LEFT_RIGHT = 0,
    WD_RIGHT_LEFT = 1,
    WD_BOTTOM_UP = 2,
    WD_TOP_DOWN = 3
};

/** type definition for callback methods 
 * if not specially mentioned all registerXyz-methods expect as argument the address of a method with this signature
 */
typedef void (TS_Point_callback)(HAL_TouchscreenPoint & p);

constexpr int16_t OUTSIDE_CHART = -1;
constexpr int16_t ILL_ARG_ANGLE = -2;
constexpr int16_t ILL_ARG_SUM_ANGLE = -3;

class ExtendedTouchEvent {
public:
    virtual ~ExtendedTouchEvent() = default;

    /** 
     * @param touch reference to HAL_Touchscreen
     * @param tft reference to HAL_TFT 
     * @param black height of the dark area at the lower edge of longer side (touchsensitive but unlit area)
     */
    ExtendedTouchEvent(HAL_Touchscreen &touch, HAL_TFT &tft, int16_t black = BLACKEDGE);

    /** 
     * @param touch reference to HAL_Touchscreen
     * @param tft reference to HAL_TFT 
     * @param compat reference to Compat (only for tests - no need to use in production code)
     * @param black height of the dark area at the lower edge of longer side (touchsensitive but unlit area)
     */
    ExtendedTouchEvent(HAL_Touchscreen &touch, HAL_TFT &tft, Compat & compat, int16_t black = BLACKEDGE);

    /**
     * sets the rotation of touchscreen and tft to a new value. Allowed values are defined in ROTATION
     * @param new value for rotation
     */
    void setRotation(ROTATION n);

    /**
     * checks whether a point is inside a rectangle defined by startpoint (x, y) and width w and height h. The
     * rectangle is defined by the diagonal opposite points (x, y) and (x + w, y + h)
     * @param p Point to be checked
     * @param x x-coordinate of startpoint of rectangle to be checked
     * @param y y-coordinate of startpoint of rectangle to be checked
     * @param w width of rectangle to be checked
     * @param h height of rectangle to be checked
     */ 
    bool isInRectangle(const HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t w, int16_t h);

    /**
     * checks whether a point is inside a circle defined by ist centerpoint (x, y) and radius r.
     * @param p Point to be checked
     * @param x x-coordinate of the centerpoint
     * @param y y-coordinate of the centerpoint
     * @param r radius of circle to be checked
     */
    bool isInCircle(const HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t r);

    /**
     * checks whether a point is inside a polygon. The polygon is defined by its vertices (atleast 3) and may be simple, i.e. convex, concave or star shaped, 
     * but <b>not self-intersecting</b>. For a definition of these terms please have a look at https://en.wikipedia.org/wiki/Polygon#Convexity_and_non-convexity
     * @param p Point to be checked
     * @param nvert number of vertices in the polygon, greater than 2
     * @param vert two dimensional array with the coordinates of the vertices order of the points must be either clockwise
     *   or counterclockwise
     */
    bool isInPolygon(const HAL_TouchscreenPoint & p, int16_t nvert, int16_t vert[][2]);

    /**
     * checks whether a point is inside a slice of a piechart. Piechart ist defined by ints centerpoint(x, y) and
     * radius r and an array of angles. The angles are used like usual in mathematics, i.e. <b>counterclockwise</b>
     * and start is on the x-axis. Incomplete charts (sum of all angles < 360) are supported.
     * @param p Point to be checked
     * @param x x-coordinate of the centerpoint
     * @param y y-coordinate of the centerpoint
     * @param r radius of piechart to be checked
     * @param nangles number of slices in the piechart
     * @param angles array with the size of each slice as angles (0° .. 360°) - ordered counterclockwise
     * @returns 0 .. nvert-1 if inside, returned number is the index in angles to the slice which matches
     *          OUTSIDE_CHART if p lies outside the circle defined by (x, y, r)
     *          ILL_ARG_ANGLE if a member of angles[] is <= 0
     *          ILL_ARG_SUM_ANGLE if sum of angles[] > 360
     */
    int16_t isInPieSlice(const HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t r,
                         int16_t nangles, const int16_t angles[]);

    /**
     * overloaded function which uses percentage to describe the pie chart
     * @param p Point to be checked
     * @param x x-coordinate of the centerpoint
     * @param y y-coordinate of the centerpoint
     * @param r radius of piechart to be checked
     * @param npercent number of slices in the piechart
     * @param percent array with the percentages of the slices (0 < percentage <= 1) - ordered counterclockwise
     * @returns 0 .. nvert-1 if inside, returned number is the index in angles to the slice which matches
     *          OUTSIDE_CHART if p lies outside the circle defined by (x, y, r)
     *          ILL_ARG_ANGLE if a member of angles[] is <= 0
     *          ILL_ARG_SUM_ANGLE if sum of angles[] > 1
     */
    int16_t isInPieSlice(const HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t r,
                         int16_t npercent, const float percent[]);





    /** returns the time (in ms) the last touch event occured */
    unsigned long lastTouchEventTime();

    /**
     * main function of this library which should be called in the main loop of the sketch
     * positions of recognized touches will be checked and under certain conditions the corresponding events will be raised
     */
    void pollTouchScreen();

    /** size for the used TFT display in pixels, defaults are SCREENX resp. SCREENY */
    void setResolution(int16_t xResolution, int16_t yResolution);

    /** Toggles draw mode on or off
     * move events will be triggered only if in draw mode
     * wipe bevents only if not in draw mode
     */
    void setDrawMode(bool drawMode);

    //values to define the top left and the bottom right values
    //returned from the touch screen. default values are for
    //for a 2.4 inch TFT display with 320 x 240 pixels
    void calibrate(uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax);

    /**
     * define the minimal difference to the last point to detect a move, defaults to 10
     */
    void setMoveTreshold(uint16_t threshold);

    /** sets the minimal distance between touch start and touch end to
     * detect a wipe event, defaults for wipeX resp. wipeY are WIPEX resp. WIPEY
     */
    void setWipe(uint16_t wipeX, uint16_t wipeY);

    /** sets the minimal time (in ms) to interpret a click as a long click,  default = CLICKLONG ms */
    void setLongClick(uint16_t clickLong);

    /** sets the minimal time between two clicks to be detected as double click, default = DOUBLECLICK ms */
    void setDblClick(uint16_t dblclick);

    /** register a callback function for a start touch event */ 
    void registerOnTouchDown(TS_Point_callback * callback);

    /** register a callback function for the end of a touch */
    void registerOnTouchUp(TS_Point_callback * callback);

    /** register a callback function to be called for touch click events, i.e. touch plus release, fired after release */
    void registerOnTouchClick(TS_Point_callback * callback);

    /** register a callback function for handling double click events
     * @see setDblClick
     */
    void registerOnTouchDblClick(TS_Point_callback * callback);

    /** register a callback function for handling long touch events
     * @see setLongClick
     */
    void registerOnTouchLong(TS_Point_callback * callback);

    /** register a callback function for touch draw events */
    void registerOnTouchDraw(TS_Point_callback * callback);

    /** register a callback function for touch wipe events
     * @param callback adress of a method with signature void callback(WIPE_DIRECTION)
     */
    void registerOnTouchWipe(void (*callback)(WIPE_DIRECTION direction));

    /** register a callback on any event.
     * @param callback adress of a method with signature void onAllEvents(HAL_TouchscreenPoint, EVENT)
     */
    void registerOnAllEvents(void (*callback)(HAL_TouchscreenPoint & p, EVENT event));

private:
    HAL_Touchscreen & _touch;
    HAL_TFT & _tft;
    Compat & _compat;
    // need a non temporary for referencing as default in constructor
    static Compat COMPAT; 

    int16_t _black;
    // time of last touch
    unsigned long _lasttouch = 0;

    HAL_TouchscreenPoint _lastMove; //last move position
    HAL_TouchscreenPoint _start; //position at start touch
    bool _down = false; //last touch status
    uint32_t _begin = 0; //start of last touched
    uint32_t _lastClick = 0; //time of last click used for doubleclick detection
    bool _long = false; //a long click event was already triggered
    bool _drawMode = true; //draw mode means move events but no wipe events
    //setup
    int16_t _xResolution = SCREENX; //screen resolution in pixels in x direction
    int16_t _yResolution = SCREENY; //screen resolution in pixels in y direction
    uint16_t _moveth = MOVETRESHOLD; //define the minimal movement to detect a move
    uint16_t _wipeX = WIPEX; //define a minimum movement to detect a wipe in direction of x-axis
    uint16_t _wipeY = WIPEY; //define a minimum movement to detect a wipe in direction of y-axis
    uint16_t _clickLong = CLICKLONG; //define a minimum time to detect a long click
    uint16_t _dblClick = DOUBLECLICK; //define a minimum time between two clicks to detect a doubleclick

    uint16_t _minPress = MINPRESSURE; //pressure to detect touch
    uint16_t _tsSmallMin = TFT_SMALL_MIN; //minimal x return value
    uint16_t _tsSmallMax = TFT_SMALL_MAX;  //maximal x return value
    uint16_t _tsLongMin = TFT_LONG_MIN; //minimal y return value
    uint16_t _tsLongMax = TFT_LONG_MAX; //maximal y return value

    TS_Point_callback * _onTouchDown = nullptr;
    TS_Point_callback * _onTouchUp = nullptr;
    TS_Point_callback * _onTouchLong = nullptr;
    TS_Point_callback * _onTouchDraw = nullptr;
    TS_Point_callback * _onTouchClick = nullptr;
    TS_Point_callback * _onTouchDblClick = nullptr;
    void (*_onTouchWipe)(WIPE_DIRECTION direction) = nullptr;
    void (*_onAllEvents)(HAL_TouchscreenPoint & scr, EVENT event) = nullptr;

    //calculate wipe direction from start and end point
    WIPE_DIRECTION wipeDirection(HAL_TouchscreenPoint & start, HAL_TouchscreenPoint & end);

    // call specialized callback for delivering event, additionally calls fireAllEvents
    void fireEvent(TS_Point_callback *callback, HAL_TouchscreenPoint & scr, EVENT type);

    // call _onAllEvents callback for all events
    void fireAllEvents(HAL_TouchscreenPoint & scr, EVENT type);

    //convert raw position into screen coordinates
    HAL_TouchscreenPoint toScreen(HAL_TouchscreenPoint & p);
};

#endif /* EXTENDEDTOUCHEVENT_H_ */
