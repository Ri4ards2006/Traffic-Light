/**************************************************************
 * This is an example for GadgetBox to create a Learning IR Remote control using Blynk
 * 
 * There are up to 10 IR commands that can be learned. Raw IR commands not supported.
 * 
 * eCogs Used:
 *       eCog107 - Remote Control - Connect to eCog B (Except for Teensy - Move to eCog D for the Teensy)
 *       eCog100 - Wifi - Connect to eCog C
 * 
 * Libraries required (Install with Library Manager):
 *       GadgetBox
 *       IRremote
 *       Blynk
 *       IRremoteESP8266 if on NodeMCU. Install manually from https://github.com/markszabo/IRremoteESP8266
 *       
 * GadgetBox Motherboards supported:      
 *       Arduino Mini Pro (Only 4 IR commands)
 *       Teensy (Be sure to install the Teensy version of IRremote)
 *       
 * Blynk App:
 *       Clone the following project in Blynk http://tinyurl.com/hjrc5pu
 *       
 * Blynk Virtual Pins:      
 *       V0:      The table to select which IR command to learn
 *       V1-V10:  Trigger a send of the IR command - connect a button or timer to send out IR command 
 *       V14:     Terminal for feedback
 *       V15:     RTC (Real Time Clock)
 *       V16:     Time Now
 *       V21-V30: Stores the IR command that is learned in the Blynk cloud
 *       V31:     LCD widget for feedback while learning
 ***************************************************************       
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
 **************************************************************/
#include <GadgetBox.h>          //Install GadgetBox Library from Library Manager
#include "password.h"           //Open password.h and update your credentials

//#define SKETCHDEBUG             //Uncomment to print out debug info, uses more space... Only provides one IR command
//#define BLYNK_PRINT Serial

#if defined(CORE_TEENSY)        //Use Hardware Serial for Teensy
  #define EspSerial Serial2
  #define BLYNK_PRINT Serial
#elif defined(ARDUINO_AVR_PRO)  // or Software Serial on eCog C for Mini Pro
  #include <SoftwareSerial.h>
  SoftwareSerial EspSerial(CC0, CC1); // RX, TX (For Pro Mini)
  #define SMALL
#elif defined(ESP8266)
  #define BLYNK_PRINT Serial
  #include "GadgetBox_esp8266.h"
  const char* GBName = "IRBox";
#endif

#if defined(ESP8266)
  IRsend irsend(BC3);
  int RECV_PIN = BC2;             //Pin for IR Receiver - on eCog B by default
#else
  #include <ESP8266_Lib.h>
  #include <BlynkSimpleShieldEsp8266.h>
  #include <IRremote.h>           //Install IRremote library from Library Manager  
  // Your ESP8266 baud rate:
  #define ESP8266_BAUD 9600 
  ESP8266 wifi(&EspSerial); 
  IRsend irsend;
  int RECV_PIN = BC2;             //Pin for IR Receiver - on eCog B by default 
#endif

#include <SimpleTimer.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

SimpleTimer timer;

#if !defined(SMALL)
#define FLIPDISPLAY       //Should the OLED display be flipped? Comment out if not.
String currentTime;
String currentDate;
int retries = 5;
int reconnects = 0;

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 99
Adafruit_SSD1306 display(OLED_RESET);

WidgetTerminal terminal(V14);
WidgetRTC rtc;

BLYNK_ATTACH_WIDGET(rtc, V15);

boolean receiveEnable = true;
int receiveTmn = 0;
#endif

IRrecv irrecv(RECV_PIN);

decode_results results;

WidgetLCD lcd(V31);

void setup()
{
  irrecv.enableIRIn(); // Start the receiver
  Serial.begin(9600);
#if defined(ESP8266)
  Wire.begin(0,SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  #if defined(FLIPDISPLAY)
    display.setRotation(90);
  #endif
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(GBName);
  display.println("Waiting");
  display.println("For WiFi");    
  display.display();

  //startWifiManager(GBName);   //Comment this out if you want to just type credentials into password.h

  //config blynk
  if (strlen(settings.blynkToken) == 0) {
    BLYNK_ENABLED = false;
    Blynk.begin(auth, ssid, pass);  //If WifiManager did not work or is disabled then manually start Blynk with credentials from password.h
  }
  if (BLYNK_ENABLED) {
    Blynk.config(settings.blynkToken, settings.blynkServer, atoi(settings.blynkPort));
  }  

  startOTA(GBName); 
  irsend.begin();
  Wire.begin(0,SCL);
#else
  delay(10);
  EspSerial.begin(115200);
  EspSerial.println("AT+CIOBAUD=9600");
  delay(1000);
  Blynk.begin(auth, wifi, ssid, pass);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);  
#endif

#if !defined(SMALL)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  #if defined(FLIPDISPLAY)
    display.setRotation(90);
  #endif
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(GBName);
  display.println("Waiting");
  display.println("For Blynk");    
  display.display();

  while (Blynk.connect() == false) {
    // Wait until connected
    if (retries == 0){
      Serial.println("Too many failed connects, restarting...");
      reset();
    }
    else
      retries--;
  }

  // Begin synchronizing time
  rtc.begin();

  // Display digital clock every 30 seconds on Blynk app
  timer.setInterval(30000L, blynkClockDisplay);
  // Display digital clock every 1 seconds on OLED Display
  timer.setInterval(10000L, clockDisplay); 
  //This disables the receiver when sending
  receiveTmn = timer.setInterval(2000L, enableReceiver);
#endif
}

