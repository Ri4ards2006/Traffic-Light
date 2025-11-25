#include "SparkFun_FT5xx6.h"

#define INT_PIN 2
#define WIRE_PORT Wire

FT5316 myCTP;   // The FT object 

#define USE_BUFFER true
#define BUFFER_LEN 10


void myCTPISR( void )
{
  myCTP.interrupt();  // The sketch file needs to specify a unique function for each object that will be used with interrupts
}


void setup() {
  Serial.begin(115200);
  Serial.println("Example999 - Testing - FT5316 CTP Driver");
  Serial.println();

  // Let's check out the members:
  Serial.println("Just after declaration");
  printClassInfo(myCTP);
  Serial.println();
  Serial.println();


//  // Now let's use 'begin()'
//  myCTP.begin();
//  Serial.println("After 'begin()'");
//  printClassInfo(myCTP);
//  Serial.println();
//  Serial.println();

  

  Serial.print("Trying to enable a buffer! Status: ");
  statusDecoder(myCTP.useBuffer(BUFFER_LEN)); // Calling with only buffer length will attempt to dynamically allocate the buffer - beware lack of memory!
//  statusDecoder(myCTP.useBuffer(BUFFER_LENGTH, myTouchBuffer)); // Calling with providd buffer space uses compile-time allocation which is better ;)
  Serial.println();
  Serial.println("\n");

  // Now let's use 'begin()' with an argument for the interrupt pin and ISR function - this will allow us not to poll
  myCTP.begin(WIRE_PORT, INT_PIN, myCTPISR);
  Serial.println("After 'begin()'");
  printClassInfo(myCTP);
  Serial.println();
  Serial.println();



//  // Now try to remove the buffer (you know, to save memory!)
//  Serial.print("Removing the buffer? Status: ");
//  statusDecoder(myCTP.removeBuffer());
//  Serial.println();
//
//  Serial.println("After 'removeBuffer()'");
//  printClassInfo(myCTP);
//  Serial.println();
//  Serial.println();

  

  // Print out the touch record before anything happens
  Serial.println("Here is the initial touch record");
  printTouchRecord(&myCTP.lastTouch);
  
}

void loop() {
  // put your main code here, to run repeatedly:




// // Polling code:
//  FT5xx6_TouchRecord_TypeDef touch;
//  myCTP.update();
//  if(myCTP.newTouch)
//  {
//    touch = myCTP.read();
//    printTouchRecord(&touch);
//  }



// Interrupt code:
FT5xx6_TouchRecord_TypeDef touch;
if(myCTP.newData)                   // The additional conditional keeps execution time a lot lower (because the .update() method uses a lot of I2C and computation time)
{
  myCTP.update();
  if(myCTP.newTouch)
  {
//    touch = myCTP.read();
//    printTouchRecord(&touch);
  }
//  if(myCTP.available() > 7)
//  {
//    myCTP.clearBuffer();
//  }
  Serial.println(myCTP.available());
}

if(myCTP.available() > 7)
{
  while(myCTP.available())
  {
    touch = myCTP.read();
    printTouchRecord(&touch);
  }
}




//The next things to test: 
//buffering


}

void FT5xx6_InterruptCallback(FT5xx6* me)
{
//  Serial.print("Interrupt! from address: 0x");
//  Serial.println((uint32_t)me, HEX);
//  Serial.print(millis());
//  Serial.println();
}

void  FT5xx6_ReturnCallback(FT5xx6_Status_TypeDef retval, char * file, uint16_t line)
{
    if(retval != FT5xx6_Status_Nominal)
    {
        Serial.print("Returned ");
        statusDecoder(retval);
        Serial.print(" at line ");
        Serial.print(line);
        Serial.print(" of file'");
        Serial.print(file);
        Serial.print("'");
        Serial.println();
    }
}

