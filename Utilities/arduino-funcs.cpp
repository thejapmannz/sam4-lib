/*
 * arduino_funcs.cpp
 * Copied from Arduino libraries...
 * Created: 25/05/2016 4:06:06 PM
 *  Author: Ben Jones
 */ 

long ardu_map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long ardu_constrain(const long x, const long a, const long b) {
	if(x < a) {
		return a;
	}
	else if(b < x) {
		return b;
	}
	else
	return x;
}