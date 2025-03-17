/*
SparkFun Touch Input Arduino Library
Example99 DemoCode

NOTE: This example is specific to the Teensy and will probably not compile on other platforms

This example demonstrates the SFTI library using the following hardware:
Qwiic Capacitive Touchscreen 7in (FT5316)   - https://www.sparkfun.com/products/15104
SmartLED Shield V4 for Teensy               - https://www.sparkfun.com/products/15046
RGB Panel 32 x 64                           - https://www.sparkfun.com/products/14718
A Teensy 3.2, 3.6, or 3.5 microcontroller

Expected results:
You will be able to interact with the RGB panel using the touchscreen in three ways:
1. Any finger on the touchscreen will be highlighted by a small circle, color in chronological order
2. You can change the blue square in the upper left corner to red by clicking on it
3. You can drag around the large teal circle

Hardware Hookup:
1. Follow the SmartLED Shield V4 for Teensy hookup guide
2. Connect the Qwiic Capacitive Touchscreen to the Teensy with a Qwiic Breadboard Cable (https://www.sparkfun.com/products/14425)
3. Connect the interrupt pin on the touchscreen to pin 0 on the Teensy


This code is open source. Please test it, tweak it, and learn from it or make it better!

Author: Owen Lyke
Date: Nov 20 2018

*/






#include "SparkFun_TouchInput.h"                  // Click here to get the library: http://librarymanager/All#SparkFun_TouchInput
#include "SparkFun_TouchInput_Driver_FT5xx6.h"    // Click here to get the library: http://librarymanager/All#SparkFun_TouchInput_Driver_FT5xx6


#define SERIAL_PORT Serial        // Allows you to easily change the serial port to use 



////////////////////////////////////////////////////////
// Touch Input Items
///////////////////////////////////////////////////////

// Settings for the hardware driver
#define INT_PIN     0               // Choose a pin number for your interrupt, and make sure it is interrupt-capable
#define WIRE_PORT   Wire            // To use interrupts you will also have to explicitly state which Wire port to use
sfti_driver_ft5xx6  myDriver;       //
FT5316 myCTP;                       // The driver will be based on the specific FT5316 chip
void myCTPISR( void )               // Set up interrupt capability to identify when new data is available from the hardware
{
  myCTP.interrupt();  
//   myDevice.update(); // This line squeaks by on the Teensy 3.6 but doesn't fare so well on the Arduino Uno
}
const double capXpix = 800.0;
const double capYpix = 460.0;


// Create an object of the sfti_device class to represent the touch panel you want to work with
sfti_device myDevice;



// Create the elements that you want to interact with on the touch panel


sf2drt_coordinate_t buttonCoords[] = {{1,1}, {1, 200}, {200,200}, {200, 1}};              // Create coordinates for a polygon button
sf2drt_polygon buttonPolygon(4, buttonCoords);                                                // Create a polygn to represent the button
void buttonPressedCallback( void ){ SERIAL_PORT.println("Button pressed!"); }                 // 
void buttonReleasedCallback( void ){ SERIAL_PORT.println("Button released!"); }
sfti_element_button myButton(buttonPolygon, buttonPressedCallback, buttonReleasedCallback);   // Buttons can be pressed and released


sf2drt_coordinate_t handleCoords[] = {{300,100}, {300, 300}, {500,300}, {500, 100}};          // Create coordinates for a polygon button
sf2drt_polygon handlePolygon(4, handleCoords);      
void handlePickedUpCallback( void ){ SERIAL_PORT.println("Handle picked up!"); }
void handleSetDownCallback( void ){ SERIAL_PORT.println("Handle set down!"); }
void handleMovedCallback( void )
{ 
  extern sfti_element_handle myHandle;
//  SERIAL_PORT.print("Handle moved! {"); 
//  SERIAL_PORT.print(myHandle.com.x);
//  SERIAL_PORT.print(", ");
//  SERIAL_PORT.print(myHandle.com.y);
//  SERIAL_PORT.print("}");
//  SERIAL_PORT.println();
}
sfti_element_handle myHandle(handlePolygon, handlePickedUpCallback, handleSetDownCallback, handleMovedCallback); // Handles can be picked up and dragged to a new location



sfti_element_slider mySlider;                                               // Sliders are handles that are contrained to a particular path and report their progress along that path
sfti_element_debugger myDebugger((HardwareSerial*)&SERIAL_PORT);            // The debugger responds to records by simply printing their information
                                                                            // The casting here could have severe consequences if not done correctly (and with caution) - If you're wondering why it is here, that's to make the Teensy3.6's usbserial class object work here
   


////////////////////////////////////////////////////////
// RGB Panel Items
///////////////////////////////////////////////////////

// Get the SmartLEDShield library from GitHub Here: https://github.com/pixelmatix/SmartMatrix

