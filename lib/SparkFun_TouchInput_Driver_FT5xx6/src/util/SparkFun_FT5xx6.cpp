/*

SparkFun_FT5xx6.cpp
Header: SparkFun_FT5xx6.h

Purpose: 

Author: Owen Lyke

*/

#include "SparkFun_FT5xx6.h"



// Operators for touchRecords
bool operator == (const touchRecord& tr1, const touchRecord& tr2)
{
	if(tr1.numTouches != tr2.numTouches){ return false; }
	if(tr1.t1x != tr2.t1x){ return false; }
	if(tr1.t1y != tr2.t1y){ return false; }
	if(tr1.t2x != tr2.t2x){ return false; }
	if(tr1.t2y != tr2.t2y){ return false; }
	if(tr1.t3x != tr2.t3x){ return false; }
	if(tr1.t3y != tr2.t3y){ return false; }
	if(tr1.t4x != tr2.t4x){ return false; }
	if(tr1.t4y != tr2.t4y){ return false; }
	if(tr1.t5x != tr2.t5x){ return false; }
	if(tr1.t5y != tr2.t5y){ return false; }
	if(tr1.gesture != tr2.gesture){ return false; }
	return true;
}
bool operator != (const touchRecord& tr1, const touchRecord& tr2)
{
	return !(tr1 == tr2);
}






FT5xx6::FT5xx6(uint8_t address)
{
	_hasInterrupts = false;							// Pretty self-explanatory
	_mode = FT5xx6_Mode_Polling;					// Polling or interrupt
	_wire = NULL;									// Which I2C port to use
	_addr = (FT5xx6_I2CAddress_TypeDef)address;		// The I2C address - can be passed in by derived constructors

	_int = FT5XX6_UNUSED_PIN;						// The pin (if any) to use for 

	_ptouchRecordHead = NULL;						// Pointer to the touchRecord buffer (if enabled)
	_writeOffset = 0;								// Offset of the head touchRecord pointer to the Write location
	_readOffset = 0;								// Offset of the head touchRecord pointer to the Read location
	_recordDepth = 0;								// Total number of records allowed in the buffer
	_recordsAvailable = 0;							// The number of unread records
	_writeOK = false;								// If it is OK to write another record
	_readOK = false;								// If it is OK to read another record
	_bufferWasAllocated = false;	

	newTouch = false;
	newData = false;

	lastUpdate = 0x00;				// The system time of the last update() call

	lastTouch.numTouches = 0x00;
	lastTouch.t1x = 0x00;
	lastTouch.t1y = 0x00;
	lastTouch.t2x = 0x00;
	lastTouch.t2y = 0x00;
	lastTouch.t3x = 0x00;
	lastTouch.t3y = 0x00;
	lastTouch.t4x = 0x00;
	lastTouch.t4y = 0x00;
	lastTouch.t5x = 0x00;
	lastTouch.t5y = 0x00;
	lastTouch.gesture = (FT5xx6_Gestures_TypeDef)0x00;
	lastTouch.timestamp = 0x00;
}

// Retrieves touch data from the CTP
FT5xx6_Status_TypeDef 		FT5xx6::getTouchRecord( FT5xx6_TouchRecord_TypeDef * precord )
{
	FT5xx6_Status_TypeDef retval = FT5xx6_Status_Nominal;

	const uint8_t min_num_registers = 31;
	uint8_t current_register = 0;

	uint8_t registers[min_num_registers];

	uint8_t result = _wire->requestFrom((uint8_t)_addr, (uint8_t)min_num_registers);
	if(result == min_num_registers)
	{
		precord->timestamp = millis();
		while(_wire->available())
		{
			registers[current_register++] = _wire->read();
		}

		precord->numTouches = (registers[FT5xx6_REG_TD_STATUS] & 0x0F);
		precord->gesture = (FT5xx6_Gestures_TypeDef)registers[FT5xx6_REG_GEST_ID];
		if(precord->numTouches > 0)
		{ 
			precord->t1x = word(registers[FT5xx6_REG_T1_XH] & 0x0F, registers[FT5xx6_REG_T1_XL]); 
			precord->t1y = word(registers[FT5xx6_REG_T1_YH] & 0x0F, registers[FT5xx6_REG_T1_YL]);
		}
		if(precord->numTouches > 1)
		{ 
			precord->t2x = word(registers[FT5xx6_REG_T2_XH] & 0x0F, registers[FT5xx6_REG_T2_XL]); 
			precord->t2y = word(registers[FT5xx6_REG_T2_YH] & 0x0F, registers[FT5xx6_REG_T2_YL]);
		}
		if(precord->numTouches > 2)
		{ 
			precord->t3x = word(registers[FT5xx6_REG_T3_XH] & 0x0F, registers[FT5xx6_REG_T3_XL]); 
			precord->t3y = word(registers[FT5xx6_REG_T3_YH] & 0x0F, registers[FT5xx6_REG_T3_YL]);
		}
		if(precord->numTouches > 3)
		{ 
			precord->t4x = word(registers[FT5xx6_REG_T4_XH] & 0x0F, registers[FT5xx6_REG_T4_XL]); 
			precord->t4y = word(registers[FT5xx6_REG_T4_YH] & 0x0F, registers[FT5xx6_REG_T4_YL]);
		}
		if(precord->numTouches > 4)
		{ 
			precord->t5x = word(registers[FT5xx6_REG_T5_XH] & 0x0F, registers[FT5xx6_REG_T5_XL]); 
			precord->t5y = word(registers[FT5xx6_REG_T5_YH] & 0x0F, registers[FT5xx6_REG_T5_YL]);
		}
	}
	else
	{
		retval = FT5xx6_Status_Error;
	}
	return returnStatus(retval, __FILE__, __LINE__);
}

