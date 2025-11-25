/* 

Arduino library for control of FT5xx6 Capacitive Touch panels (CTPs).

Based on work by Helge Langehaug (https://github.com/hellange/arduino/tree/master/capacitive_7in_panel)

Author: Owen Lyke

*/

#ifndef SF_FT5XX6
#define SF_FT5XX6

#include <Arduino.h>
#include <Wire.h>

#define FT5XX6_UNUSED_PIN 0xFF

typedef enum{						// Provides type checking for known I2C addresses
	I2C_Address_Unknown = 0x03,		// 
	I2C_Address_FT5316 = 0x38 		// Add more I2C addresses as needed.
}FT5xx6_I2CAddress_TypeDef;

typedef enum{
	FT5xx6_REG_DEV_MODE = 0x00,
	FT5xx6_REG_GEST_ID,
	FT5xx6_REG_TD_STATUS,
	FT5xx6_REG_T1_XH = 0x03,
	FT5xx6_REG_T1_XL,
	FT5xx6_REG_T1_YH,
	FT5xx6_REG_T1_YL,
	// Discontinuous 0x07-0x08 (there may be valid registers here but they were not listed in the reference library.)
	FT5xx6_REG_T2_XH = 0x09,
	FT5xx6_REG_T2_XL,
	FT5xx6_REG_T2_YH,
	FT5xx6_REG_T2_YL,
	// Discontinuous 0x0D-0x0E (there may be valid registers here but they were not listed in the reference library.)
	FT5xx6_REG_T3_XH = 0x0F,
	FT5xx6_REG_T3_XL,
	FT5xx6_REG_T3_YH,
	FT5xx6_REG_T3_YL,
	// Discontinuous 0x13-0x14 (there may be valid registers here but they were not listed in the reference library.)
	FT5xx6_REG_T4_XH = 0x15,
	FT5xx6_REG_T4_XL,
	FT5xx6_REG_T4_YH,
	FT5xx6_REG_T4_YL,
	// Discontinuous 0x19-0x1A (there may be valid registers here but they were not listed in the reference library.)
	FT5xx6_REG_T5_XH = 0x1b,
	FT5xx6_REG_T5_XL,
	FT5xx6_REG_T5_YH,
	FT5xx6_REG_T5_YL,
	// There are probably more valid registers but they are not specifically listed in the reference library
}FT5xx6_RegisterAddresses_Typedef;

typedef enum{
	FT5xx6_Status_Nominal = 0x00,				// The one and only 'all is good' return status
	FT5xx6_Status_Error,						// A general error - meaning is context dependent
	FT5xx6_Status_Not_Enough_Core				// There was not enough memory to allocate a desired element
}FT5xx6_Status_TypeDef;

typedef enum{
	FT5xx6_GEST_ID_NO_GESTURE = 0x00,
	FT5xx6_GEST_ID_MOVE_UP = 0x10,
	FT5xx6_GEST_ID_MOVE_LEFT = 0x14,
	FT5xx6_GEST_ID_MOVE_DOWN = 0x18,
	FT5xx6_GEST_ID_MOVE_RIGHT = 0x1C,
	FT5xx6_GEST_ID_ZOOM_IN = 0x48,
	FT5xx6_GEST_ID_ZOOM_OUT = 0x49
}FT5xx6_Gestures_TypeDef;

typedef struct touchRecord{
	uint8_t numTouches;
	int16_t t1x;
	int16_t t1y;
	int16_t t2x;
	int16_t t2y;
	int16_t t3x;
	int16_t t3y;
	int16_t t4x;
	int16_t t4y;
	int16_t t5x;
	int16_t t5y;
	FT5xx6_Gestures_TypeDef gesture;
	uint32_t timestamp;
}FT5xx6_TouchRecord_TypeDef;


typedef enum{
	FT5xx6_Mode_Interrupt = 0x00,
	FT5xx6_Mode_Polling
}FT5xx6_Mode_TypeDef;

class FT5xx6{
private:
protected:

	public: // temporary

	bool 						_hasInterrupts;			// Pretty self-explanatory
	FT5xx6_Mode_TypeDef			_mode;					// Polling or interrupt
	TwoWire* 					_wire;					// Which I2C port to use
	FT5xx6_I2CAddress_TypeDef 	_addr;					// The I2C address

