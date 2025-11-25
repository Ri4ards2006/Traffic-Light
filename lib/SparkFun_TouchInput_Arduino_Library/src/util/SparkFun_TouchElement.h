/*

SparkFun_TouchElement.h

Purpose:

Author: Owen Lyke

*/



#ifndef SF_TOUCH_ELEMENT_H
#define SF_TOUCH_ELEMENT_H


#include "SparkFun_TouchTypes.h"
#include "SparkFun_TouchRecord.h"
#include "SparkFun_2DRayTracing.h"

class sfti_element{
private:
protected:
public:

// Private (one day)


// Protected (one day)
	void call(void (*func)( void ));	// A function to safely call void void callback functions

	sfti_element* pprev;	// Pointer to the previous element in a linked list
	sfti_element* pnext;	// Pointer to the next element in a linked list

	bool isFreeElement( void );

// Public
	sfti_element(); 									// Constructor

	virtual void processRecord(sfti_record rec) = 0;	// This is the function that will be called when there is a new touch record. It is up to the derived classes how to handle this

};


////////////////////////////////////////////////
// 											  //
////////////////////////////////////////////////

class sfti_element_button : public sfti_element{
private:
protected:
public:
// Private (one day)
	
// Protected (one day)
	void (*pressed)( void );
	void (*released)( void );
// Public
	sf2drt_polygon poly;
	bool isPressed;

	void processRecord(sfti_record rec);	// This is the function that will be called when there is a new touch record. It is up to the derived classes how to handle this
	
	sfti_element_button(sf2drt_polygon& basepoly, void (*func_pressed)( void ) = NULL, void (*func_released)( void ) = NULL);
	void setPressedCallback(void (*func)( void ));
	void setReleasedCallback(void (*func)( void ));
};

class sfti_element_handle : public sfti_element{
private:
protected:
public:
// Private (one day)
// Protected (one day)
// Public
	sf2drt_polygon poly;	// The area of the handle
	bool isHeld;

	void (*pickedUp)( void );
	void (*setDown)( void );
	void (*moved)( void );

	sfti_coord_t com;

	void processRecord(sfti_record rec);	// This is the function that will be called when there is a new touch record. It is up to the derived classes how to handle this
	sfti_element_handle( sf2drt_polygon& basePoly, void (*func_pickedup)( void ) = NULL, void (*func_setdown)( void ) = NULL,  void (*func_moved)( void ) = NULL);


	void setPickedUpCallback(void (*func)( void ));
	void setSetDownCallback(void (*func)( void ));
	void setMovedCallback(void (*func)( void ));


	bool get_nearest_coord( sfti_record rec, sf2drt_coordinate_t* pcoord );
};

class sfti_element_slider : public sfti_element{
private:
protected:
public:
// Private (one day)
// Protected (one day)
// Public
	void processRecord(sfti_record rec);	// This is the function that will be called when there is a new touch record. It is up to the derived classes how to handle this
	sfti_element_slider();
};


class sfti_element_debugger : public sfti_element{
private:
protected:
public:
// Private (one day)
// Protected (one day)
	HardwareSerial* _debugPort;
// Public
	void processRecord(sfti_record rec);	// This is the function that will be called when there is a new touch record. It is up to the derived classes how to handle this

	// sfti_element_debugger(HardwareSerial& port = Serial);
	sfti_element_debugger(HardwareSerial* port = NULL);

};




#endif /* SF_TOUCH_ELEMENT_H */