#include <SmartLEDShieldV4.h>  // uncomment this line for SmartLED Shield V4 (needs to be before #include <SmartMatrix3.h>)
#include <SmartMatrix3.h>
#include "colorwheel.c"
#include "gimpbitmap.h"

#define COLOR_DEPTH 24                  // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint8_t kMatrixWidth = 64;        // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 32;       // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 36;       // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN;   // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);      // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

const int defaultBrightness = (100*255)/100;    // full (100%) brightness
//const int defaultBrightness = (15*255)/100;    // dim: 15% brightness
const int defaultScrollOffset = 6;
const rgb24 defaultBackgroundColor = {0x40, 0, 0};

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;









                                                                         
void setup() {
  
  SERIAL_PORT.begin(115200);
  SERIAL_PORT.println("Example99 - TouchInput RGB Matrix Demo Code");

  // Setup for the touch panel
  // This is a peculiarity of the wrapper nature of the driver we are using. This step links the non-sfti driver (myCTP) with the sfti driver called myDriver.
  myDriver.plldrv = &myCTP;

  // Set up the device to use the driver you provided
  myDevice.linkDriver(&myDriver);

  // Add the elements you created to the touchscreen device
  myDevice.addElement(&myButton);
  myDevice.addElement(&myHandle);

  // Start the touch panel driver
  myCTP.begin(WIRE_PORT, INT_PIN, myCTPISR);


  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);

  matrix.addLayer(&backgroundLayer); 
  matrix.addLayer(&scrollingLayer); 
  matrix.addLayer(&indexedLayer); 
  matrix.begin();

  matrix.setBrightness(defaultBrightness);
  scrollingLayer.setOffsetFromTop(defaultScrollOffset);
  backgroundLayer.enableColorCorrection(true);
}






void loop() {
  myDevice.update();  // Update the touchscreen

  backgroundLayer.fillScreen({0, 0, 0});
  backgroundLayer.swapBuffers();

  // Map from touchscreen coordinates to RGB panel coordinates
  double scaleX = 39.0/capXpix;
  double scaleY = 22.0/capYpix;
  
  int x0, y0;
  
  rgb24 color, color2, color3, color4;

  // Define colors to use
  color.green = 200;
  color.blue = 200;
  color.red = 100;

  color2.red = 255;
  color2.blue = 0;
  color2.green = 0;
  
  color3.red = 0;
  color3.blue = 255;
  color3.green = 0;
  
  // Set the location of the handle
  x0 = round(myHandle.com.x*scaleX);
  y0 = round(myHandle.com.y*scaleY);

  // Draw circles at each touch point with a different color
  for(sfti_mxtouch_t indi = 0; indi < myDevice.record.numTouches; indi++)
  {
    sfti_coord_t coord = *(myDevice.record.pcoords +indi);
    
    int x1, y1;
    
    x1 = round(coord.x*scaleX);
    y1 = round(coord.y*scaleY);
    
    switch(indi)
    {
      case 0 : 
        color4.red = 255;
        color4.green = 0;
        color4.blue = 0;
        break;
      case 1 :
        color4.red = 255;
        color4.green = 255;
        color4.blue = 0;
        break;
      case 2 :
        color4.red = 0;
        color4.green = 255;
        color4.blue = 0;
        break;
      case 3 :
        color4.red = 255;
        color4.green = 0;
        color4.blue = 255;
        break;
      case 4 :
        color4.red = 255;
        color4.green = 255;
        color4.blue = 255;
        break;
    }
    
    backgroundLayer.drawCircle(x1, y1, 2, color4);
    backgroundLayer.drawCircle(x1, y1, 1, color4);
    
  }
  
  
  
  // Draw the square
  if(myButton.isPressed)
  {
  backgroundLayer.drawRectangle(0, 0, 8, 8, color2);
  backgroundLayer.drawRectangle(1, 1, 7, 7, color2);
  backgroundLayer.drawRectangle(2, 2, 6, 6, color2);
  backgroundLayer.drawRectangle(3, 3, 5, 5, color2);
  }
  else
  {
  backgroundLayer.drawRectangle(0, 0, 8, 8, color3);
  backgroundLayer.drawRectangle(1, 1, 7, 7, color3);
  backgroundLayer.drawRectangle(2, 2, 6, 6, color3);
  backgroundLayer.drawRectangle(3, 3, 5, 5, color3);
  }
  
  
  
  // Draw the handle circle
  backgroundLayer.drawCircle(x0, y0, 3, color);
  backgroundLayer.drawCircle(x0, y0, 2, color);
  backgroundLayer.drawCircle(x0, y0, 1, color);
  
  
  // Show the new screen
  backgroundLayer.swapBuffers();

}
