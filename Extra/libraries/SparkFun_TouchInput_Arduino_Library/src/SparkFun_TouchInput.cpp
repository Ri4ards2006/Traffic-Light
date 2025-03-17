/*

SparkFun_TouchInput.cpp
Header: SparkFun_TouchInput.h

Purpose:

Author: Owen Lyke

*/

#include "SparkFun_TouchInput.h"

sfti_device::sfti_device( void )
{
	pFirstE = NULL;
	pDriver = NULL;
	valid = false;
}

void sfti_device::linkDriver(sfti_driver* pdrv)
{
	if(pdrv != NULL)
	{
		valid = true;
	}
	else
	{
		valid = false;
	}
	pDriver = pdrv;
}

void sfti_device::addElement(sfti_element* pelem)
{
	Serial.println("Adding an element");

	if(pelem != NULL)
	{
		Serial.println("Okay, the pointer itself is not null");

		if(pelem->isFreeElement())
		{
			// So here we are guaranteed to have a "free" element (means it has neither pnext or pprev)
			if(pFirstE == NULL)		// If our current list has no beginning then add it there...
			{
				pFirstE = pelem;
			}
			else					// Otherwise find the end of the list and append it there
			{
				sfti_element* ptemp = pFirstE;
				while(ptemp->pnext != NULL)
				{
					ptemp = ptemp->pnext;	// Advance through the linked list until you get to pnext==NULL
				}
				ptemp->pnext = pelem;	// Link pelem at the end of the list
				pelem->pnext = NULL;	// Ensure that the linked list will still have an end
			}
		}
	}
	else
	{
		// failed
	}
}

void sfti_device::removeElement(sfti_element* pelem)
{
	if(pelem != NULL)
	{
		pelem->pprev = pelem->pnext;	// Remove the middle man
		pelem->pnext = NULL;			// Then make pelem a "free" element by setting pnext and pprev to NULL
		pelem->pprev = NULL;		
	}
}

void sfti_device::update( void )
{
	// Check for touch data updates
	if(pDriver->hasNewRecord())
	{
		record = pDriver->getLastRecord();

		// Process touch data with all the elements
		if(pFirstE != NULL)
		{
			sfti_element* ptemp = pFirstE;

			do{
				ptemp->processRecord(record);
				ptemp = ptemp->pnext;
			}while(ptemp != NULL);
		}
	}
}






// // Touch Input Record : Base
// // Constructor
// sftir_b::sftir_b()
// {
// 	maxEntries = 0;
// 	numEntries = 0;
// 	pcoords = NULL;
// 	valid = false;
// }

// // Initialization: This is how the memory for the record is declared
// void sftir_b::init(sfti_max_t max, sf2drt_coordinate_t* ppoints)
// {
// 	// Static initialization (harder but safer)
// 	numEntries = 0;
// 	maxEntries = 0;
// 	pcoords = NULL;
// 	valid = false;

// 	if(max <= 0){ return; }
// 	if(ppoints == NULL){ return; }
	
// 	maxEntries = max;
// 	pcoords = ppoints;
// 	valid = true;
// }

// // Set a coordinate value
// void sftir_b::setCoord(sfti_max_t index, sf2drt_coordinate_t coord)
// {
// 	*(pcoords + index) = coord;
// }






// // Touch input elements

// // Base class
// // Constructor
// sftie::sftie()
// {

// }

// void sftie::callCallback(void (*cllbck)( void ))
// {
// 	if(cllbck != NULL)
// 	{
// 		(*cllbck)();
// 	}
// }


// // Button class
// sftie_button::sftie_button() : sf2drt_polygon()
// {
// 	onPress = NULL;
// 	onRelease = NULL;

// 	isPressed = false;
// }

// void sftie_button::setFromPolygon(sf2drt_polygon poly)
// {
// 	_p_points = poly._p_points;
// 	_num_sides = poly._num_sides;
// 	_rotation = 0;
// }

// void sftie_button::setOnPress(void (*func)(void))
// {
// 	onPress = func;
// }

// void sftie_button::setOnRelease(void (*func)(void))
// {
// 	onRelease = func;
// }

// void sftie_button::process(sftir_b record)
// {
// 	bool contact = false;
// 	for(sfti_max_t indi = 0; indi < record.numEntries; indi++)
// 	{
// 		if(contains(*(record.pcoords + indi)))
// 		{
// 			contact = true;
// 		}
// 	}

// 	if(!isPressed)
// 	{
// 		if(contact)
// 		{
// 			callCallback(onPress);
// 		}
// 	}


// 	if(isPressed)
// 	{
// 		if(!contact)
// 		{
// 			callCallback(onRelease);
// 		}
// 	}


