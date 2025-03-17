/*
 * ExtendedTouchEvent.cpp
 *
 *  Created on: 17.05.2020
 *      Author: arduhe
 */
#include "ExtendedTouchEvent.h"
#include "HAL_Touchscreen.h"
#include "HAL_TFT.h"
#include "Debug.h"
#include <cmath>

#define SQR(x) ((x) * (x))

// Instance of Compat for referencing as default in constructor
Compat ExtendedTouchEvent::COMPAT; 

ExtendedTouchEvent::ExtendedTouchEvent(HAL_Touchscreen &touch, HAL_TFT &tft, Compat & compat, int16_t black) :
        _touch(touch), _tft(tft), _compat(compat), _black(black) {}

ExtendedTouchEvent::ExtendedTouchEvent(HAL_Touchscreen &touch, HAL_TFT &tft, int16_t black) :
        _touch(touch), _tft(tft), _compat(ExtendedTouchEvent::COMPAT), _black(black) {}



void ExtendedTouchEvent::setRotation(ROTATION n) {
    _touch.setRotation(n);
    _tft.setRotation(n);
}

bool ExtendedTouchEvent::isInRectangle(const HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t w, int16_t h) {
    return ((x <= p.x) && (p.x <= x + w) && (y <= p.y) && (p.y <= y + h));
}

unsigned long ExtendedTouchEvent::lastTouchEventTime() {
    return _lasttouch;
}
//#####################################################

bool ExtendedTouchEvent::isInCircle(const HAL_TouchscreenPoint & p, int16_t x, int16_t y, int16_t r) {
    return SQR(x - p.x) + SQR(y - p.y) < SQR(r);
}
//#####################################################

bool ExtendedTouchEvent::isInPolygon(const HAL_TouchscreenPoint & p, int16_t nvertices, int16_t vertices[][2]) {
    bool c = false;
    for (int i = 0, j = nvertices - 1; i < nvertices; j = i++) {
        if ((p.x == vertices[i][0]) && (p.y == vertices[i][1])) {
            return true; // special case vertex
        }
        int dy = vertices[j][1] - vertices[i][1];
        if (((vertices[i][1] > p.y) != (vertices[j][1] > p.y)) &&
            ((dy < 0) ? (p.x * dy > ((vertices[j][0] - vertices[i][0]) * (p.y - vertices[i][1]) + vertices[i][0] * dy)) :
                        (p.x * dy < ((vertices[j][0] - vertices[i][0]) * (p.y - vertices[i][1]) + vertices[i][0] * dy)))) {
            c = ! c;
        }
    }
    return c;
}
//#####################################################

int16_t ExtendedTouchEvent::isInPieSlice(const HAL_TouchscreenPoint & p, const int16_t x, const int16_t y, const int16_t r,
                                         const int16_t npercent, const float percent[]) {

    int16_t angles[npercent];
    for (int i = 0; i < npercent; i++) {
        angles[i] = trunc(percent[i] * 360 + 0.5f);
    }

    return isInPieSlice(p, x, y, r, npercent, angles);
}
//#####################################################

int16_t ExtendedTouchEvent::isInPieSlice(const HAL_TouchscreenPoint & p, const int16_t x, const int16_t y, const int16_t r,
                                         const int16_t nangles, const int16_t angles[]) {
    float sum = 0;
    for (int i = 0; i < nangles; i++) {
        if (angles[i] <= 0) {
            return ILL_ARG_ANGLE;
        }
        sum += angles[i];
    }

    if (sum > 360) {
        return ILL_ARG_SUM_ANGLE;
    }

    if (ExtendedTouchEvent::isInCircle(p, x, y, r)) {
        // atan2(y, x) not x, y
        // because the origin of coordinates is not bottom left but top left, sign of y must be inverted
        // and convert to degrees (-pi .. pi -> 0 .. 360)
        double angle = atan2(y - p.y, p.x - x) / M_PI * 180;
        if (angle < 0) {
            angle += 360;
        }

        int16_t start = 0;
        for (int16_t i = 0; i < nangles; i++) {
            int16_t end = start + angles[i];

            if ((start <= angle) && (angle < end)) {
                return i;
            }
            start = end;
        }
    }

    return OUTSIDE_CHART;
}
//#####################################################

