#include "SparkFun_TouchInput_Driver_FT5xx6.h"

// A statically allocated coordinate array to hold the coordinate data for the current touchRecord
sfti_coord_t staticCoordinateArry[FT5XX6_MAX_TOUCH_COORDS];


sfti_driver_ft5xx6::sfti_driver_ft5xx6( void ) : sfti_driver()
{
  plldrv = NULL;
}

sfti_record sfti_driver_ft5xx6::getLastRecord( void )
{
	sfti_record retRecord(FT5XX6_MAX_TOUCH_COORDS, staticCoordinateArry); 

	FT5xx6_TouchRecord_TypeDef llrecord = plldrv->read();

	uint8_t numTouches = llrecord.numTouches;
	retRecord.numTouches = numTouches;
	switch(numTouches)
	{
		case 5 : 
			(retRecord.pcoords + 4)->x = (float)llrecord.t5x;
			(retRecord.pcoords + 4)->y = (float)llrecord.t5y;
		case 4 :
			(retRecord.pcoords + 3)->x = (float)llrecord.t4x;
			(retRecord.pcoords + 3)->y = (float)llrecord.t4y;
		case 3 :
			(retRecord.pcoords + 2)->x = (float)llrecord.t3x;
			(retRecord.pcoords + 2)->y = (float)llrecord.t3y;
		case 2 :
			(retRecord.pcoords + 1)->x = (float)llrecord.t2x;
			(retRecord.pcoords + 1)->y = (float)llrecord.t2y;
		case 1 :
			(retRecord.pcoords + 0)->x = (float)llrecord.t1x;
			(retRecord.pcoords + 0)->y = (float)llrecord.t1y;
			break;
		default :
			break;
	}
	retRecord.timestamp = llrecord.timestamp;

	return retRecord;
}

bool sfti_driver_ft5xx6::hasNewRecord( void )
{
  if(plldrv != NULL)
  {
  	if(plldrv->newData)
  	{
  		plldrv->update();
  	}
  	else if(plldrv->_mode != FT5xx6_Mode_Interrupt)
  	{
  		plldrv->update();																				// Used in polling. Reads from the CTP and sees if there are any new touches
  	}

    if(plldrv->newTouch)
    {
    	return true;
    }
    return false;
  }
  return false;
}