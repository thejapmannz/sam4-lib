/*
 * arduino_funcs.h
 * Copied from Arduino libraries...
 * Created: 25/05/2016 4:06:06 PM
 *  Author: Ben Jones
 */ 


#ifndef ARDUINO_FUNCS_H_
#define ARDUINO_FUNCS_H_

long ardu_map(long x, long in_min, long in_max, long out_min, long out_max);
long ardu_constrain(const long x, const long a, const long b);

#include "arduino-funcs.cpp"

#endif /* ARDUINO_FUNCS_H_ */