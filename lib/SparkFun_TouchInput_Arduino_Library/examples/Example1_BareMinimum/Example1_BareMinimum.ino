/*
SparkFun Touch Input Arduino Library
Example1 Bare Minimum

This example demonstrates the SFTI library using the following hardware:
- Qwiic Capacitive Touchscreen 7in (FT5316)   - https://www.sparkfun.com/products/15104
- Arduino-compatible microtonroller board with I2C (Arduino Uno, Redboard, Blackboard, Teensy, etc...)

Expected results:
You will be able to interact with a button and a draggable slider on the touchscreen with results printed to the serial monitor

If you have a hard time remembering where the handle or button is located then try turining on the debugger element with "USE_DEBUGGER 1" 
because it will print out the location of your touches to the screen


Hardware Hookup:
Connect the Qwiic touchscreen to the microcontroller with a Qwiic cable, or solder to the I2C pins if necessary


This code is open source. Please test it, tweak it, and learn from it or make it better!

Author: Owen Lyke
Date: Nov 21 2018

*/


// First include the necessary header files:
// SparkFun_TouchInput is the foundation
// SparkFun_TouchInput_Driver_FT5xx6 is a driver extension - meaning that in the future TouchInput can be used with more hardware. 
// You could even write your own driver (and it doesn't strictly need to really come from a touchscreen, so feel free to be creative!)
#include "SparkFun_TouchInput.h"                  // Click here to get the library: http://librarymanager/All#SparkFun_TouchInput
#include "SparkFun_TouchInput_Driver_FT5xx6.h"    // Click here to get the library: http://librarymanager/All#SparkFun_TouchInput_Driver_FT5xx6

#define SERIAL_PORT Serial        // Allows you to easily change the serial port to use (for example if you need to use SerialUSB for SAMD21 based boards)

// To get everything working we will need a few things:
// sfti_device myDevice             -- the sfti_device class represents the overall touch input system. It is requires a driver (to get touch readings) and elements (to respond to touch readings)
// sfti_driver_ft5xx6  myDriver     -- the sfti_driver_ft5xx6 is a driver extension that satisfies the interface requirements laid out in the TouchInput library
// sfti_element_button myButton     -- an example of one kind of "element" that responds to touch data. This can call a callback function when pressed or released and also report if it is currently pressed
// sfti_element_handle myHandle     -- another "element" example. This element can be picked up and dragged to a new location on screen. 

// The overall device can be declared on its own
sfti_device myDevice;

// The driver can also be declared alone but we will also need a specific FT5xx6 extension for the touchscreen hardware we're using
sfti_driver_ft5xx6  myDriver; // This contains the interface that TouchInput expects
FT5316 myCTP;                 // This is a fully-featured driver for the FT5316 that "myDriver" can use

// The button and handle elements use a "polygon" from the "SparkFun_2DRayTracing" library. The most basic way to make a polygon is to specify all the vertices and the number of sides:
sf2drt_coordinate_t buttonCoords[] = {{1,1}, {1, 200}, {200,200}, {200, 1}};              // Create coordinates for a polygon button
sf2drt_polygon buttonPolygon(4, buttonCoords);                                            // Create a polygn to represent the button

// It is also possible to construct polygons using fewer points, such as a 2-corner rectangle
sf2drt_rect_2corner handlePolygon({300, 100}, {500, 300});



// When instantiating a button or handle type you have the option of providing callback functions with a "void (*func)( void )" Signature corresponding to certain events
void buttonPressedCallback( void ){ SERIAL_PORT.println("Button pressed!"); }                 // 
void buttonReleasedCallback( void ){ SERIAL_PORT.println("Button released!"); }
sfti_element_button myButton(buttonPolygon, buttonPressedCallback, buttonReleasedCallback);   // Buttons can be pressed and released

void handlePickedUpCallback( void ){ SERIAL_PORT.println("Handle picked up!"); }
void handleSetDownCallback( void ){ SERIAL_PORT.println("Handle set down!"); }
//void handleMovedCallback( void ); // We won't use a callback for "moved" because it would be called so often
sfti_element_handle myHandle(handlePolygon, handlePickedUpCallback, handleSetDownCallback/*, handleMovedCallback*/ /* NULL if not specified */ );



// There is another kind of element that can be useful:
// The debugger responds to records by simply printing their information
// Note that it it really important that whatever you define as "SERIAL_PORT" has the same functions as a (HardwareSerial) class, otherwise
// some weird things might happen. If you're not sure then change "USE_DEBUGGER" to 0
#define USE_DEBUGGER 0
#if USE_DEBUGGER
sfti_element_debugger myDebugger((HardwareSerial*)&SERIAL_PORT);  
#endif



void setup() {
  SERIAL_PORT.begin(115200);
  SERIAL_PORT.println("Example1 - TouchInput BareMinimum");

  // So far we have created all the objects that we need but they don't know about each other
  // Now we can make all the necessary links

  myDriver.plldrv = &myCTP;           // This tells myDriver to use myCTP as the low-level interface
  myDevice.linkDriver(&myDriver);     // This tells myDevice to use myDriver to get touch records

  // These lines tell myDevice that it should send touch data to the elements
  myDevice.addElement(&myButton);
  myDevice.addElement(&myHandle);
  #if USE_DEBUGGER
  myDevice.addElement(&myDebugger);
  #endif

  myCTP.begin();  // Start the low-level touch panel driver

}

void loop() {
  myDevice.update(); // When using polling mode you use "update()" to see if the current touch data has changed, and if it has cause the elements to handle those touch records
}