// 	if(contact)
// 	{
// 		isPressed = true;
// 	}
// 	else
// 	{
// 		isPressed = false;
// 	}
// }




// // Handle objects
// sftie_handle::sftie_handle()
// {
// 	onPickup = NULL;
// 	onSetdown = NULL;
// 	onChange = NULL;

// 	ref = {0,0};
// 	controlTouch = {0,0};
// 	stickyRadius = 20;
// }

// void sftie_handle::setFromPolygon(sf2drt_polygon poly, sfti_coordinate_t reference)
// {
// 	_p_points = poly._p_points;
// 	_num_sides = poly._num_sides;
// 	_rotation = 0;

// 	ref = reference;
// }

// void sftie_handle::setOnPickup(void (*func)(void))
// {
// 	onPickup = func;
// }
// void sftie_handle::setOnSetdown(void (*func)(void))
// {
// 	onSetdown = func;
// }
// void sftie_handle::setOnChange(void (*func)(void))
// {
// 	onChange = func;
// }

// float sftie_handle::radius(sfti_coordinate_t p1, sfti_coordinate_t p2)
// {
// 	return sqrt(pow(((double)p2.x - (double)p1.x),2) + pow(((double)p2.y-(double)p1.y),2));
// }

// void sftie_handle::process(sftir_b record)
// {
// 	if(record.numEntries > 0)
// 	{
// 		if(isHeld)
// 		{
// 			bool maintain = false;

// 			// See if any of the current touches are close enough to the control touch to maintain control
// 			sfti_coordinate_t closestToControl = getNearestTouch(record, controlTouch);
// 			if(radius(closestToControl, controlTouch) <= stickyRadius)
// 			{
// 				maintain = true;
// 			}

// 			// Alternatively you could (also) check if there are any touches still in the polygon
// 			// if(contains(any touch))
// 			// {
// 				// maintain = true;
// 			// }
			

// 			if(maintain)
// 			{
// 				// if still held see if location moved ( update reference coordinate )
// 				if((closestToControl.x != controlTouch.x) || (closestToControl.y != controlTouch.y))
// 				{
// 					// Move the polygon
// 					sf2drt_diff_t dx = closestToControl.x - controlTouch.x;
// 					sf2drt_diff_t dy = closestToControl.y - controlTouch.y;
// 					displace(dx, dy);											// Update the handle area location
// 					ref.x += dx;												// Update reference point location
// 					ref.y += dy;												// Update reference point location

// 					callCallback(onChange);
// 				}
// 				controlTouch = closestToControl;
// 			}
// 			else
// 			{
// 				isHeld = false;
// 				callCallback(onSetdown);
// 			}
// 		}
// 		else
// 		{
// 			// Figure out if the handle got picked up
// 			if(contacted(record ))
// 			{
// 				isHeld = true;
// 				controlTouch = getNearestTouch(record, ref);
// 				callCallback(onPickup);
// 			}	
// 		}
// 	}
// 	else
// 	{
// 		if(isHeld)
// 		{
// 			isHeld = false; // If there are no touches then it can't be held
// 			callCallback(onSetdown);
// 		}
// 	}
// }


// sfti_coordinate_t sftie_handle::getNearestTouch( sftir_b record, sfti_coordinate_t wrt )
// {
// 	// if(record.numEntries > 0)
// 	// {
// 		sfti_coordinate_t retval = *(record.pcoords + 0); // Guaranteed to exist 
// 		double min_rad = radius(retval, wrt);

// 		for(sfti_max_t indi = 1; indi < record.numEntries; indi++)
// 		{
// 			double rad = radius(*(record.pcoords + indi), wrt);
// 			if(rad < min_rad)
// 			{
// 				retval = *(record.pcoords + indi);
// 			}
// 		}
// 		return retval;
// 	// }
// }

// bool sftie_handle::contacted( sftir_b record )
// {
// 	for(sfti_max_t indi = 0; indi < record.numEntries; indi++)
// 	{
// 		if(contains(*(record.pcoords + indi)))
// 		{
// 			// Serial.println("Found it");
// 			return true;
// 			break;					// Break the loop
// 		}
// 		// else
// 		// {
// 		// 	Serial.println("No contact here");
// 		// }
// 	}
// 	return false;
// }


// // Sliders!!

// // Slider constructor
// sftie_slider::sftie_slider() : sftie_handle()
// {
// 	stepSize = 1;
// }

