#include "SparkFun_TouchInput.h"                  // This file includes the majority of what you need
#include "SparkFun_TouchInput_Driver_FT5xx6.h"    // This file includes a specific TouchDriver (sfti_driver class) to use with the FT5xx6 hardware

#define SERIAL_PORT Serial        // Allows you to easily change the serial port to use 

// Settings for the hardware driver
#define INT_PIN     2               // Choose a pin number for your interrupt, and make sure it is interrupt-capable
#define WIRE_PORT   Wire            // To use interrupts you will also have to explicitly state which Wire port to use
sfti_driver_ft5xx6  myDriver;       //
FT5316 myCTP;                       // The driver will be based on the specific FT5316 chip
void myCTPISR( void )               // Set up interrupt capability to identify when new data is available from the hardware
{
  myCTP.interrupt();  
//   myDevice.update(); // This line squeaks by on the Teensy 3.6 but doesn't fare so well on the Arduino Uno
}


// Create an object of the sfti_device class to represent the touch panel you want to work with
sfti_device myDevice;



// Create the elements that you want to interact with on the touch panel


sf2drt_coordinate_t buttonCoords[] = {{10,10}, {10, 110}, {110,110}, {110, 10}};              // Create coordinates for a polygon button
sf2drt_polygon buttonPolygon(4, buttonCoords);                                                // Create a polygn to represent the button
void buttonPressedCallback( void ){ SERIAL_PORT.println("Button pressed!"); }                 // 
void buttonReleasedCallback( void ){ SERIAL_PORT.println("Button released!"); }
sfti_element_button myButton(buttonPolygon, buttonPressedCallback, buttonReleasedCallback);   // Buttons can be pressed and released


sf2drt_coordinate_t handleCoords[] = {{350,150}, {350, 250}, {450,250}, {450, 150}};          // Create coordinates for a polygon button
sf2drt_polygon handlePolygon(4, handleCoords);      
void handlePickedUpCallback( void ){ SERIAL_PORT.println("Handle picked up!"); }
void handleSetDownCallback( void ){ SERIAL_PORT.println("Handle set down!"); }
void handleMovedCallback( void )
{ 
  extern sfti_element_handle myHandle;
  SERIAL_PORT.print("Handle moved! {"); 
  SERIAL_PORT.print(myHandle.com.x);
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print(myHandle.com.y);
  SERIAL_PORT.print("}");
  SERIAL_PORT.println();
}
sfti_element_handle myHandle(handlePolygon, handlePickedUpCallback, handleSetDownCallback, handleMovedCallback); // Handles can be picked up and dragged to a new location



sfti_element_slider mySlider;                                               // Sliders are handles that are contrained to a particular path and report their progress along that path
sfti_element_debugger myDebugger((HardwareSerial*)&SERIAL_PORT);            // The debugger responds to records by simply printing their information
                                                                            // The casting here could have severe consequences if not done correctly (and with caution) - If you're wondering why it is here, that's to make the Teensy3.6's usbserial class object work here
                                                                            
void setup() {
  // put your setup code here, to run once:
  SERIAL_PORT.begin(115200);
  while(!SERIAL_PORT){}

  SERIAL_PORT.println("Example999 - TouchInput Testing");

  myCTP.begin(WIRE_PORT, INT_PIN, myCTPISR);


  SERIAL_PORT.print("Address of myButton: 0x"); 
  SERIAL_PORT.print((uint32_t)&myButton, HEX);
  
  SERIAL_PORT.print(", ");
  SERIAL_PORT.print("Address of mySlider: 0x");
  SERIAL_PORT.print((uint32_t)&mySlider, HEX);
  SERIAL_PORT.println();

  // This is a peculiarity of the wrapper nature of the driver we are using. This step links the non-sfti driver (myCTP) with the sfti driver called myDriver.
  myDriver.plldrv = &myCTP;

  // Set up the device to use the driver you provided
  myDevice.linkDriver(&myDriver);

  // Add the elements you created to the device
//  myDevice.addElement(&myDebugger); 
  myDevice.addElement(&myButton);
  myDevice.addElement(&myHandle);
//  myDevice.addElement(&mySlider); // So far slider is not implemented, but it will effectively be a handle constrained to a path that reports its progress along that path
   
}

void loop() {
  // put your main code here, to run repeatedly:
  myDevice.update();
  
}
