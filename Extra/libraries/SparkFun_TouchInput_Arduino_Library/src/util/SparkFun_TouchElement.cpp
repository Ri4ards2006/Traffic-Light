/*

SparkFun_TouchElement.cpp
Header: SparkFun_TouchElement.h

Purpose:

Author: Owen Lyke

*/

#include "SparkFun_TouchElement.h"


sfti_element::sfti_element( void )
{
	pprev = NULL;
	pnext = NULL;
}

void sfti_element::call(void (*func)( void ))
{
	if(func != NULL)	// Safely call the function
	{
		(*func)();
	}
}

bool sfti_element::isFreeElement( void )
{
	if((pnext == NULL) && (pprev == NULL))	// If we mess with an element that has a valid pnext value then it could break whatever list it is currently in
	{
		return true;
		// if(pelem->pprev != NULL)	// It is, however, POSSIBLE to take the very last element of a list and switch it over. However let's not do that cause it's weird
		// {
		// 	pelem->pprev->pnext = NULL;
		// }
	}
	return false;
}






// Button
sfti_element_button::sfti_element_button(sf2drt_polygon& basepoly, void (*func_pressed)( void ), void (*func_released)( void )) : sfti_element()
{
	pressed = func_pressed;
	released = func_released;

	isPressed = false;
	poly = basepoly;
}

void sfti_element_button::setPressedCallback(void (*func)( void ))
{
	pressed = func;
}

void sfti_element_button::setReleasedCallback(void (*func)( void ))
{
	released = func;
}

void sfti_element_button::processRecord(sfti_record rec)
{
	bool pressedNow = false;

	for(sfti_mxtouch_t indi = 0; indi < rec.numTouches; indi++)
	{
		sf2drt_coordinate_t ctemp;
		ctemp.x = (sf2drt_extent_t)((rec.pcoords + indi)->x);
		ctemp.y = (sf2drt_extent_t)((rec.pcoords + indi)->y);

		if(poly.contains(ctemp))
		{
			pressedNow = true;
		}
	}

	if((!isPressed) && (pressedNow))
	{
		call(pressed);
	}

	if((isPressed) && (!pressedNow))
	{
		call(released);
	}

	isPressed = pressedNow;
}



// Handle
sfti_element_handle::sfti_element_handle( sf2drt_polygon& basePoly, void (*func_pickedup)( void ), void (*func_setdown)( void ),  void (*func_moved)( void )) : sfti_element()
{
	pickedUp = func_pickedup;
	setDown = func_setdown;
	moved = func_moved;

	isHeld = false;
	poly = basePoly;
}

void sfti_element_handle::setPickedUpCallback(void (*func)( void ))
{
	pickedUp = func;
}

void sfti_element_handle::setSetDownCallback(void (*func)( void ))
{
	setDown = func;
}

void sfti_element_handle::setMovedCallback(void (*func)( void ))
{
	moved = func;
}

void sfti_element_handle::processRecord(sfti_record rec)
{
		bool heldNow = false;
		sf2drt_coordinate_t nearest;

		
		sf2drt_coordinate_t tempcom = poly.getCOM();	// Get the center of mass of the handle
		com.x = (sfti_extent_t)tempcom.x;
		com.y = (sfti_extent_t)tempcom.y;

		if(get_nearest_coord( rec, &nearest ))
		{

			if(poly.contains(nearest))
			{
				// Be aware that this is not the most general or best way to do this.
				// This method works well for simple and convex shapes but not for strange shapes
				// If there was a C-shaped polygon then the average location of the vertices would not be contained. Anyhow this section is open to improvements!
				heldNow = true;
			}


			if(isHeld && heldNow)
			{
				// If the handle was previously held then we should move the handle to simulate dragging
				// There are several ways to do this such as:
				// 1 - compute how far the finger has moved since last time (including some method of trying to make sure it was the same finger)
				// 2 - simply move the COM to the nearest touch (much easier and still acceptable)
				// 3 - surely other ways exist too
				sf2drt_diff_t dx = (sf2drt_diff_t)(nearest.x) - (sf2drt_diff_t)(com.x);
				sf2drt_diff_t dy = (sf2drt_diff_t)(nearest.y) - (sf2drt_diff_t)(com.y);

				poly.displace(dx, dy);

				call(moved);
			}


			if(heldNow)
			{
				if(!isHeld)
				{
					call(pickedUp);
				}

				// Move heldNow into isHeld so that next time we might move the handle
				isHeld = true;
			}
			else
			{
				if(isHeld)
				{
					call(setDown);
				}

				isHeld = false;
			}
		}
		else
		{	
			// No touches are present on the screen
			if(isHeld)
			{
				call(setDown);
			}

			isHeld = false;
		}
}

