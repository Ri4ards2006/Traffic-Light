/*

SparkFun_TouchRecord.cpp
Header: SparkFun_TouchRecord.h

Purpose:

Author: Owen Lyke

*/

#include "SparkFun_TouchRecord.h"



sfti_record::sfti_record()
{
	valid = false;
	dynamic = false;

	maxTouches = 0;	
	numTouches = 0;	
	pcoords = NULL;	
	timestamp = 0;	
}

sfti_record::sfti_record(sfti_mxtouch_t max, sfti_coord_t* pc)
{
	valid = false;
	dynamic = false;

	maxTouches = 0;	
	numTouches = 0;	
	pcoords = NULL;	
	timestamp = 0;	

	if(pc == NULL)
	{
		sfti_coord_t* ptemp = NULL;
		ptemp = (sfti_coord_t*)malloc(max*sizeof(sfti_coord_t)); 
		// ptemp = new (nothrow) sfti_coord_t [max];					// "nothrow" not declared in the scope
		if(ptemp != NULL)
		{
			valid = true;
			dynamic = true;
			pcoords = ptemp;
			maxTouches = max;
			Serial.println("Dynamically allocated space for coordinates");
		}
	}
	else
	{
		valid = true; 
		maxTouches = max;	
		pcoords = pc;	
	}
}

// sfti_record::~sfti_record()										// Destructor
// {
// 	// If the memory for the coordinates was dynamically allocated then we need to get rid of it
// 	if(dynamic)
// 	{
// 		// free(pcoords);
// 		delete[] pcoords;			// new/delete used in favor over malloc/free because c++
// 		Serial.println("Deallocated coordinates");
// 	}
// }