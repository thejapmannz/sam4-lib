/*
 * arduino_funcs.cpp
 * Copied from Arduino libraries. Uses templates so functions 
 *   can be applied to all types with full accuracy.
 *
 * Created: 25/05/2016 4:06:06 PM
 *  Author: Ben Jones
 */ 

template <typename num>
inline num ardu_map(num x, num in_min, num in_max, num out_min, num out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template <typename num>
inline num ardu_min(num x, num y)
{
	return x < y ? x : y;
}

template <typename num>
inline num ardu_max(num x, num y)
{
	return x > y ? x : y;
}


template <typename num>
inline num ardu_constrain(num x, num xmin, num xmax) {
	return ardu_max(xmin, ardu_min(x, xmax));
}