void ExtendedTouchEvent::pollTouchScreen() {
    bool tch;
    int16_t dx, dy;
    //first we get position and check if touched
    HAL_TouchscreenPoint p = _touch.getPoint();
    DEBUG_ETE("pollTouchScreen p=(%d %d %d)\n", p.x, p.y, p.z);
    tch = (p.z >= _minPress);

    uint32_t now = _compat.ll_millis();

    if (tch && _down) { //continous touch
        HAL_TouchscreenPoint scr = toScreen(p);

        dx = abs(_lastMove.x - p.x);
        dy = abs(_lastMove.y - p.y);
        _lastMove = p;
        if (_drawMode && ((dx > _moveth) || (dy > _moveth))) { //movement?
            fireEvent(_onTouchDraw, scr, EVENT::EVT_DRAW);
        }
        if (((now - _begin) > _clickLong) && (!_long)) {
            fireEvent(_onTouchLong, scr, EVENT::EVT_LONG);

            _long = true;
        }
    } else if (tch && !_down) { //start of touch
        HAL_TouchscreenPoint scr = toScreen(p);
        _start = p;
        _lastMove = p;
        _begin = now;
        _long = false;
        _down = true;

        fireEvent(_onTouchDown, scr, EVENT::EVT_DOWN);
    } else if (_down) { // stop of touch
        HAL_TouchscreenPoint scr = toScreen(p);
        _down = false;
        _long = false;
        fireEvent(_onTouchUp, scr, EVENT::EVT_UP);

        if (!_drawMode && ((abs(_start.x - p.x) > _wipeX) || (abs(_start.y - p.y) > _wipeY))) {
            if (_onTouchWipe)
                _onTouchWipe(wipeDirection(_start, p));

            fireAllEvents(scr, EVENT::EVT_WIPE);
        } else {
            if ((now - _begin) < _clickLong) {
                if ((now - _lastClick) < _dblClick) {
                    _lastClick = 0;
                    fireEvent(_onTouchDblClick, scr, EVENT::EVT_DBLCLICK);
                } else {
                    _lastClick = now;
                    fireEvent(_onTouchClick, scr, EVENT::EVT_CLICK);
                }
            }
        }
    }
}
//##################################################################

WIPE_DIRECTION ExtendedTouchEvent::wipeDirection(HAL_TouchscreenPoint & b, HAL_TouchscreenPoint & e) {
    HAL_TouchscreenPoint ptmp;
    int bx, by, ex, ey;

    ptmp = toScreen(b);
    bx = ptmp.x;
    by = ptmp.y;
    ptmp = toScreen(e);
    ex = ptmp.x;
    ey = ptmp.y;

    WIPE_DIRECTION dir; // wipe direction 0 = left, 1 = right, 2 = up, 3 = down
    if (abs(bx - ex) > abs(by - ey)) { // movement in x direction
        dir = ((bx - ex) > 0) ? WIPE_DIRECTION::WD_LEFT_RIGHT : WIPE_DIRECTION::WD_RIGHT_LEFT;
    } else { // movement in y direction
        dir = ((by - ey) > 0) ? WIPE_DIRECTION::WD_BOTTOM_UP : WIPE_DIRECTION::WD_TOP_DOWN;
    }
    return dir;
}
//##################################################################

void ExtendedTouchEvent::setResolution(int16_t xResolution, int16_t yResolution) {
    _xResolution = xResolution;
    _yResolution = yResolution;
}
//##################################################################

void ExtendedTouchEvent::setDrawMode(bool drawMode) {
    _drawMode = drawMode;
}
//##################################################################

void ExtendedTouchEvent::calibrate(uint16_t smallMin, uint16_t longMin, uint16_t smallMax, uint16_t longMax) {
    _tsSmallMin = smallMin;
    _tsSmallMax = smallMax;
    _tsLongMin = longMin;
    _tsLongMax = longMax;
}
//##################################################################

void ExtendedTouchEvent::setMoveTreshold(uint16_t threshold) {
    _moveth = threshold;
}
//##################################################################

void ExtendedTouchEvent::setWipe(uint16_t wipeX, uint16_t wipeY) {
    _wipeX = wipeX;
    _wipeY = wipeY;
}
//##################################################################

