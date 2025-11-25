/*
 * contains some (low level) utility functions which have different implementation in the different 
 * environments (esp32, esp8266, native)
 */

#ifndef EXTENDEDTOUCHEVENT_COMPAT
#define EXTENDEDTOUCHEVENT_COMPAT

#if defined ESP32
    #include <esp32-hal.h>
    #include <Arduino.h>
#elif defined ESP8266
    #include <core_esp8266_features.h>
    #include <Arduino.h>
#else
    #include <ArduinoFake.h>
#endif

class Compat {
public:
    /** current time in ms */
    virtual unsigned long ll_millis() {
        return millis();
    };

    // 
    virtual long ll_map(long x, long in_min, long in_max, long out_min, long out_max) {
        return map(x, in_min, in_max, out_min, out_max);
    };
 };

#endif