/*


This is a TouchInput compatible driver for the FT5xx6 CTP hardware

Effectively it is just a wrapper of the pre-existing SparkFun_FT5xx6 library that provides the necesssary hooks

Author: Owen Lyke

*/



#ifndef SF_TOUCHINPUT_DRIVER_FT5xx6_H
#define SF_TOUCHINPUT_DRIVER_FT5xx6_H

#include "SparkFun_TouchInput.h"
#include "util/SparkFun_FT5xx6.h"

// FT5xx6
class sfti_driver_ft5xx6: public sfti_driver{
public:
  FT5xx6* plldrv;
  sfti_record getLastRecord( void );
  bool     hasNewRecord( void );
  sfti_driver_ft5xx6();
};


#endif /* SF_TOUCHINPUT_DRIVER_FT5xx6_H */