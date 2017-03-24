/* 
 * samTC.cpp
 * Driver for the Timer-Counter peripheral. Allows a variety of 
 *     timer functions and techniques.
 * 
 * Created: 7/11/2016
 * Author: Benjamin Jones
 */


#include "sam.h"
#include "samClock.hpp"

samTC_c::samTC_c(int ID) {
	//Constructor - save ID and base.
	
	this->ID = ID;
	
	this->base = 
}