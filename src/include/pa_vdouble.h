/*
  $Id: pa_vdouble.h,v 1.7 2001/03/08 16:54:25 paf Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "pa_value.h"
#include "pa_common.h"
#include "pa_double.h"

#define MAX_DOUBLE_AS_STRING 20

class VDouble : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "double"; }
	// clone
	Value *clone() { return NEW VDouble(fdouble); }

	// double: fdouble
	String *get_string() {
		char *buf=static_cast<char *>(pool().malloc(MAX_DOUBLE_AS_STRING));
		snprintf(buf, MAX_DOUBLE_AS_STRING, "%g", fdouble);
		String *result=NEW String(pool());
		result->APPEND_CONST(buf);
		return result;
	}
	// double: fdouble
	double get_double() { return fdouble.value; }
	// double: 0 or !0
	bool get_bool() { return fdouble.value!=0; }

public: // usage

	VDouble(Double& adouble) : Value(adouble.pool()), 
		fdouble(adouble) {
	}

private:

	Double& fdouble;

};

#endif
