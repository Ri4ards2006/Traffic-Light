/**************************************************************
 * Use this sketch to test the Motion Detector eCog
 * 
 * eCogs Used:
 *       eCog117 - Motion Detector - Connect to eCog A
 * 
 * Libraries required (Install with Library Manager):
 *       GadgetBox
 *       
 * GadgetBox Motherboards supported:      
 *       Arduino Mini Pro
 *       Teensy 3.2
 *       
 **************************************************************/
#include <GadgetBox.h>

int motionPin = AC0;    

void setup() {
  Serial.begin(9600);
  pinMode(motionPin, INPUT);
}

void loop() {
  if (!digitalRead(motionPin))     //Motion is detected when a 0 is read from the motion pin. After motion is read it can take time before it detects again.
    Serial.println("Motion Detected: ");
  else
    Serial.println("No Motion or waiting for motion to reset.");
  delay(1000);
}
