#include <Arduino.h>
#include "SerialManager.h"
#include "VentiData.h"

//#define DEBUG
//#define MASTER

Data sendData = Data();
Data receiveData = Data();

#if defined(__AVR_ATmega328P__)
  HardwareSerial& hardwareSerial = Serial;
  //Serial_& usbSerial = Serial; 
#else
  HardwareSerial& hardwareSerial = Serial1;
  Serial_& usbSerial = Serial; 
#endif

#ifdef MASTER
  SerialManager<Data, Data> manager(hardwareSerial, receiveData, sendData, NONE, SECURE);
#else
  SerialManager<Data, Data> manager(hardwareSerial, receiveData, sendData, SECURE, NONE);
#endif

void setup() {
	Serial.begin(9600);
  #ifdef MASTER
	  pinMode(10, INPUT);
  #endif
	pinMode(LED_BUILTIN, OUTPUT);
	hardwareSerial.begin(9600);
	delay(500);
}

void loop() {
  #ifdef MASTER
	  if (HIGH == digitalRead(10)) {
	  	digitalWrite(LED_BUILTIN, HIGH);
	  	sendData.bpm  = 1;
      	sendData.dvol = 41;
	  	sendData.duty  = 42;
	  }
	  else {
	  	digitalWrite(LED_BUILTIN, LOW);
	  	sendData.bpm  = 0;
      	sendData.dvol = 0;
      	sendData.duty  = 0;
	  }
	  usbSerial.println(sendData.bpm);
	  manager.send();
	  delay(500);
  #else
    if (receiveData.bpm == 1) {
		digitalWrite(LED_BUILTIN, LOW);
	}
	else {
		digitalWrite(LED_BUILTIN, HIGH);
	}
	receiveData.dt_fall = 255;
	usbSerial.println(manager.receive());
	#ifdef DEBUG
    	usbSerial.println("#######################");
    	receiveData.print(usbSerial);
	#endif
  #endif
}