/**************************************************************
 * Use this sketch to test analog thumb joystick
 * 
 * eCogs Used:
 *       eCog104 - Thumb Joystick - Connect to eCog A
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

//Use for eCog A
int horizontalPin = AC1;
int verticalPin = AC0;  

//Use for eCog D
//int horizontalPin = DC1;
//int verticalPin = DC0;    

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("Horizontal Axis: ");
  Serial.print(analogRead(horizontalPin));
  Serial.print("    Vertical Axis: ");
  Serial.println(analogRead(verticalPin));  
  delay(1000);
}
