/* This example demonstrates how to use the ExtendedTouchEvent library, especially usage of the methods
 * setRotation(), isInRectangle() and lastTouchEventTime() in cooperation with the TFT_eSPI library as graphic lib.
 *
 * It also implements a simple screensaver which dims screen and sets processor into light sleep in case of longer inactivity.
 * 
 * Orientation of screen is changed (cycled through all 4 possibilities) if you touch the small green rectangle.
 *
 * If <b>USE_PWM</b> is defined, brightness of display will be controlled by y-coordinate of last click (the higher the brighter)
 * using PWM. Otherwise the screensaver simply switches on and off.
 *
 * It uses a 2.4" TFT display (with  a ILI9341 controller) with resistive touch screen (XPT2046).
 * The module is used together with an ESP32 but any other arduino compatible processor should work
 * For the used pins see definitions in the code (board_setup.h)
 *
 * If <b>USE_ILI9488</b> is defined a 3,5" TFT display (with an ILI9488 controller) is used instead.
 */
#include "Arduino.h"

#include <SPI.h>
#include <TFT_eSPI.h>
#include "ExtendedTouchEvent.h"
#include "HAL_TFT_eSPI_Adapter.h"              // adapter for graphics driver
#include "HAL_TFT_eSPI_TouchscreenAdapter.h"   // adapter for touchscreen driver
#include "esp_sleep.h"


// parameters for touchscreen
#define MINPRESSURE 500
#define MAXPRESSURE 2000
//following values might to be altered according to the hardware
#define TS_SMALLMIN 250
#define TS_LONGMIN 250
#define TS_SMALLMAX 3850
#define TS_LONGMAX 3900

//Instances of touchscreen and TFT
TFT_eSPI tft = TFT_eSPI();

//init TouchEvent with pointer to the touch screen driver
HAL_TFT_eSPI_Adapter tftadapter = HAL_TFT_eSPI_Adapter(tft);
HAL_TFT_eSPI_XPT2046_TS_Adapter touchadapter = HAL_TFT_eSPI_XPT2046_TS_Adapter(tft);
ExtendedTouchEvent tevent(touchadapter, tftadapter, 200);

// coordinates of last touch
int tsx, tsy;
// current touch state
bool tsdown = false;
// current screen orientation 0 = portrait, ..., 3 = landscape reverse
ROTATION rotation = ROTATION::PORTRAIT_TR;

// time of last touch (values see millis())
unsigned long lastknowntouch = 0;
// desired state of backlights
boolean light_on = true;

void drawCycle();
void draw_screen();

void handleTouchDown(HAL_TouchscreenPoint & p) {
	tsdown = true;
	Serial.printf("########### touch down x=%d, y=%,d z=%d #############\n", p.x, p.y, p.z);
	drawCycle();
}
//#####################################################

void handleTouchUp(HAL_TouchscreenPoint & p) {
	tsdown = false;
	Serial.printf("########### touch up x=%d, y=%d,d z=%d #############\n", p.x, p.y, p.z);
	drawCycle();
}
//#####################################################

void handleTouchEvent(HAL_TouchscreenPoint & p) {
	tsx = p.x; // save x und y as raw values
	tsy = p.y;
	delay(1);

	// get screen orientation
	ROTATION rot = tftadapter.getRotation();

	// check if touch is inside of green square

	int w = tftadapter.width();
	int h = tftadapter.height();

	boolean b;
	switch (rot) {
		// landscape
		case ROTATION::LANDSCAPE_BR:
		case ROTATION::LANDSCAPE_TL:
		{
			b = tevent.isInRectangle(p, w / 2 - 10, h / 2 - 20, 20, 40);
			break;
		}
		case ROTATION::PORTRAIT_BL:
		case ROTATION::PORTRAIT_TR: {
			b = tevent.isInRectangle(p, w / 2 - 20, h / 2 - 10, 40, 20);
			break;
		}
	}

	if (b) {
		Serial.printf("in green square w=%d, h=%d\n", tftadapter.width(), tftadapter.height());
		// change orientation
		switch (rotation) {
			// landscape
			case ROTATION::LANDSCAPE_BR: {
				rotation = ROTATION::PORTRAIT_BL;
				break;
			}
			case ROTATION::PORTRAIT_BL:{
				rotation = ROTATION::LANDSCAPE_TL;
				break;
			}
			case ROTATION::LANDSCAPE_TL:{
				rotation = ROTATION::PORTRAIT_TR;
				break;
			}
			case ROTATION::PORTRAIT_TR: {
				rotation = ROTATION::LANDSCAPE_BR;
				break;
			}
		}

		Serial.printf("rotation=%d\n", rotation);
		tevent.setRotation(rotation);
	}

	// redraw screen
	draw_screen();
}
//#####################################################