void statusDecoder( FT5xx6_Status_TypeDef status)
{
  switch(status)
  {
    case FT5xx6_Status_Nominal : Serial.print("Nominal"); break;
    case FT5xx6_Status_Error : Serial.print("Error"); break;
    case FT5xx6_Status_Not_Enough_Core : Serial.print("Not Enough Core"); break;
    default : Serial.print("Unknown status"); break;
  }
}

void printTouchRecord(FT5xx6_TouchRecord_TypeDef * precord)
{
  Serial.print("Touch record info: ");
  
  Serial.print("Number of touches: "); 
  Serial.print(precord->numTouches);
  Serial.print(" : ");
  
  if(precord->numTouches > 0)
  {
    Serial.print("Touch "); 
    Serial.print(1); 
    Serial.print(", x: ");
    Serial.print(precord->t1x);
    Serial.print(", y: ");
    Serial.print(precord->t1y);
    Serial.print(" : ");
  }
  if(precord->numTouches > 1)
  {
    Serial.print("Touch "); 
    Serial.print(2); 
    Serial.print(", x: ");
    Serial.print(precord->t2x);
    Serial.print(", y: ");
    Serial.print(precord->t2y);
    Serial.print(" : ");
  }
  if(precord->numTouches > 2)
  {
    Serial.print("Touch "); 
    Serial.print(3); 
    Serial.print(", x: ");
    Serial.print(precord->t3x);
    Serial.print(", y: ");
    Serial.print(precord->t3y);
    Serial.print(" : ");
  }
  if(precord->numTouches > 3)
  {
    Serial.print("Touch "); 
    Serial.print(4); 
    Serial.print(", x: ");
    Serial.print(precord->t4x);
    Serial.print(", y: ");
    Serial.print(precord->t4y);
    Serial.print(" : ");
  }
  if(precord->numTouches > 4)
  {
    Serial.print("Touch "); 
    Serial.print(5); 
    Serial.print(", x: ");
    Serial.print(precord->t5x);
    Serial.print(", y: ");
    Serial.print(precord->t5y);
    Serial.print(" : ");
  }
  Serial.println();
  
}

void printClassInfo(FT5xx6 &FT)
{
    Serial.print("Address of the FT object: 0x"); Serial.println((uint32_t)&FT, HEX);
  Serial.print("_has interrupts: "); Serial.println(FT._hasInterrupts);
  Serial.print("_mode: 0x"); Serial.println(FT._mode, HEX);
  Serial.print("Address of wire pointer: 0x"); Serial.println((uint32_t)&FT._wire, HEX);
  Serial.print("Value of _wire pointer: 0x"); Serial.println((uint32_t)FT._wire, HEX);
  Serial.print("I2C address: 0x"); Serial.println(FT._addr, HEX);
  Serial.print("Interrupt pin: "); Serial.println(FT._int);
  
  Serial.print("Address of buffer head pointer: 0x"); Serial.println((uint32_t)&FT._ptouchRecordHead, HEX);
  Serial.print("Value of buffer head pointer: 0x"); Serial.println((uint32_t)FT._ptouchRecordHead, HEX);
  Serial.print("_writeOffset: "); Serial.println(FT._writeOffset);
  Serial.print("_readOffset: "); Serial.println(FT._readOffset);
  Serial.print("_recordDepth: "); Serial.println(FT._recordDepth);
  Serial.print("_recordsAvailable: "); Serial.println(FT._recordsAvailable);
  Serial.print("_writeOK: "); Serial.println(FT._writeOK);
  Serial.print("_readOK: "); Serial.println(FT._readOK);
  Serial.print("_bufferWasAllocated: "); Serial.println(FT._bufferWasAllocated);
  Serial.print("newTouch: "); Serial.println(FT.newTouch);
  Serial.print("Time of last update (ms)"); Serial.println(FT.lastUpdate);
  Serial.print("Address where lastTouch is stored: 0x"); Serial.println((uint32_t)&FT.lastTouch, HEX);
  
//  Serial.println((uint32_t)FT.lastTouch);
}

