/*
  $Id: pa_vdouble.h,v 1.9 2001/03/08 17:14:51 paf Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "pa_value.h"
#include "pa_common.h"

#define MAX_DOUBLE_AS_STRING 20

class VDouble : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "double"; }

	// double: fdouble
	String *get_string() {
		char *buf=static_cast<char *>(pool().malloc(MAX_DOUBLE_AS_STRING));
		snprintf(buf, MAX_DOUBLE_AS_STRING, "%g", fdouble);
		String *result=NEW String(pool());
		result->APPEND_CONST(buf);
		return result;
	}
	// double: fdouble
	double get_double() { return fdouble; }
	// double: 0 or !0
	bool get_bool() { return fdouble!=0; }

public: // usage

	VDouble(Pool& apool, double adouble) : Value(apool), 
		fdouble(adouble) {
	}

private:

	double fdouble;

};

#endif
