#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
  #include <IRremoteESP8266.h>    //Install IRremoteESP8266 library from https://github.com/markszabo/IRremoteESP8266
  #include <DNSServer.h>
  #include <ESP8266WebServer.h>
  #include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager  
  #include <EEPROM.h>
  static bool BLYNK_ENABLED = true;
  #define EEPROM_SALT 12663
  typedef struct {
    int   salt = EEPROM_SALT;
    char  blynkToken[33]  = "";
    char  blynkServer[33] = "blynk-cloud.com";
    char  blynkPort[6]    = "8442";
  } WMSettings;
  
  WMSettings settings;
  #include <ArduinoOTA.h>


  //gets called when WiFiManager enters configuration mode
  void configModeCallback (WiFiManager *myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
    //entered config mode, make led toggle faster
    //ticker.attach(0.2, tick);
  }

  //flag for saving data
  bool shouldSaveConfig = false;

  //callback notifying us of the need to save config
  void saveConfigCallback () {
    Serial.println("Should save config");
    shouldSaveConfig = true;
  }  

  void restart() {
    ESP.reset();
    delay(1000);
  }
  
  void reset() {
    //reset settings to defaults
    /*
      WMSettings defaults;
      settings = defaults;
      EEPROM.begin(512);
      EEPROM.put(0, settings);
      EEPROM.end();
    */
    //reset wifi credentials
    WiFi.disconnect();
    delay(1000);
    ESP.reset();
    delay(1000);
  }  

  void startWifiManager(const char* hostname) {
    WiFiManager wifiManager;
    //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);
  
    //timeout - this will quit WiFiManager if it's not configured in 3 minutes, causing a restart
    wifiManager.setConfigPortalTimeout(180);
  
    //wifiManager.resetSettings();    //Uncomment this to wipe WiFi settings from EEPROM on boot.
  
    //custom params
    EEPROM.begin(512);
    EEPROM.get(0, settings);
    EEPROM.end();
  
    if (settings.salt != EEPROM_SALT) {
      Serial.println("Invalid settings in EEPROM, trying with defaults");
      WMSettings defaults;
      settings = defaults;
    }
  
    Serial.println(settings.blynkToken);
    Serial.println(settings.blynkServer);
    Serial.println(settings.blynkPort);
    
    WiFiManagerParameter custom_blynk_text("Blynk config. <br/> No token to disable.");
    wifiManager.addParameter(&custom_blynk_text);
  
    WiFiManagerParameter custom_blynk_token("blynk-token", "blynk token", settings.blynkToken, 33);
    wifiManager.addParameter(&custom_blynk_token);
  
    WiFiManagerParameter custom_blynk_server("blynk-server", "blynk server", settings.blynkServer, 33);
    wifiManager.addParameter(&custom_blynk_server);
  
    WiFiManagerParameter custom_blynk_port("blynk-port", "blynk port", settings.blynkPort, 6);
    wifiManager.addParameter(&custom_blynk_port);
  
    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);
  
    if (!wifiManager.autoConnect(hostname)) {
      Serial.println("failed to connect and hit timeout");
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(1000);
    }
  
    //Serial.println(custom_blynk_token.getValue());
    //save the custom parameters to FS
    if (shouldSaveConfig) {
      Serial.println("Saving config");
  
      strcpy(settings.blynkToken, custom_blynk_token.getValue());
      strcpy(settings.blynkServer, custom_blynk_server.getValue());
      strcpy(settings.blynkPort, custom_blynk_port.getValue());
  
      Serial.println(settings.blynkToken);
      Serial.println(settings.blynkServer);
      Serial.println(settings.blynkPort);
  
      EEPROM.begin(512);
      EEPROM.put(0, settings);
      EEPROM.end();
    }    
  }

  void startOTA(const char* hostname) {
      //OTA
      ArduinoOTA.onStart([]() {
        Serial.println("Start OTA");
      });
      ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      });
      ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });
      ArduinoOTA.setHostname("hostname");
      ArduinoOTA.begin(); 
  }

#endif
