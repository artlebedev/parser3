/*
  $Id: pa_vdouble.h,v 1.1 2001/03/06 12:22:57 paf Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "pa_value.h"

class VDouble : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "Double"; }
	// double: value
	double get_double() { return value; };

public: // usage

	VDouble(Pool& apool, double avalue=0) : Value(apool), 
		value(avalue) {
	}

private:
	double value;

};

#endif