void loop()
{
#if defined(ESP8266)
  //ota loop
  ArduinoOTA.handle();
#endif
  
  Blynk.run();
#if !defined(SMALL) 
  timer.run();

  if (receiveEnable) {
    if (irrecv.decode(&results)) {
      storeCode(&results);
      irrecv.resume();
    }
  }
#endif  
}

// Storage for the recorded code
int codeType = -1; // The type of code
unsigned long codeValue; // The code value if not raw
//unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int toggle = 0; // The RC5/6 toggle state

// Stores the code for later playback
// Most of this code is just logging
void storeCode(decode_results *results) {
  codeType = results->decode_type;
  int count = results->rawlen;
#if !defined(SMALL)  
  //terminal.print("IR Received: ");
  if (codeType == UNKNOWN) {
  }
  else {
    if (codeType == NEC) {
        Serial.print("Received NEC: ");
        terminal.print("Received NEC: ");
      if (results->value == REPEAT) {
        return;
      }
    } 
    else if (codeType == SONY) {
        Serial.print("Received SONY: ");
        terminal.print("Received SONY: ");    
    } 
    else if (codeType == PANASONIC) {
        Serial.print("Received PANASONIC: "); 
        terminal.print("Received PANASONIC: "); 
    }
    else if (codeType == JVC) {
        Serial.print("Received JVC: ");
        terminal.print("Received JVC: ");  
    }
    else if (codeType == RC5) {
        Serial.print("Received RC5: ");
        terminal.print("Received RC5: ");   
    } 
    else if (codeType == RC6) {
        Serial.print("Received RC6: "); 
        terminal.print("Received RC6: ");     
    } 
    else {
        Serial.print("Unexpected codeType ");
        Serial.print(codeType, DEC);
        Serial.println("");  
        terminal.print("Unexpected codeType: ");     
    }
  }
    terminal.print(results->value, HEX);
    terminal.print(" length: ");
    terminal.println(results->bits);
    terminal.flush();  
    Serial.print(results->value, HEX);
    Serial.print(" length: ");
    Serial.println(results->bits);    
#endif
    lcd.print(5,1,results->value);
    lcd.print(14,1,results->bits);
    codeValue = results->value;
    codeLen = results->bits;
}

void sendCode(int repeat) {
#if !defined(SMALL)
  if (receiveEnable){
    irsend.sendNEC(codeValue,codeLen);  //Just clear the sender
    irsend.sendNEC(codeValue,codeLen);  //Just clear the sender
    irsend.sendNEC(codeValue,codeLen);  //Just clear the sender
    Serial.println("Stopping IR Receive while Sending");
    terminal.println("Stopping IR Receive while Sending");
    terminal.flush();    
  }
  receiveEnable = false;
  timer.restartTimer(receiveTmn);
#endif
  if (codeType == NEC) {
    if (repeat) {
      irsend.sendNEC(REPEAT, codeLen);
    } 
    else {
      irsend.sendNEC(codeValue, codeLen);
    }
  } 
  else if (codeType == SONY) {
    irsend.sendSony(codeValue, codeLen);
  } 
  else if (codeType == PANASONIC) {
    irsend.sendPanasonic(codeValue, codeLen);
  }
  else if (codeType == JVC) {
    irsend.sendPanasonic(codeValue, codeLen);
  }
  else if (codeType == RC5 || codeType == RC6) {
    if (!repeat) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    }
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      irsend.sendRC5(codeValue, codeLen);
    } 
    else {
      irsend.sendRC6(codeValue, codeLen);
    }
  } 
  else if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    //irsend.sendRaw(rawCodes, codeLen, 38);    Raw is disabled to save space...
  }
}