void setupPWM() {
    if (ledcSetup(0, 5000, 8)) {
        ledcAttachPin(TFT_BL, 0);
    } else {
        Serial.println("PWM setup failed");
    }
}
//#####################################################

// switch off display
void dim() {
    light_on = false;
    Serial.println("dim");
#ifdef USE_PWM
	setupPWM();
    ledcWrite(0, 256);
#else
    // other displays might use LOW for off
    digitalWrite(TFT_BL, HIGH);
#endif
}
//#####################################################

// switch on display
void enlighten() {
    light_on = true;
#ifdef USE_PWM
	setupPWM();
    ledcWrite(0, min(tsy, 255));
#else
    // other displays might use HIGH for on
    digitalWrite(TFT_BL, LOW);
#endif
}
//#####################################################

// time in ms, since last touch
unsigned long notTouchedSince() {
	unsigned long now = millis();
	if (lastknowntouch <= now) {
		return now - lastknowntouch;
	} else {
	    // millis has an overflow every 50 days ;-)
		return ULONG_MAX - lastknowntouch + now;
	}
}
//#####################################################

// switch off backgroundlights in case of longer inactivity, switch on 1st touch
void screensaver() {
	unsigned long lasttouch = tevent.lastTouchEventTime();

	if (lastknowntouch != lasttouch) {
		lastknowntouch = lasttouch;
#ifdef USE_PWM
        enlighten();
#else
        if (! light_on) {
            enlighten();
        }
#endif
	} else {
		if (notTouchedSince() > 20000) { // TODO time configurable
			if (light_on) {
				dim();
			}
		}
	}
}
//#####################################################

// draw the yellow / red circle
void drawCycle() {
	uint16_t w, h;
	w = tftadapter.width();
	h = tftadapter.height();
	if ((rotation == ROTATION::LANDSCAPE_BR) || (rotation == ROTATION::LANDSCAPE_TL)) {
		if (tsdown)
			tft.fillCircle(w/2, h/2 + 40, 10, TFT_RED);
		else
			tft.fillCircle(w/2, h/2 + 40, 10, TFT_YELLOW);
	} else {
		if (tsdown)
			tft.fillCircle(w/2, h/2 + 40, 10, TFT_RED);
		else
			tft.fillCircle(w/2, h/2 + 40, 10, TFT_YELLOW);
	}
}
//#####################################################

// display a number
void display_right(int x, int y, String val) {
	tft.setTextDatum(BR_DATUM);
	tft.drawString(val, x, y);
	tft.setTextDatum(TL_DATUM);
}
//#####################################################

// show position in landscape orientation
void drawPositionLandscape(uint16_t x, uint16_t y) {
	tft.setCursor(20, 60);
	tft.print("X = ");
	display_right(110, 60, String(x));
	tft.setCursor(180, 60);
	tft.print("Y = ");
	display_right(270, 60, String(y));

	drawCycle();
}
//#####################################################

// show position in portrait orientation
void drawPositionPortrait(uint16_t x, uint16_t y) {
	tft.setCursor(20, 60);
	tft.print("X = ");
	display_right(110, 60, String(x));
	tft.setCursor(20, 100);
	tft.print("Y = ");
	display_right(110, 100, String(y));

	drawCycle();
}
//#####################################################

#ifdef ILI9488
	#define HL "ILI9488"
#else
	#define HL "ILI9341"
#endif

// draw the complete screen
void draw_screen() {
	Serial.println("draw_screen");
	uint16_t w, h;
	w = tftadapter.width();
	h = tftadapter.height();

	// choose background colour and font
	tftadapter.setRotation(rotation);
	tft.fillScreen(TFT_DARKCYAN);

 	tft.setCursor(80, h - 20);
	tft.setTextFont(2);
	tft.setTextSize(1);
	tft.setTextColor(TFT_RED);
	tft.print(HL);


	tft.setTextColor(TFT_WHITE);
		tft.setFreeFont(& FreeSans9pt7b);
	// draw messured positions and  green square
	if ((rotation == ROTATION::LANDSCAPE_BR) || (rotation == ROTATION::LANDSCAPE_TL)) {
		drawPositionLandscape(tsx, tsy);
		tft.fillRect(w / 2 - 10, h / 2 - 20, 20, 40, TFT_GREEN);
	} else {
		drawPositionPortrait(tsx, tsy);
		tft.fillRect(w / 2 - 20, h / 2 - 10, 40, 20, TFT_GREEN);
	}

	// show rotations index in green square
	tft.setCursor(w / 2 - 5, h / 2 + 6);
	tft.setTextColor(TFT_BLACK, TFT_GREEN);
	tft.print(fromRotation(rotation));

	// draw arrows to the corners
	tft.drawLine(0, 0, 20, 0, TFT_WHITE);
	tft.drawLine(0, 0, 0, 20, TFT_WHITE);
	tft.drawLine(0, 0, 40, 40, TFT_WHITE);

	tft.drawLine(w - 1, 0, w - 20, 0, TFT_YELLOW);
	tft.drawLine(w - 1, 0, w - 1, 20, TFT_YELLOW);
	tft.drawLine(w - 1, 0, w - 40, 40, TFT_YELLOW);

	tft.drawLine(w - 1, h - 1, w - 40, h, TFT_CYAN);
	tft.drawLine(w - 1, h - 1, w, h - 40, TFT_CYAN);
	tft.drawLine(w - 1, h - 1, w - 40, h - 40, TFT_CYAN);

	tft.drawLine(0, h - 1, 20, h - 1, TFT_RED);
	tft.drawLine(0, h - 1, 0, h - 20, TFT_RED);
	tft.drawLine(0, h - 1, 40, h - 40, TFT_RED);
}
//#####################################################