void ExtendedTouchEvent::setLongClick(uint16_t clickLong) {
    _clickLong = clickLong;
}
//##################################################################

void ExtendedTouchEvent::setDblClick(uint16_t dblclick) {
    _dblClick = dblclick;
}
//##################################################################

void ExtendedTouchEvent::registerOnTouchDown(TS_Point_callback *callback) {
    _onTouchDown = callback;
}
//##################################################################

void ExtendedTouchEvent::registerOnTouchUp(TS_Point_callback *callback) {
    _onTouchUp = callback;
}
//##################################################################

void ExtendedTouchEvent::registerOnTouchClick(TS_Point_callback *callback) {
    _onTouchClick = callback;
}
//##################################################################

void ExtendedTouchEvent::registerOnTouchDblClick(TS_Point_callback *callback) {
    _onTouchDblClick = callback;
}
//##################################################################

void ExtendedTouchEvent::registerOnTouchLong(TS_Point_callback *callback) {
    _onTouchLong = callback;
}
//##################################################################

void ExtendedTouchEvent::registerOnTouchDraw(TS_Point_callback *callback) {
    _onTouchDraw = callback;
}
//##################################################################

void ExtendedTouchEvent::registerOnTouchWipe(void (*callback)(WIPE_DIRECTION direction)) {
    _onTouchWipe = callback;
}
//##################################################################

void ExtendedTouchEvent::registerOnAllEvents(void (*callback)(HAL_TouchscreenPoint & p, EVENT event)) {
    _onAllEvents = callback;
}
//##################################################################

void ExtendedTouchEvent::fireEvent(TS_Point_callback *callback, HAL_TouchscreenPoint & scr, EVENT type) {
    if (callback) {
        callback(scr);
    }
    fireAllEvents(scr, type);
}
//##################################################################

void ExtendedTouchEvent::fireAllEvents(HAL_TouchscreenPoint & scr, EVENT type) {
    if (_onAllEvents) {
        _onAllEvents(scr, type);
    }
}
//##################################################################

HAL_TouchscreenPoint ExtendedTouchEvent::toScreen(HAL_TouchscreenPoint & p) {
    _lasttouch = _compat.ll_millis();

    int w = _tft.width();
    int h = _tft.height();
    DEBUG_ETE("######## to screen(x=%d, y=%d, z=%d , w=%d, h=%d)\n", p.x, p.y, p.z, w, h);
    DEBUG_ETE("######## to screen(_tsSmallMin=%d, _tsSmallMax=%d, _tsLongMin=%d, _tsLongMax=%d, _black=%d)\n",
          _tsSmallMin, _tsSmallMax, _tsLongMin, _tsLongMax, _black);

    HAL_TouchscreenPoint screen;

    if (_touch.coordinatesAreRaw())  {
        switch (_tft.getRotation()) {
            case ROTATION::PORTRAIT_TR:
                screen.x = _compat.ll_map(p.x, _tsSmallMin, _tsSmallMax, 0, w);
                screen.y = _compat.ll_map(p.y, _tsLongMin, _tsLongMax - _black, 0, h);
                break;
            case ROTATION::LANDSCAPE_BR:
                screen.x = _compat.ll_map(p.x, _tsLongMin, _tsLongMax - _black, 0, w);
                screen.y = _compat.ll_map(p.y, _tsSmallMin, _tsSmallMax, 0, h);
                break;
            case ROTATION::PORTRAIT_BL:
                screen.x = _compat.ll_map(p.x, _tsSmallMin, _tsSmallMax, 0, w);
                screen.y = _compat.ll_map(p.y, _tsLongMin + _black, _tsLongMax, 0, h);
                break;
            case ROTATION::LANDSCAPE_TL:
                screen.x = _compat.ll_map(p.x, _tsLongMin + _black, _tsLongMax, 0, w);
                screen.y = _compat.ll_map(p.y, _tsSmallMin, _tsSmallMax, 0, h);
                break;
        }
    } else {
        screen = p;
        screen.z = 0;
    }

    DEBUG_ETE("-> screen.x=%d, screen.y=%d #######\n", screen.x, screen.y);

    return screen;
}
