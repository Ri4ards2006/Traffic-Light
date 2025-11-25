// This sketch includes a function to draw circle segments
// for pie charts in 1 degree increments

#include "board_setup.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "ExtendedTouchEvent.h"
#include "HAL_TFT_eSPI_Adapter.h"              // adapter for graphics driver
#include "HAL_TFT_eSPI_TouchscreenAdapter.h"   // adapter for touchscreen driver
#include <math.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
//init TouchEvent with pointer to the touch screen driver
HAL_TFT_eSPI_Adapter tftadapter = HAL_TFT_eSPI_Adapter(tft);
HAL_TFT_eSPI_XPT2046_TS_Adapter touchadapter = HAL_TFT_eSPI_XPT2046_TS_Adapter(tft);
ExtendedTouchEvent tevent(touchadapter, tftadapter, 200);

#define DEG2RAD 0.0174532925

byte inc = 0;
unsigned int col = 0;
int midw;
int midh;

// #########################################################################
// Draw circle segments
// #########################################################################

// x,y == coords of centre of circle
// r = radius
// start_angle = 0 - 359
// sub_angle   = 0 - 360 = subtended angle
// colour = 16-bit colour value
int fillSegment(int x, int y, int r, int start_angle, int sub_angle, unsigned int colour) {
    Serial.println("fillsegment");

    // Calculate first pair of coordinates for segment start
    float sx = cos((start_angle) * DEG2RAD);
    float sy = - sin((start_angle) * DEG2RAD);
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;

    // Draw colour blocks every inc degrees
    for (int i = start_angle; i < start_angle + sub_angle; i++) {
        // Calculate pair of coordinates for segment end
        int x2 = cos((i + 1) * DEG2RAD) * r + x;
        int y2 = - sin((i + 1) * DEG2RAD) * r + y;

        tft.fillTriangle(x1, y1, x2, y2, x, y, colour);

        // Copy segment end to segment start for next segment
        x1 = x2;
        y1 = y2;
    }
    Serial.println();

    return 0;
}
//###########################################################################

#define TFT_DARKBLUE    0x000F      /*   0,   0, 128 */
#define TFT_DARKRED     0x7800      /* 128,   0,   0 */
#define TFT_DARKYELLOW  0x7BE0      /* 128, 128,   0 */

static float piepercent[] = { 1.f/6, 1.f/12, 1.f/3, 150.f/360 };
static int highcolors[] = { TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW };
static int darkcolors[] = { TFT_DARKRED, TFT_DARKGREEN, TFT_DARKBLUE, TFT_DARKYELLOW };

void drawPieChart(int highlight = -1) {
    int start = 0;
    for (int i = 0; i < 4; i++) {
        int next = trunc(piepercent[i] * 360 + 0.5f);
        int end = start + next;
        int color = (highlight == i) ?  highcolors[i] : darkcolors[i];
        Serial.printf("start=%d, end=%d\n", start, end);
        fillSegment(midw, midh, 100, start, next, color);
        start = end;
    }
}
//###########################################################################

void drawScreen() {
    midw = tftadapter.width() / 2;
    midh = tftadapter.height()  / 2;

    tft.fillScreen(TFT_SKYBLUE);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK);

    tft.printf("rot=%d, w=%d, h=%d", tft.getRotation(), tftadapter.width(), tftadapter.height());

    drawPieChart();
}
//###########################################################################

void handleTouchClicked(HAL_TouchscreenPoint & p) {
    Serial.printf("########### touch clicked x=%d, y=%d z=%d #############\n", p.x, p.y, p.z);
    auto pie = tevent.isInPieSlice(p, midw, midh, 100, 4, piepercent);
    // Erase old chart with 360 degree black plot
    fillSegment(midw, midh, 100, 0, 360, TFT_BLACK);

    drawPieChart(pie);
}
//###########################################################################

void handleLongClicked(HAL_TouchscreenPoint & p) {
    Serial.printf("########### double clicked x=%d, y=%d z=%d #############\n", p.x, p.y, p.z);
    tevent.setRotation(toRotation((tft.getRotation() + 1) % 4));
    drawScreen();
}
//###########################################################################

void setup(void) {
    Serial.begin(SPEED);

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

    tft.begin();
    touchadapter.begin();

    tevent.registerOnTouchClick(handleTouchClicked);
    tevent.registerOnTouchLong(handleLongClicked);
    tevent.setRotation(ROTATION::PORTRAIT_BL);

    drawScreen();
}
//###########################################################################

void loop() {
    tevent.pollTouchScreen();
}