	uint8_t 					_int;					// The pin (if any) to use for 
	

	FT5xx6_TouchRecord_TypeDef*	_ptouchRecordHead;		// Pointer to the touchRecord buffer (if enabled)
	uint16_t					_writeOffset;			// Offset of the head touchRecord pointer to the Write location
	uint16_t					_readOffset;			// Offset of the head touchRecord pointer to the Read location
	uint16_t 					_recordDepth;			// Total number of records allowed in the buffer
	volatile uint16_t 			_recordsAvailable;		// The number of unread records
	bool 						_writeOK;				// If it is OK to write another record
	bool 						_readOK;				// If it is OK to read another record
	bool 						_bufferWasAllocated;	// Keeps track of whether the buffer needs to be disallocated on 'removeBuffer'

	FT5xx6_Status_TypeDef 		getTouchRecord( FT5xx6_TouchRecord_TypeDef * precord );											// Retrieves touch data from the CTP
	FT5xx6_Status_TypeDef		write( FT5xx6_TouchRecord_TypeDef* precord, uint16_t numRecords ); 								// Write a touch record (or series of touch records) to the buffer

	FT5xx6_Status_TypeDef		returnStatus(FT5xx6_Status_TypeDef retval, const char * file, uint16_t line);							// A way to exit functions that return statuses as well as provide a hook for the user

public:

	FT5xx6_TouchRecord_TypeDef 	lastTouch;																						// The most recent touch record
	uint32_t 					lastUpdate;																						// The system time of the last update() call (ms)
	bool 						newTouch;
	bool						newData;

	FT5xx6(uint8_t address = I2C_Address_Unknown);																				// Constructor

	FT5xx6_Status_TypeDef begin(TwoWire &wirePort = Wire, uint8_t intPin = FT5XX6_UNUSED_PIN, void (*userISR)(void) = NULL);	// Begin with provisions for interrupt-driven or polling based sampling
	FT5xx6_Status_TypeDef useBuffer(uint8_t depth = 0, FT5xx6_TouchRecord_TypeDef * ptouchRecords = NULL);						// Allow use of a buffer. Dynamically allocate if no address is provided
	FT5xx6_Status_TypeDef removeBuffer( void );

	uint16_t 					available( void ); 																				// Returns the number of touchRecords that are available
	FT5xx6_Status_TypeDef		clearBuffer( void );																			// Sets 'recordsAvailable' to 0 and resets read/write offsets								
	FT5xx6_TouchRecord_TypeDef 	read( void );																					// Return the oldest (first received) touch record, move read pointer, and decrement 'recordsAvailable'
	FT5xx6_TouchRecord_TypeDef 	peek( uint16_t offsetFromRead = 0 );															// Get the touch record at a particular offset from the read offset (modulo the recordDepth)

	FT5xx6_Status_TypeDef		setMode(FT5xx6_Mode_TypeDef mode);

	FT5xx6_Status_TypeDef update( void );																						// Used in polling. Reads from the CTP and sees if there are any new touches
	FT5xx6_Status_TypeDef interrupt( void );																					// Used for interrupts. Call this member function in your ISR function (as provided to 'begin()')
	virtual void abstractGuard( void ) = 0;																						// A guard that prevents instantiaion of this base class directly

};

// Note: the functor approach will not work because you can't generally convert a functor to a function-pointer
// // A functor to act as the callback for an interrupt from a given FT5xx6 object:
// class FT5xx6_InterruptCallback{
// private:
// 	public: // temporary
// 	FT5xx6* me;	// A pointer to the FT5xx6 object on which to call the method
// protected:
// public:
// 	FT5xx6_InterruptCallback(FT5xx6* target) : me(target) { }
// 	void operator () (void) const {
// 		me->interrupt();
// 	}
// };


// Callbacks available to the user
void 	FT5xx6_ReturnCallback(FT5xx6_Status_TypeDef retval, const char * file, uint16_t line)	__attribute__ ((weak));
void 	FT5xx6_InterruptCallback(FT5xx6* me)											__attribute__ ((weak));



// Specific implemenations with known I2C addresses
class FT5316 : public FT5xx6{
private:
protected:
public:
	FT5316();						// Constructor
	void abstractGuard( void );		// Implement this function to allow instantiation of this class (it doesn't need to do anything...)
};



#endif

