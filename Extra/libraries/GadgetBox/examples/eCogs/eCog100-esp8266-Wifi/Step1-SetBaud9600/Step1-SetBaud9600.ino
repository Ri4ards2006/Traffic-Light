/**************************************************************
 * This sketch sets the esp8266 on eCog100 to 9600 baud for stable use with Arduino boards
 * 
 * eCogs Used:
 *       eCog100 - Wifi - Connect to eCog C
 * 
 * Libraries required (Install with Library Manager):
 *       GadgetBox
 *       
 * GadgetBox Motherboards supported:      
 *       Arduino Mini Pro (Software Serial on eCog C)
 *       Teensy 3.2 (Hardware Serial2 on eCog C)
 *       
 **************************************************************/
#include <GadgetBox.h>

#if defined(CORE_TEENSY)        //Use Hardware Serial for Teensy
  #define EspSerial Serial2
#elif defined(ARDUINO_AVR_PRO)  // or Software Serial on eCog C for Mini Pro
  #include <SoftwareSerial.h>
  SoftwareSerial EspSerial(CC0, CC1); // RX, TX (For Pro Mini)
#endif

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  EspSerial.begin(115200);
  delay(3000);
  //EspSerial.println("AT+CIOBAUD=9600,8,1,0,0");  //This is for older firmwares
  EspSerial.println("AT+UART_DEF=9600,8,1,0,0");
  
  delay(1000);  
  EspSerial.begin(9600);
  EspSerial.println("AT+GMR");  //print version
}

void loop() {
  // This allows you to send commands to the esp8266 with arduino Serial Monitor
  while(EspSerial.available()) Serial.write(EspSerial.read());
  while(Serial.available()) EspSerial.write(Serial.read());
}
