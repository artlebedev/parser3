/*
  $Id: pa_vdouble.h,v 1.6 2001/03/08 12:19:19 paf Exp $
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
	// double: fvalue
	String *get_string() {
		char *buf=static_cast<char *>(pool().malloc(MAX_DOUBLE_AS_STRING));
		snprintf(buf, MAX_DOUBLE_AS_STRING, "%g", fvalue);
		String *result=NEW String(pool());
		result->APPEND_CONST(buf);
		return result;
	};
	// double: fvalue
	double get_double() { return fvalue; };
	// double: 0 or !0
	bool get_bool() { return fvalue!=0; };

public: // usage

	VDouble(Pool& apool, double avalue=0) : Value(apool), 
		fvalue(avalue) {
	}

private:

	double fvalue;

};

#endif
