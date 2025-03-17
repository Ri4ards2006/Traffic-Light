/*

SparkFun_TouchInput.h

Purpose: This is the main include file for the SparkFun Touch Input Arduino Library. 
The library provides an easy way to set up touch input devices that consist of a
driver (to actually get touch positions from hardware) and elements that respond to 
touches. 

Author: Owen Lyke

ToDo:
- Add provisions for buffered touch records when the update() function cannot be called as frequently as desired.

*/
#ifndef SF_TOUCH_INPUT_H
#define SF_TOUCH_INPUT_H

#include "Arduino.h"

#include "util/SparkFun_TouchTypes.h"		// Types is a header file that can be used to flexibly determine the variable types used for various purposes in the library 	
#include "util/SparkFun_2DRayTracing.h"		// 2DRayTracing is a header file that makes it easy to detect if a point is within an arbitrary area.
#include "util/SparkFun_TouchRecord.h"		// Record defines a common interface for touch data in the library
#include "util/SparkFun_TouchElement.h"		// Element defines a common way to create interactive objects
#include "util/SparkFun_TouchDriver.h"		// Driver defines how different touchscreen controllers interface to the library


// The device class is what the user will interact with most often
class sfti_device{
private:
protected:
public:
// Private (one day)
	
	sfti_record record;

	sfti_element* pFirstE;		// Pointer to the first element
	sfti_driver* pDriver;		// Pointer to the driver for the device
	bool valid;					// If the device has been configured to a satisfactory level or not
// Protected (one day)
// Public (one day)
	sfti_device();								// Constructor
	void linkDriver(sfti_driver* pdrv);			// How to add a driver to your device
	void addElement(sfti_element* pelem);		// How to add an element to your device
	void removeElement(sfti_element* pelem);	// How to get rid of an element from your device

	void update( void ); 						// Asks the driver if there are any new records and if there are it feeds those records to all associated elements
};
















// typedef uint8_t 	sfti_pressure_t;
// typedef uint8_t 	sfti_max_t;					// A type that defines a limit to the number of touches that a record can hold
// typedef uint16_t	sfti_extent_t;				// A type that desfines the maximum linear dimension of an interactive surface (probably in pixels) 
// // typedef int32_t		sfti_relative_t;			// A type that defines the maximum relative displacement from another point

// typedef sf2drt_coordinate_t sfti_coordinate_t;	// A type to use for coordinates

// // The base class for a touch input record
// class sftir_b		// b is for base
// {	
// private:
// protected:
// 	sfti_max_t				maxEntries;	// The maximum number of valid entries that COULD be held in the arrays pointed to by the members
// public:
// 	sfti_max_t				numEntries;	// The number of valid entries in the record. Must be less than or equal to the maximum number of possible entries
// 	sfti_coordinate_t* 	pcoords;	// Pointer to the coordinates. Number of sfti_coordinate_t types must be greater than or equal to numEntries always
// 	bool 					valid;

// 	sftir_b();	// Constructor

// 	void init(sfti_max_t max, sfti_coordinate_t* ppoints);		
// 	void setCoord(sfti_max_t index, sfti_coordinate_t coord);
// };

// // A touch input record that has pre-allocated memory for five touch coordinates - makes it easier to set up and use
// class sftir_b5 : public sftir_b{
// private:
// protected:
// 	sfti_coordinate_t fiveCoords[5];
// public:
// 	sftir_b5() : sftir_b()
// 	{
// 		init(5, fiveCoords); // Initialize the base class with statically-allocated memory
// 	}
// };




// // The base class of a touh input element - that is anything that the user would interact with including:
// // - Buttons
// // - Sliders
// // - Knobs
// // - Gestures
// class sftie{
// private:
// protected:
// public:
// 	sftie();									// Constructor
// 	void callCallback(void (*cllbck)( void ));		// To carefully call a void void callback (making sure it is not NULL first)
// 	virtual void process(sftir_b record) = 0;	// The function that is used to make an element do its thing!
// };


// // A button. This is simple - a fixed-position area that can be pressed or not pressed
// class sftie_button : public sftie, public sf2drt_polygon{
// private:
// protected:

// 	void (*onPress)( void );						// Callback for when the button is pressed
// 	void (*onRelease)( void );						// Callback for when the button is released

// public:
// 	bool isPressed;									// State of the button

// 	sftie_button(); 								// Construtor

// 	void setFromPolygon(sf2drt_polygon poly); 		// Copies the properties of a polygon into the internal polygon object

// 	void setOnPress(void (*func)(void));			// Sets the callback function for when a button is pressed
// 	void setOnRelease(void (*func)(void)); 			// Sets the callback function for when a button is released

// 	void process(sftir_b record);					// Process implementation for a button
// };


// // A handle - a fixed-shape area that can be dragged to new locations
// class sftie_handle : public sftie, public sf2drt_polygon{
// private:
// protected:

// 	void (*onPickup)( void );						// Callback for when the handle is grabbed
// 	void (*onSetdown)( void );						// Callback for when the handle is set down
// 	void (*onChange)( void );						// Callback for when the handle reference location changes

// 	float radius(sfti_coordinate_t p1, sfti_coordinate_t p2);	

// public:
// 	bool isHeld;									// Whether or not the handle is currently picked up
// 	sfti_coordinate_t 	ref;						// The reference coordiante of the handle
// 	sfti_coordinate_t 	controlTouch;				// The coordiante of the touch that is currently controlling the handle
// 	sfti_extent_t		stickyRadius;				// How many extent units a captured touch can change between updates and still be considered the same touch

// 	sftie_handle(); 								// Construtor

// 	void setFromPolygon(sf2drt_polygon poly, sfti_coordinate_t reference); 	// Copies the properties of a polygon into the internal polygon object, and sets the desired reference location

// 	void setOnPickup(void (*func)(void));			// Sets the callback function for when a button is pressed
// 	void setOnSetdown(void (*func)(void)); 			// Sets the callback function for when a button is released
// 	void setOnChange(void (*func)(void));			// Sets the callback for when the location of the handle is changed

// 	virtual void process(sftir_b record);					// Process implementation for a handle
// 	virtual sfti_coordinate_t getNearestTouch( sftir_b record, sfti_coordinate_t wrt );		// Process helper - gets touch nearest to a given point
// 	virtual bool contacted( sftir_b record );
// };


// class sftie_slider : public sftie_handle{
// private:
// protected:
// public:
// 	sftie_slider();	// Constructor

// 	double 		percent;
// 	uint16_t 	stepSize;
// 	int32_t 	min;
// 	int32_t 	max;

// 	int32_t			getVal( void );
// 	virtual double	getPercent( void ) = 0;
// };

// class sftie_linear_slider : public sftie_slider{
// private:
// protected:
// public:
// 	sfti_coordinate_t pMin;	// Location on-screen with minimum value
// 	sfti_coordinate_t pMax;	// Location on-screen with maximum value

// 	sftie_linear_slider(); // Constructor

// 	virtual void process(sftir_b record);
// 	virtual double	getPercent( void );
// };


#endif /* SF_TOUCH_INPUT_H */