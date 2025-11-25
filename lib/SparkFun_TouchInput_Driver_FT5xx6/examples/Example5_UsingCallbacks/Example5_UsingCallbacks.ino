/*
  Track your finger's locations on a capacitive touch panel (CTP)
  Finally check out a little bit of advanced debugging using two provided callbacks
  
  By: Owen Lyke
  SparkFun Electronics
  Date: October 30 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
  Example1_BasicReadings
  To connect the sensor to an Arduino:
  This library supports the sensor using the I2C protocol
  On Qwiic enabled boards simply connnect the sensor with a Qwiic cable and it is set to go
  On non-qwiic boards you will need to connect 4 wires between the sensor and the host board
  (Arduino pin) = (Breakout pin)
      SCL       =       SCL
      SDA       =       SDA
      GND       =       GND
      3.3V      =       3.3V
      INT_PIN   =       INT
*/

#include "SparkFun_TouchInput_Driver_FT5xx6.h"    // Click here to get the library: http://librarymanager/All#SparkFun_TouchInput_Driver_FT5xx6

#define SERIAL_PORT Serial

#define INT_PIN 2                       // Choose a pin number for your interrupt, and make sure it is interrupt-capable
#define WIRE_PORT Wire                  // To use interrupts you will also have to explicitly state which Wire port to use

#define BUFFER_LEN 10
FT5xx6_TouchRecord_TypeDef myTouchBuffer[BUFFER_LEN]; // Allocating a global variable for the buffer is suggested, in preference over dynamic allocation


FT5316 myCTP;                           // The FT5xx6 CTP controller object called 'myCTP'


// The two following callbacks are provided in the library as weak functions with blank implementations.
// If you re-define them (as shown below) your copies will be used instead to allow extra flexibility
// If you do not redefine them then hopefully the compiler will optimize out the blank functions

// This function is called when any FT5xx6 object's interrupt is triggered
// The 'me' parameter is the pointer to the object that called the callback
void FT5xx6_InterruptCallback(FT5xx6* me)
{
  SERIAL_PORT.print("Interrupt! from address: 0x");
  SERIAL_PORT.println((uint32_t)me, HEX);
  SERIAL_PORT.print(millis());
  SERIAL_PORT.println();
}

// This callback is called whenever a member function returns a status. You can use it to trace program execution
void  FT5xx6_ReturnCallback(FT5xx6_Status_TypeDef retval, const char * file, uint16_t line)
{
    SERIAL_PORT.print("Returned ");
    statusDecoder(retval);
    SERIAL_PORT.print(" at line ");
    SERIAL_PORT.print(line);
    SERIAL_PORT.print(" of file'");
    SERIAL_PORT.print(file);
    SERIAL_PORT.print("'");
    SERIAL_PORT.println();

}


void setup() {
  SERIAL_PORT.begin(115200);
  SERIAL_PORT.println("Example999 - Testing - FT5316 CTP Driver");
  SERIAL_PORT.println();

  SERIAL_PORT.print("Beginning the CTP. Status: ");
  statusDecoder( myCTP.begin(WIRE_PORT, INT_PIN, myCTPISR) );  // To use an interrupt we need 3 arguments: Wire port, interrupt pin number, and ISR handle name
  SERIAL_PORT.println("\n");

  SERIAL_PORT.print("Adding a buffer. Status: ");
  statusDecoder( myCTP.useBuffer(BUFFER_LEN, myTouchBuffer) );
  SERIAL_PORT.println("\n");
}

void loop() {
  // Interrupt code:
  FT5xx6_TouchRecord_TypeDef touch;
  if(myCTP.newData)                           // The additional conditional keeps execution time a lot lower (because the .update() method uses a lot of I2C and computation time)
  {
    myCTP.update();
    SERIAL_PORT.println(myCTP.available());
                                              // Now instead of reading the touch inputs right away we will wait for a certain number to accumulate without being read
  }

  if(myCTP.available() > 7)                   // When there are eight or more records we will print them all at once.
  {
    while(myCTP.available())
    {
      printTouchRecord(myCTP.read());
    }
  }
}

void printTouchRecord(FT5xx6_TouchRecord_TypeDef precord)
{
  SERIAL_PORT.print("Touch record info: ");

  SERIAL_PORT.print("Timestamp: "); 
  SERIAL_PORT.print(precord.timestamp);
  SERIAL_PORT.print(" : ");
  
  SERIAL_PORT.print("Number of touches: "); 
  SERIAL_PORT.print(precord.numTouches);
  SERIAL_PORT.print(" : ");
  
  if(precord.numTouches > 0)
  {
    SERIAL_PORT.print("Touch "); 
    SERIAL_PORT.print(1); 
    SERIAL_PORT.print(", x: ");
    SERIAL_PORT.print(precord.t1x);
    SERIAL_PORT.print(", y: ");
    SERIAL_PORT.print(precord.t1y);
    SERIAL_PORT.print(" : ");
  }
  if(precord.numTouches > 1)
  {
    SERIAL_PORT.print("Touch "); 
    SERIAL_PORT.print(2); 
    SERIAL_PORT.print(", x: ");
    SERIAL_PORT.print(precord.t2x);
    SERIAL_PORT.print(", y: ");
    SERIAL_PORT.print(precord.t2y);
    SERIAL_PORT.print(" : ");
  }
  if(precord.numTouches > 2)
  {
    SERIAL_PORT.print("Touch "); 
    SERIAL_PORT.print(3); 
    SERIAL_PORT.print(", x: ");
    SERIAL_PORT.print(precord.t3x);
    SERIAL_PORT.print(", y: ");
    SERIAL_PORT.print(precord.t3y);
    SERIAL_PORT.print(" : ");
  }
  if(precord.numTouches > 3)
  {
    SERIAL_PORT.print("Touch "); 
    SERIAL_PORT.print(4); 
    SERIAL_PORT.print(", x: ");
    SERIAL_PORT.print(precord.t4x);
    SERIAL_PORT.print(", y: ");
    SERIAL_PORT.print(precord.t4y);
    SERIAL_PORT.print(" : ");
  }
  if(precord.numTouches > 4)
  {
    SERIAL_PORT.print("Touch "); 
    SERIAL_PORT.print(5); 
    SERIAL_PORT.print(", x: ");
    SERIAL_PORT.print(precord.t5x);
    SERIAL_PORT.print(", y: ");
    SERIAL_PORT.print(precord.t5y);
    SERIAL_PORT.print(" : ");
  }
  SERIAL_PORT.println();
  
}

void statusDecoder( FT5xx6_Status_TypeDef status)
{
  switch(status)
  {
    case FT5xx6_Status_Nominal :            SERIAL_PORT.print("Nominal");             break;
    case FT5xx6_Status_Error :              SERIAL_PORT.print("Error");               break;
    case FT5xx6_Status_Not_Enough_Core :    SERIAL_PORT.print("Not Enough Core");     break;
    default :                               SERIAL_PORT.print("Unknown status");      break;
  }
}

// This function is the Interrupt Service Routine (ISR) for 'myCTP' specifically. 
// If oyu had more than one FT5xx6 object using interrupts Arduino would not be 
// able to guess which one to handle for an ISR. That's why we create a unique
// function for each one you are concerned with.
void myCTPISR( void )
{
  myCTP.interrupt();  
}