void learnIR(int rowId) {
  lcd.clear();
  lcd.print(0,0,"Waiting for IR");
  int found = 0;
  irrecv.enableIRIn();
  for(int i = 0;i<50;i++){
    if(irrecv.decode(&results)){
      storeCode(&results);
      found = 1;
      switch (rowId)
      {
        case 1:
          Blynk.virtualWrite(V0, "pick", 1);
          Blynk.virtualWrite(V21,codeType,codeValue,codeLen);
          break;
#if !defined(SKETCHDEBUG)            
        case 2:
          Blynk.virtualWrite(V0, "pick", 2);
          Blynk.virtualWrite(V22,codeType,codeValue,codeLen);
          break;
        case 3:
          Blynk.virtualWrite(V0, "pick", 3);
          Blynk.virtualWrite(V23,codeType,codeValue,codeLen);
          break;
        case 4:
          Blynk.virtualWrite(V0, "pick", 4);
          Blynk.virtualWrite(V24,codeType,codeValue,codeLen);
          break;  
#if !defined(ARDUINO_AVR_PRO)            
        case 5:
          Blynk.virtualWrite(V0, "pick", 5);
          Blynk.virtualWrite(V25,codeType,codeValue,codeLen);
          break;          
        case 6:
          Blynk.virtualWrite(V0, "pick", 6);
          Blynk.virtualWrite(V26,codeType,codeValue,codeLen);
          break;    
        case 7:
          Blynk.virtualWrite(V0, "pick", 7);
          Blynk.virtualWrite(V27,codeType,codeValue,codeLen);
          break;  
        case 8:
          Blynk.virtualWrite(V0, "pick", 8);
          Blynk.virtualWrite(V28,codeType,codeValue,codeLen);
          break;  
        case 9:
          Blynk.virtualWrite(V0, "pick", 9);
          Blynk.virtualWrite(V29,codeType,codeValue,codeLen);
          break;  
        case 10:
          Blynk.virtualWrite(V0, "pick", 10);
          Blynk.virtualWrite(V30,codeType,codeValue,codeLen);
          break;                                                                       
#endif
#endif           
        default:
          break;
      }
      lcd.print(0,0,"IR Found      ");
      break; 
    }
    delay(100);
  }
  if (!found) {
    lcd.clear(); 
    lcd.print(0,0,"IR Timeout");
  }
}

BLYNK_WRITE(V0)
{
  //Learn Mode
   String cmd = param[0].asStr();
   if (cmd == "select") {
       //row in table was selected.        
        learnIR(param[1].asInt());
   }
   if (cmd == "deselect") {
       //row in table was deselected. 
       learnIR(param[1].asInt());
   }
}

BLYNK_WRITE(V1)
{
  if(param[0].asInt()){
    Blynk.syncVirtual(V21);     
  }
}
#if !defined(SKETCHDEBUG)  
BLYNK_WRITE(V2)
{
  if(param[0].asInt()){
    Blynk.syncVirtual(V22); 
  }
}

BLYNK_WRITE(V3)
{
  if(param[0].asInt()){
    Blynk.syncVirtual(V23); 
  }
}

BLYNK_WRITE(V4)
{
  if(param[0].asInt()){
    Blynk.syncVirtual(V24); 
  }
}
#if !defined(SMALL) 
  BLYNK_WRITE(V5)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V25); 
    }
  }

  BLYNK_WRITE(V6)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V26); 
    }
  }
  
  BLYNK_WRITE(V7)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V27); 
    }
  }
  
  BLYNK_WRITE(V8)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V28); 
    }
  }

  BLYNK_WRITE(V9)
  {
    Serial.println("V9");
    if(param[0].asInt()){
      Blynk.syncVirtual(V29); 
    }
  }
  
  BLYNK_WRITE(V10)
  {
    Serial.println("V10");
    if(param[0].asInt()){
      Blynk.syncVirtual(V30); 
    }
  }
#endif
#endif 

BLYNK_WRITE(V21)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    sendCode(0); 
    #if !defined(SMALL)
    terminal.print("IR1 Sent at: ");
    terminal.println(currentTime);
    terminal.flush();
    #endif      
}
#if !defined(SKETCHDEBUG)  
BLYNK_WRITE(V22)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    sendCode(0);
    #if !defined(SMALL)
    terminal.print("IR2 Sent at: ");
    terminal.println(currentTime);
    terminal.flush();
    #endif    
}

BLYNK_WRITE(V23)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    sendCode(0);
    #if !defined(SMALL)
    terminal.print("IR3 Sent at: ");
    terminal.println(currentTime);
    terminal.flush();
    #endif       
}