// int32_t	sftie_slider::getVal( void )
// {
// 	int32_t intermediate = (int32_t)round(percent*(double)(max-min));
// 	uint32_t remainder = intermediate % stepSize;
// 	if(remainder > (stepSize/2))
// 	{
// 		intermediate += (stepSize-remainder);
// 	}
// 	else
// 	{
// 		intermediate -= remainder;
// 	}
// 	return intermediate + min;
// }

// // virtual double	sftie_slider::getPercent( void ) = 0;




// sftie_linear_slider::sftie_linear_slider() : sftie_slider()
// {
// 	pMin = {0,0};
// 	pMax = {0,0};
// }

// void sftie_linear_slider::process( sftir_b record )
// {
// 	// This is up next, yo
// 	// long story short I want to make the handle stick to a "rail" of limited length, then return the percent along that rail.

// 	if(record.numEntries > 0)
// 	{
// 		if(isHeld)
// 		{
// 			bool maintain = false; // Will we maintain being held?
// 			sfti_coordinate_t closestToControl = getNearestTouch(record, controlTouch);	// If isHeld then controlTouch represents a valid touch that was the last location that was in control of the handle
// 			if(radius(closestToControl, controlTouch) <= stickyRadius)
// 			{
// 				maintain = true;
// 			}

// 			if(maintain)
// 			{
// 				// Then update the location of the handle - subject to the rules of the rail

// 				// The slopes of the "rail"
// 				double mX = ((double)pMax.x - (double)pMin.x);
// 				double mY = ((double)pMax.y - (double)pMin.y);
// 				// [mX, mY] makes a vector pointing in the direction we want to go.

// 				double qX = (double)closestToControl.x - (double)controlTouch.x;
// 				double qY = (double)closestToControl.y - (double)controlTouch.y;
// 				// [qX, qY] makes a vector pointing in the direction that control moved

// 				// Taking the dot product of the two vectors will give us the constrained motion 
// 				double dot = (mX*qX) + (mY*qY);

// 				// This makes the q vector now representative of the actual movement without endoint constraints 
// 				qX = mX*dot;
// 				qY = mY*dot;

// 				// Multiplying the dot product times the rail vector gives the desired new change, but that is not constrained to any endopoints
// 				// So we need a way to make sure that the result will not run over the endpoints
// 			 	// Since the rail slopes point from min to max we can tell which endpoint we are approaching by the sign of the dot product
// 			 	// Without constraining the relative locations of the min/max points we also need some way to tell if the new point is beyond the endoint... 
// 			 	// Aha! We can use the sign of the difference between min/max??
// 			 	double magq = sqrt(pow(qX,2)+pow(qY,2));
// 			 	if(dot > 0)
// 			 	{
// 			 		// Desired change aligned with min->max vector - so approaching maximum point
// 			 		// Lets take the vector from current point to the maximum point and compare its magnitude with that of the desired change.
// 			 		double fX = (double)pMax.x - (double)ref.x;
// 					double fY = (double)pMax.y - (double)ref.y;

// 					double magf = sqrt(pow(fX,2)+pow(fY,2));
// 					if(magf < magq)
// 					{
// 						qX *= magf/magq; // Should scale down the change so that it does not overshoot the endpoint
// 						qY *= magf/magq;
// 					}
// 			 	}
// 			 	else
// 			 	{
// 			 		// Anti-aligned, so approaching the min point
// 			 		// Lets take the vector from current point to the minimum point and compare its magnitude with that of the desired change.
// 			 		double fX = (double)pMin.x - (double)ref.x;
// 					double fY = (double)pMin.y - (double)ref.y;

// 					double magf = sqrt(pow(fX,2)+pow(fY,2));
// 					if(magf < magq)
// 					{
// 						qX *= magf/magq; // Should scale down the change so that it does not overshoot the endpoint
// 						qY *= magf/magq;
// 					}
// 			 	}

// 			 	// Now we should be able to convert the floating point to an actual displacement to use - we could constrain

// 			 	sf2drt_diff_t dx = round(qX);
// 				sf2drt_diff_t dy = (sf2drt_diff_t)((double)dx*(mY/mX));

// 				if((dx != 0) || (dy != 0))
// 				{
// 					displace(dx, dy);											// Update the handle area location
// 					ref.x += dx;												// Update reference point location
// 					ref.y += dy;												// Update reference point location

// 					callCallback(onChange);
// 				}
// 			}
// 		}
// 		else
// 		{

// 		}
// 	}
// 	else
// 	{
// 		if(isHeld)
// 		{
// 			isHeld = false; 			// If there are no touches then it can't be held
// 			callCallback(onSetdown);
// 		}
// 	}
// }


// double	sftie_linear_slider::getPercent( void )
// {
// 	return 0;
// }