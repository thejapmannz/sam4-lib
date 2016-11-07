/*
 * arduino_funcs.hpp
 * Copied from Arduino libraries. Uses templates so functions 
 *   can be applied to all types with full accuracy.
 *
 * Created: 25/05/2016 4:06:06 PM
 *  Author: Ben Jones
 */ 


#ifndef ARDUINO_FUNCS_HPP_
#define ARDUINO_FUNCS_HPP_

template <typename num>
inline num ardu_map(num x, num in_min, num in_max, num out_min, num out_max);

template <typename num>
inline num ardu_min(num x, num y);

template <typename num>
inline num ardu_max(num x, num y);

template <typename num>
inline num ardu_constrain(num x, num xmin, num xmax);

#include "arduino-funcs.cpp"

#endif /* ARDUINO_FUNCS_HPP_ */