BLYNK_WRITE(V24)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    sendCode(0);
    #if !defined(SMALL)
    terminal.print("IR4 Sent at: ");
    terminal.println(currentTime);
    terminal.flush();
    #endif         
}
#if !defined(SMALL) 
  BLYNK_WRITE(V25)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
      terminal.print("IR5 Sent at: ");
      terminal.println(currentTime);
      terminal.flush();
  }

  BLYNK_WRITE(V26)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
      terminal.print("IR6 Sent at: ");
      terminal.println(currentTime);
      terminal.flush();
  }
  
  BLYNK_WRITE(V27)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
      terminal.print("IR7 Sent at: ");
      terminal.println(currentTime);
      terminal.flush();
  }
  
  BLYNK_WRITE(V28)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
      terminal.print("IR8 Sent at: ");
      terminal.println(currentTime);
      terminal.flush();
  }

  BLYNK_WRITE(V29)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
      sendCode(0);
      sendCode(0);
      terminal.print("IR9 Sent at: ");
      terminal.println(currentTime);
      terminal.flush();
  }

  BLYNK_WRITE(V30)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
      sendCode(0);
      sendCode(0);
      terminal.print("IR10 Sent at: ");
      terminal.println(currentTime);
      terminal.flush();
  }
#endif
#endif 

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  //Setup the IR command table
  Blynk.virtualWrite(V0, "clr");
  Blynk.virtualWrite(V0, "add", 0, "Button", "Virtual Pin");
  Blynk.virtualWrite(V0, "add", 1, "IR1", "V1");
  #if !defined(SKETCHDEBUG)  
  Blynk.virtualWrite(V0, "add", 2, "IR2", "V2");
  Blynk.virtualWrite(V0, "add", 3, "IR3", "V3");
  Blynk.virtualWrite(V0, "add", 4, "IR4", "V4");
  #if !defined(SMALL) 
    Blynk.virtualWrite(V0, "add", 5, "IR5", "V5"); 
    Blynk.virtualWrite(V0, "add", 6, "IR6", "V6");   
    Blynk.virtualWrite(V0, "add", 7, "IR7", "V7"); 
    Blynk.virtualWrite(V0, "add", 8, "IR8", "V8"); 
    Blynk.virtualWrite(V0, "add", 9, "IR9", "V9"); 
    Blynk.virtualWrite(V0, "add", 10, "IR10", "V10");
  #endif
  #endif

#if defined(CORE_TEENSY)
  lcd.clear();
  lcd.print(0,0,"Teensy");
  lcd.print(0,1,"Connected");
#elif defined(ARDUINO_AVR_PRO)
  lcd.clear();
  lcd.print(0,0,"Mini Pro");
#elif defined(ESP8266)
  lcd.clear();
  lcd.print(0,0,"NodeMCU");
  lcd.print(0,1,"Connected");
#endif

#if !defined(SMALL)
  terminal.print(GBName);
  terminal.println(" Online");
  terminal.print(reconnects);
  terminal.println(" reconnects so far.");
  terminal.flush();

  Serial.print(reconnects);
  Serial.println(" reconnects so far.");
  if (reconnects == 4) {
    terminal.println("Too many reconnects, please stop Blynk App, wait 20 seconds, and restart.");
    terminal.println("There is a bug in the Blynk App...");
    terminal.flush();
    #if defined(ESP8266)
      restart();
    #endif
  }
  else 
    reconnects++;  

  display.clearDisplay();
  display.setRotation(90);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(GBName);
  display.println("Connected");    
  display.display(); 
#endif



  delay(4000);
  lcd.clear();
  lcd.print(0,0,"Tap a line");
  lcd.print(0,1,"to Learn IR");
}

#if !defined(SMALL)
// Utility function for digital clock display: prints preceding colon and leading 0
void printDigits(int digits)
{
  Serial.print(":");
  if (digits < 10) {
    Serial.print('0');
  }
  Serial.print(digits);
}

// Digital clock display of the time on Blynk App
void blynkClockDisplay()
{
  currentTime = String(hour()) + ":" + minute() + ":" + second();
  currentDate = String(month()) + " " + day() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();

  // Send time to the App
  Blynk.virtualWrite(V16, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V17, currentDate);  
}

// Digital clock display of the time on OLED display
void clockDisplay()
{
  currentTime = String(hour()) + ":" + minute() + ":" + second();
  currentDate = String(month()) + " " + day() + " " + year();

  display.clearDisplay();
  display.setCursor(0,0);
  display.println(GBName);
  if (Blynk.connected())
    display.println("Connected");    
  else
    display.println("Offline");
  display.println(currentTime);   
  display.print(currentDate);  
  display.display();   
}

void enableReceiver()
{
  if (!receiveEnable) {
    receiveEnable = true;
    irrecv.enableIRIn();
//    irrecv.resume();
    Serial.println("IR Receive Enabled.");    
    terminal.println("IR Receive Enabled.");
    terminal.flush();
  }
}
#endif
