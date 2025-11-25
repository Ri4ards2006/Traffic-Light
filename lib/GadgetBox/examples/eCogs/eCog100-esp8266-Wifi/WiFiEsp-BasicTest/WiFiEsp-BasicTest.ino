/**************************************************************
 * Use this sketch to test the basic functionality of the Esp8266 wifi module.
 * Uses the WiFiEsp Library - Once you verify this works then look at the other examples included with the library.
 * 
 * eCogs Used:
 *       eCog100 - Wifi - Connect to eCog C
 * 
 * Libraries required (Install with Library Manager):
 *       GadgetBox
 *       WiFiEsp
 *       
 * GadgetBox Motherboards supported:      
 *       Arduino Mini Pro (Software Serial on eCog C)
 *       Teensy 3.2 (Hardware Serial2 on eCog C)
 *       
 **************************************************************/
/*
 WiFiEsp test: BasicTest
 
 Performs basic connectivity test and checks.
*/

#include "WiFiEsp.h"
#include <GadgetBox.h>

#if defined(CORE_TEENSY)        //Use Hardware Serial for Teensy
  #define EspSerial Serial2
#elif defined(ARDUINO_AVR_PRO)  // or Software Serial on eCog C for Mini Pro
  #include <SoftwareSerial.h>
  SoftwareSerial EspSerial(CC0, CC1); // RX, TX (For Pro Mini)
#endif

char ssid[] = "Twim";     // your network SSID (name)
char pwd[] = "12345678";  // your network password
char pwdErr[] = "xxxx";   // wrong password


void setup()
{
  Serial.begin(9600);
  EspSerial.begin(9600);
  WiFi.init(&EspSerial);
}

void loop()
{
  assertEquals("Firmware version", WiFi.firmwareVersion(), "1.5.4");
  assertEquals("Status is (WL_DISCONNECTED)", WiFi.status(), WL_DISCONNECTED);
  assertEquals("Connect", WiFi.begin(ssid, pwd), WL_CONNECTED);
  assertEquals("Check status (WL_CONNECTED)", WiFi.status(), WL_CONNECTED);
  assertEquals("Check SSID", WiFi.SSID(), ssid);

  IPAddress ip = WiFi.localIP();
  assertNotEquals("Check IP Address", ip[0], 0);
  Serial.print("IP Address: ");
  Serial.println(ip);
  
  byte mac[6]={0,0,0,0,0,0};
  WiFi.macAddress(mac);

  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
  Serial.println();
  
  assertEquals("Disconnect", WiFi.disconnect(), WL_DISCONNECTED);
  assertEquals("Check status (WL_DISCONNECTED)", WiFi.status(), WL_DISCONNECTED);
  assertEquals("IP Address", WiFi.localIP(), 0);
  assertEquals("Check SSID", WiFi.SSID(), "");
  assertEquals("Wrong pwd", WiFi.begin(ssid, pwdErr), WL_CONNECT_FAILED);

  IPAddress localIp(192, 168, 168, 111);
  WiFi.config(localIp);
  
  assertEquals("Connect", WiFi.begin(ssid, pwd), WL_CONNECTED);
  assertEquals("Check status (WL_CONNECTED)", WiFi.status(), WL_CONNECTED);

  ip = WiFi.localIP();
  assertNotEquals("Check IP Address", ip[0], 0);


  Serial.println("END OF TESTS");
  delay(60000);
}


////////////////////////////////////////////////////////////////////////////////////


void assertNotEquals(const char* test, int actual, int expected)
{
  if(actual!=expected)
    pass(test);
  else
    fail(test, actual, expected);
}

void assertEquals(const char* test, int actual, int expected)
{
  if(actual==expected)
    pass(test);
  else
    fail(test, actual, expected);
}

void assertEquals(const char* test, char* actual, char* expected)
{
  if(strcmp(actual, expected) == 0)
    pass(test);
  else
    fail(test, actual, expected);
}


void pass(const char* test)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.println(" > PASSED");
  Serial.println();
}

void fail(const char* test, char* actual, char* expected)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.print(" > FAILED");
  Serial.print(" (actual=\"");
  Serial.print(actual);
  Serial.print("\", expected=\"");
  Serial.print(expected);
  Serial.println("\")");
  Serial.println();
  delay(10000);
}

void fail(const char* test, int actual, int expected)
{
  Serial.print(F("********************************************** "));
  Serial.print(test);
  Serial.print(" > FAILED");
  Serial.print(" (actual=");
  Serial.print(actual);
  Serial.print(", expected=");
  Serial.print(expected);
  Serial.println(")");
  Serial.println();
  delay(10000);
}