// Write a touch record (or series of touch records) to the buffer											
FT5xx6_Status_TypeDef		FT5xx6::write( FT5xx6_TouchRecord_TypeDef* precord, uint16_t numRecords )
{
	FT5xx6_Status_TypeDef retval = FT5xx6_Status_Nominal;

	if((precord == NULL) || (numRecords == 0))
	{
		return returnStatus(FT5xx6_Status_Error, __FILE__, __LINE__);
	}

	if((_ptouchRecordHead != NULL) && (_recordDepth != 0))
	{
		uint16_t inCount = 0;
		for(inCount = 0; inCount < numRecords; inCount++)
		{
			if(_writeOK){
				*(_ptouchRecordHead + _writeOffset++) = *(precord + inCount);
				_readOK = true;															// Just added data that can be read
				_recordsAvailable++;													// Increment available
				if(_writeOffset >= _recordDepth){ _writeOffset = 0; }					// Handle wraparound
				if(_writeOffset == _readOffset){ _writeOK = false; }					// Not allowed to write any more if the write offset reached the read offset from behind
			}
			else
			{
				return returnStatus(FT5xx6_Status_Error, __FILE__, __LINE__);
			}
		}
	}
	else
	{
		lastTouch = *(precord + 0);
	}
	return returnStatus(retval, __FILE__, __LINE__);
} 			

// A way to exit functions that return statuses as well as provide a hook for the user
FT5xx6_Status_TypeDef		FT5xx6::returnStatus(FT5xx6_Status_TypeDef retval, const char * file, uint16_t line)
{
	FT5xx6_ReturnCallback(retval, file, line);
	return retval;
}






// Begin with provisions for interrupt-driven or polling based sampling
FT5xx6_Status_TypeDef FT5xx6::begin(TwoWire &wirePort, uint8_t intPin, void (*userISR)(void))
{
	FT5xx6_Status_TypeDef retval = FT5xx6_Status_Nominal;

	if((intPin != FT5XX6_UNUSED_PIN) && (userISR != NULL))
	{
		_hasInterrupts = true;
		_int = intPin;
		_mode = FT5xx6_Mode_Interrupt;

		pinMode(_int, INPUT);
		attachInterrupt(digitalPinToInterrupt(_int), userISR, FALLING);
	}

	_wire = &wirePort;
	_wire->begin();
	// Set clock outside of the library, if you want

	_wire->beginTransmission(_addr);
	_wire->write(FT5xx6_REG_DEV_MODE);
	_wire->write(0);
	_wire->endTransmission(_addr);

    return returnStatus(retval, __FILE__, __LINE__);

    // return retval;
}		

// Allow use of a buffer. Dynamically allocate if no address is provided		
FT5xx6_Status_TypeDef FT5xx6::useBuffer(uint8_t depth, FT5xx6_TouchRecord_TypeDef * ptouchRecords)
{
	FT5xx6_Status_TypeDef retval = FT5xx6_Status_Nominal;

	if(depth)
	{
		removeBuffer(); 																						// Remove any buffer that may currently exist
		if(ptouchRecords == NULL)
		{
			ptouchRecords = (FT5xx6_TouchRecord_TypeDef*) malloc(depth*sizeof(FT5xx6_TouchRecord_TypeDef));		// Allocate for the new buffer
			if(ptouchRecords == NULL)																			// Check if it was allocated properly
			{																									
				_ptouchRecordHead = NULL; 																		// Make sure we can't make any nasty mistakes
				return returnStatus(FT5xx6_Status_Not_Enough_Core, __FILE__, __LINE__);							// Bail
			}
			_ptouchRecordHead = ptouchRecords;
			_bufferWasAllocated = true;
		}
		else
		{
			_ptouchRecordHead = ptouchRecords;
			_bufferWasAllocated = false;
		}
		_recordDepth = depth;
		_writeOK = true;	// Can accept new data
		_readOK = false; 	// No data to read!
	}
	return returnStatus(retval, __FILE__, __LINE__);
}		