// start light sleep if necessary
void sleep() {
	if (notTouchedSince() > 30000) {
	    Serial.println("go to sleep");
		esp_err_t ws = esp_light_sleep_start();
		if (ws != ESP_OK) {
		    Serial.printf("esp_light_sleep_start failed %d -> %s\n", ws, esp_err_to_name(ws));
		} else {
			esp_sleep_wakeup_cause_t wach = esp_sleep_get_wakeup_cause();

            switch (wach) {
                case ESP_SLEEP_WAKEUP_EXT0: {
                    Serial.println("touch wake (ext0)");
                    enlighten();
                    break;
                }
                case ESP_SLEEP_WAKEUP_GPIO: {
                    Serial.println("touch wake");
                    enlighten();
                    break;
                }
                case ESP_SLEEP_WAKEUP_TIMER : {
                    Serial.println("timer wake");
                    break;
                }
			}
		}
	}
}
//#####################################################

void setup() {
    Serial.begin(SPEED);
    while (!Serial);

	pinMode(TFT_BL, OUTPUT);

// Use this calibration code in setup()/setRotation
#ifdef USE_ILI9488
	uint16_t calData[4][5] = {
		{ 290, 3463, 295, 3626, 4 },
		{ 259, 3684, 252, 3513, 7 },
		{ 315, 3458, 267, 3646, 2 },
		{ 290, 3637, 293, 3479, 1 }
	};
#else
    uint16_t calData[4][5] = {
        {287, 3470, 205, 3416, 2},
        {167, 3453, 286, 3452, 1},
        {300, 3472, 202, 3396, 4},
        {211, 3374, 246, 3495, 7}
    };
#endif

    touchadapter.setCalibrationData(calData);
    touchadapter.setRotation(ROTATION::PORTRAIT_TR);
    tft.init();

#ifdef USE_PWM
    setupPWM();
#endif

	enlighten ();
	// start drivers
	tft.begin();
	touchadapter.begin();

    esp_err_t ws;

    ws = esp_sleep_enable_ext0_wakeup(TOUCH_IRQ, 0);
    if (ws != ESP_OK) {
        Serial.printf("esp_sleep_enable_ext0_wakeup failed %d -> %s\n", ws, esp_err_to_name(ws));
    }

    ws = esp_sleep_enable_gpio_wakeup();
	if (ws != ESP_OK) {
		Serial.printf("esp_sleep_enable_gpio_wakeup failed %d -> %s\n", ws, esp_err_to_name(ws));
	}

	ws = esp_sleep_enable_timer_wakeup(30000000L); // sleep 30 sec 
	if (ws != ESP_OK) {
		Serial.printf("esp_sleep_enable_timer_wakeup failed %d -> %s\n", ws, esp_err_to_name(ws));
	}

	// resolution of display
	Serial.printf("tft.w=%d,  tft.h =%d\n", tftadapter.width(), tftadapter.height());

	//init ExtendedTouchEvent instance
	tevent.setResolution(tftadapter.width(), tftadapter.height());
	tevent.setDblClick(300);
	tevent.registerOnTouchClick(handleTouchEvent);
	tevent.registerOnTouchDown(handleTouchDown);
	tevent.registerOnTouchUp(handleTouchUp);
	tevent.setDrawMode(false);

	tevent.calibrate(TS_SMALLMIN, TS_LONGMIN, TS_SMALLMAX, TS_LONGMAX);
	// tevent.calibrate(calData[0], calData[2], calData[1], calData[3]);

	tsx = 0;
	tsy = 0;
	tsdown = false;
	rotation = ROTATION::PORTRAIT_TR;
	// display screen
	tevent.setRotation(rotation);
	draw_screen();
}
//#####################################################

void loop() {
	//poll for touch events
	tevent.pollTouchScreen();
	screensaver();
	sleep();
}
