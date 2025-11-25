#ifndef BOARD_SETUP
#define BOARD_SETUP

#ifdef USE_ILI9488
    #define ILI9488_DRIVER
#else
    #define ILI9341_DRIVER
    #define TFT_WIDTH 240
    #define TFT_HEIGHT 320
#endif

// #define SPI_FREQUENCY  20000000
#define SPI_FREQUENCY  40000000
// #define SPI_FREQUENCY  80000000

// SPI clock frequency for touch controller
#define SPI_TOUCH_FREQUENCY  2500000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  16000000

#define SUPPORT_TRANSACTIONS

#define TFT_CS   5 // Chip select control pin to TFT CS
#define TFT_DC   4 // Data Command control pin to TFT DC (may be labelled RS = Register Select)
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_RST  22  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
#define TFT_MISO 19
#define TFT_BL   17            // LED back-light control pin
#define TFT_BACKLIGHT_ON LOW  // Level to turn ON back-light (HIGH or LOW)

#define TOUCH_CS 16     // Chip select pin (T_CS) of touch screen

// used pins - adjust to the value used by your hardware
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define SMOOTH_FONT
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

// ?????
// #define TOUCH_IRQ 2     //touch screen interrupt
#endif