// Deallocates the buffer (or if the buffer was not allocated them simply sets recordDepth to 0)
FT5xx6_Status_TypeDef FT5xx6::removeBuffer( void )
{
	FT5xx6_Status_TypeDef retval = FT5xx6_Status_Nominal;

	if(_bufferWasAllocated)
	{
		free(_ptouchRecordHead);
	}
	clearBuffer();

	_recordDepth = 0;
	_bufferWasAllocated = false;
	_ptouchRecordHead = NULL;	
	_writeOK = false;
	_readOK = false;

	return returnStatus(retval, __FILE__, __LINE__);
}

// Returns the number of touchRecords that are available
uint16_t 					FT5xx6::available( void )
{
	return _recordsAvailable;
}

// Sets 'recordsAvailable' to 0 and resets read/write offsets	
FT5xx6_Status_TypeDef		FT5xx6::clearBuffer( void )
{
	FT5xx6_Status_TypeDef retval = FT5xx6_Status_Nominal;

	_recordsAvailable = 0;
	_writeOffset = 0;
	_readOffset = 0;
	_writeOK = true;
	_readOK = false;

	return returnStatus(retval, __FILE__, __LINE__);
}

// Return the oldest (first received) touch record, move read pointer, and decrement 'recordsAvailable'
FT5xx6_TouchRecord_TypeDef FT5xx6::read( void )
{
	if((_ptouchRecordHead != NULL) && (_recordDepth != 0))
	{
		if(_readOK)
		{
			FT5xx6_TouchRecord_TypeDef retval = *(_ptouchRecordHead + _readOffset++);
			if(_readOffset >= _recordDepth){ _readOffset = 0; }		// Handle wraparound
			if(_readOffset == _writeOffset){ _readOK = false; } 	// Can't read any more when read has caught up to write
			if(_recordsAvailable-- == 0)							// There are no new touches if _recordsAvailable is zero
			{
				newTouch = false;
			}
			_writeOK = true;										// Just made room for the write offset to advace, no matter what

			return retval;
		}
	}
	newTouch = false;												// If not using a buffer then a single read clears the newTouch flag
	return lastTouch;												// If not using the buffer just return the last value;
}

// Get the touch record at a particular offset from the read offset (modulo the recordDepth)
FT5xx6_TouchRecord_TypeDef 	FT5xx6::peek( uint16_t offsetFromRead )
{
	uint16_t offset = (_readOffset + offsetFromRead) % _recordDepth;
	return *(_ptouchRecordHead + offset);
}




FT5xx6_Status_TypeDef		FT5xx6::setMode(FT5xx6_Mode_TypeDef mode)
{
	_mode = mode;
}



// Used in polling. Reads from the CTP and sees if there are any new touches
FT5xx6_Status_TypeDef FT5xx6::update( void )
{
	FT5xx6_Status_TypeDef retval = FT5xx6_Status_Nominal;

	FT5xx6_TouchRecord_TypeDef newRecord;
	retval = getTouchRecord(&newRecord);
	if(retval != FT5xx6_Status_Nominal){ return returnStatus(retval, __FILE__, __LINE__); }

	newData = false;
	lastUpdate = millis();

	if(newRecord != lastTouch)
	{
		write(&newRecord, 1);
		newTouch = true;
	}

	return returnStatus(retval, __FILE__, __LINE__);
}

// Used for interrupts. Call this member function in your ISR function (FT5xx6InterruptCallback)
FT5xx6_Status_TypeDef FT5xx6::interrupt( void )
{
	FT5xx6_Status_TypeDef retval = FT5xx6_Status_Nominal;

	// if(_mode == FT5xx6_Mode_Interrupt)
	// {
	// 	FT5xx6_TouchRecord_TypeDef newRecord;
	// 	retval = getTouchRecord(&newRecord);
	// 	if(retval != FT5xx6_Status_Nominal){ return returnStatus(retval, __FILE__, __LINE__); }
	// 	write(&newRecord, 1); // Always write a new value from an interrupt
	// }
	// else
	// {
	// 	retval = FT5xx6_Status_Error;
	// }

	// set a variable to know that new data is available
	newData = true;

	FT5xx6_InterruptCallback(this);

	return returnStatus(retval, __FILE__, __LINE__);
}

// virtual void abstractGuard( void ) = 0;																	// A guard that prevents instantiaion of this base class directly









void	FT5xx6_ReturnCallback(FT5xx6_Status_TypeDef retval, const char * file, uint16_t line){}
void 	FT5xx6_InterruptCallback(FT5xx6* me){}










// The constructor of this class just makes an FT5xx6 with a particular I2C address
FT5316::FT5316() : FT5xx6(I2C_Address_FT5316)
{

}

// Implement this function to allow instantiation of this class
void FT5316::abstractGuard( void )
{

}		