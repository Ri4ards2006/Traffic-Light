/*
 * Prints debug messages at serial or stdout
 *
 * Author: arduhe
 */

#ifndef EXTENDEDTOUCHEVENT_DEBUG_H
#define EXTENDEDTOUCHEVENT_DEBUG_H

#ifdef ETE_DEBUG
    #ifdef NATIVE
        #define DEBUG_ETE(FMT, ...) printf(FMT, ##__VA_ARGS__);
    #else
   	    #include <Arduino.h>
        #define DEBUG_ETE(FMT, ...) Serial.printf(FMT, ##__VA_ARGS__);
    #endif
#else
    #define DEBUG_ETE(FMT, ...)
#endif
#endif /* EXTENDEDTOUCHEVENT_DEBUG_H */
