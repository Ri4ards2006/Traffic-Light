/*

SparkFun_TouchDriver.h

Purpose: This header responds to the need of the TouchDevice to have a driver
The needs are specifically:
- To alert the device if there is new touch data to handle
- To serve up the new touch data to the device in the TouchRecord format

Author: Owen Lyke

*/
#ifndef SF_TOUCH_DRIVER_H
#define SF_TOUCH_DRIVER_H

#include "SparkFun_TouchTypes.h"

#define FT5XX6_MAX_TOUCH_COORDS 5 // the driver simply can't report any more!

// The base class
class sfti_driver{
private:
protected:
public:
// Private (one day)
// Protected (one day)
// Public
	virtual sfti_record getLastRecord( void ) = 0;		// Returns a copy of the oldest unread record that the hardware picked up
	virtual bool 		hasNewRecord( void ) = 0;		// Tells if the driver has new data
};

#endif /* SF_TOUCH_DRIVER_H */