bool sfti_element_handle::get_nearest_coord( sfti_record rec, sf2drt_coordinate_t* pcoord )
{
	if(rec.numTouches)
	{
		sfti_coord_t sfticom;			
		sfticom.x = (double)com.x;
		com.y = (double)com.y;

		sfti_coord_t touch;

		double rad = 0;
		double minRad = 0;

		sfti_mxtouch_t indi = 0;
		touch = *(rec.pcoords + indi);
		rad = sqrt( pow( (touch.x - sfticom .x), 2) + pow( (touch.y - sfticom .y), 2) );
		minRad = rad;
		pcoord->x = (sf2drt_extent_t)(round(touch.x));
		pcoord->y = (sf2drt_extent_t)(round(touch.y));

		for(indi = 1; indi < rec.numTouches; indi++)
		{
			touch = *(rec.pcoords + indi);
			rad = sqrt( pow( (touch.x - sfticom .x), 2) + pow( (touch.y - sfticom .y), 2) );

			if(rad < minRad)
			{
				pcoord->x = (sf2drt_extent_t)(round(touch.x));
				pcoord->y = (sf2drt_extent_t)(round(touch.y));
			}
		}
		return true;	// If ther's a touch then there's a nearest touch
	}
	return false; // No touches, no nearest touch
}



// Slider
sfti_element_slider::sfti_element_slider( void ) : sfti_element()
{

}

void sfti_element_slider::processRecord(sfti_record rec)
{
	// Serial.println("Itsa me, a slider");
	// Serial.println("Record information printed through the slider:");
	// rec.printInfo();
}








// Debugger (prints info for touch records)
sfti_element_debugger::sfti_element_debugger(HardwareSerial* port) : sfti_element()
// sfti_element_debugger::sfti_element_debugger(HardwareSerial& port) : sfti_element()
{
	_debugPort = port;
	// _debugPort = &port;
}

void sfti_element_debugger::processRecord(sfti_record rec)
{
	if(_debugPort != NULL)
	{
		Serial.print("Debugger output: ");
		// _debugPort->print("valid = "); _debugPort->print(valid); // _debugPort->println();
		// _debugPort->print("dynamic = "); _debugPort->print(dynamic); // _debugPort->println();
		// _debugPort->print("maxTouches = "); _debugPort->print(maxTouches); // _debugPort->println();
		// _debugPort->print("numTouches = "); _debugPort->print(numTouches); // _debugPort->println();
		// _debugPort->print("value of pcoords = 0x"); _debugPort->print((uint32_t)pcoords, HEX); // _debugPort->println();
		_debugPort->print("timestamp = "); _debugPort->print(rec.timestamp); _debugPort->print(" "); // _debugPort->println();
		if(rec.numTouches)
		{
			_debugPort->print("Coordinate info: ");
			_debugPort->print("Touch");
			for(sfti_mxtouch_t indi = 0; indi < rec.numTouches; indi++)
			{
				_debugPort->print(" #");
				_debugPort->print(indi);
				_debugPort->print(", {");
				_debugPort->print((rec.pcoords+indi)->x);
				_debugPort->print(", ");
				_debugPort->print((rec.pcoords+indi)->y);
				_debugPort->print("}, ");
			}
		}
		_debugPort->println();
	}
}
	
