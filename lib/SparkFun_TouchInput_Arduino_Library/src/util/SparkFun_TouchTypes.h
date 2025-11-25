/*

SparkFun_TouchTypes.h

Purpose:

Author: Owen Lyke

*/


#ifndef SF_TOUCH_TYPES_H
#define SF_TOUCH_TYPES_H

#include "Arduino.h"

typedef uint32_t sfti_time_t;	// Defines the limit of the largets time values you can store
typedef double sfti_extent_t;	// Using floating point types to store the extent of touches because it is oh-so-easy + convenient
struct sfti_coord_t{			// A structure for coordinates
	sfti_extent_t x;
	sfti_extent_t y;
};

typedef uint8_t sfti_mxtouch_t;	// Defines the limit for the number of touches a record can have

typedef uint16_t sfti_mxelem_t; // Defines the limit for the number of elements in a device





#endif /* SF_TOUCH_TYPES_H */