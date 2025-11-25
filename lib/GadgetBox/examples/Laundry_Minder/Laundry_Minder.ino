/*******************************************************************
 * This is a GadgetBox example that uses a vibration sensor to alert you when laundry is done
 * 
 * eCogs Used:
 *      eCog103 - Sensor - Connect a vibration sensor - Connect to eCog A on motherboard
 *      eCog100 - Wifi - Connect to eCog C on motherboard
 *      (Optional) eCog101 - I2C OLED TFT - Any open eCog location
 *      (Optional) eCog103 - Sensor - Audio Sensor - Connect to eCog D on motherboard
 *      
 * Libraries required (Install with Library Manager):
 *      GadgetBox
 *      Blynk
 *      
 * GadgetBox Motherboards supported:
 *      Arduino Mini Pro
 *      
 * Blynk App:
 *      Clone the following project in Blynk http://tinyurl.com/joze5j4
 *      
 * Blynk Virtual Pins:
 *      V0:   A count of the total amount of vibrations detected
 *      V1:   An LED indicating when laundry is done
 *      V2:   How many Vibrations/minute
 *      V3:   Reset all counts
 *      V4:   The sound level
 * 
 *******************************************************************       
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 *******************************************************************/

#include <GadgetBox.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "yourauth";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "ssid";
char pass[] = "password";

#if defined(CORE_TEENSY)        //Use Hardware Serial for Teensy
  #define EspSerial Serial1  
#elif defined(ARDUINO_AVR_PRO)  // or Software Serial on eCog C for Mini Pro
  #include <SoftwareSerial.h>
  SoftwareSerial EspSerial(CC0, CC1); // RX, TX (For Pro Mini)
#endif

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

#define OLED_RESET 99
Adafruit_SSD1306 display(OLED_RESET);

const int  vibrationPin = CC0;    // the pin that the vibration sensor is attached to
const int  audioAnalogPin = DC0;    // the pin that the vibration sensor is attached to
//const int ledPin = 15;       // the pin that the LED is attached to

// Variables will change:
int vibrationCounter = 0;   // counter for the number of button presses
int vibrationState = 0;         // current state of the button
int lastvibrationState = 0;     // previous state of the button
int vibrationsPerMinute = 0;
int notificationSent = 0;

void setup() {
  // initialize the button pin as a input:
  pinMode(vibrationPin, INPUT);
  pinMode(audioAnalogPin, INPUT);

  // initialize serial communication:
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(1,0);
  display.clearDisplay();
  display.print("Waiting for Wifi.");  
  display.display();

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

}

void loop() {
  Blynk.run();
  
  // read the pushbutton input pin:
  vibrationState = digitalRead(vibrationPin);

  // compare the vibrationState to its previous state
  if (vibrationState != lastvibrationState) {
    // if the state has changed, increment the counter
    if (vibrationState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      vibrationCounter++;
      vibrationsPerMinute++;
      //Serial.println("on");
      Serial.print("number of vibrations:  ");
      Serial.println(vibrationCounter);
      display.setCursor(1,0);
      display.clearDisplay();
      display.print("Detected: ");
      display.print(vibrationCounter);
      display.display();
      //Blynk.virtualWrite(V1,vibrationCounter);
    } 
    else {
      // if the current state is LOW then the button
      // went from on to off:
      //Serial.println("off"); 
    }
  }
  // save the current state as the last state, 
  //for next time through the loop
  lastvibrationState = vibrationState;
  
}

BLYNK_READ(V0) {
  Blynk.virtualWrite(V0,vibrationCounter);
}

BLYNK_READ(V2) {
  Serial.println("In vibrations per minute check");
  if (!vibrationsPerMinute && vibrationCounter > 0) { // No Vibrations in the last minute send alert
    Blynk.virtualWrite(V1,255);
    if (!notificationSent) {
      notificationSent = 1;
      Blynk.notify("Laundry has been quiet for 1 minute");
      Serial.println("Laundry is quiet for 1 minute");
    }
  }
  Blynk.virtualWrite(V2,vibrationsPerMinute);
  vibrationsPerMinute = 0;
}

BLYNK_WRITE(V3){   //Reset everything
  vibrationCounter = 0;   // counter for the number of button presses
  vibrationsPerMinute = 0;
  notificationSent = 0;
  Blynk.virtualWrite(V1,0);
  Serial.println("Reset");
  display.setCursor(1,0);
  display.clearDisplay();
  display.print("Reset");
  display.display();
}

BLYNK_READ(V4){
  int sensorVal = analogRead(audioAnalogPin);
  //Serial.println(sensorVal);
  Blynk.virtualWrite(V4,sensorVal);
}

BLYNK_CONNECTED() {
  Blynk.virtualWrite(V1,0);
  display.clearDisplay();
  
  display.setCursor(0,0);
  display.print("Wifi On, waiting for motion");
  display.display();
}
