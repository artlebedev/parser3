/*
  $Id: pa_vdouble.h,v 1.2 2001/03/06 14:09:35 paf Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "pa_value.h"
#include "pa_common.h"

#define MAX_DOUBLE_STRING 20

class VDouble : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "Double"; }
	// double: value
	String *get_string() {
		char *buf=static_cast<char *>(pool().malloc(MAX_DOUBLE_STRING));
		snprintf(buf, MAX_DOUBLE_STRING, "%f", value);
		String *result=NEW String(pool());
		result->APPEND_CONST(buf);
		return result;
	};
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
