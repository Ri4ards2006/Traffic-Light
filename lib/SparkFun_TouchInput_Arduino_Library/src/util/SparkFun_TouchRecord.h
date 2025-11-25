/*

SparkFun_TouchRecord.h

Purpose:

Author: Owen Lyke

*/



#ifndef SF_TOUCH_RECORD_H
#define SF_TOUCH_RECORD_H

#include "SparkFun_TouchTypes.h"

// The record class is used to pass touch data around actively. It uses a lot of storage space though so as few as possible should exist at any given moment
// Large time-histories of touch records should be stored in a custom manner for the specific driver you are using and then converted to a record object when necessary
class sfti_record{
private:
protected:
public:

// Private (one day)
	bool valid;
	bool dynamic;

// Protected (one day)


// Public (one day)

	// The data we actually care about:
	sfti_mxtouch_t maxTouches;		// The maximum number of touches that this record can store
	sfti_mxtouch_t numTouches;		// The current number of active touces represented in the record
	sfti_coord_t* pcoords;			// Pointer to an array of 'maxTouches' coordinate types, of which 'numTouches' are valid
	sfti_time_t timestamp;			// System time when the record was updated

	sfti_record(); 												// Default constructor
	sfti_record(sfti_mxtouch_t max, sfti_coord_t* pc = NULL); 	// Allocation constructor (if you leave pc as null it will attempt dynamic allocation)
};




#endif /* SF_TOUCH_RECORD_H */