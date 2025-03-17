#include <Arduino.h>
#include "SerialManager.h"

byte sendData = 0;
byte receiveData = 0;
SerialManager<byte, byte> manager(Serial, receiveData, sendData, SECURE, NONE);

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial1.begin(115200);
	delay(500);
}

void loop() {

	if (receiveData == 1) {
		digitalWrite(LED_BUILTIN, HIGH);
	}
	else {
		digitalWrite(LED_BUILTIN, LOW);
	}
	manager.update();
}