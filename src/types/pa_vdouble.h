/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdouble.h,v 1.5 2001/03/12 12:00:07 paf Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "pa_vstateless_object.h"
#include "pa_common.h"
#include "_double.h"

#define MAX_DOUBLE_AS_STRING 20

class VDouble : public VStateless_object {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "double"; }
	// double: this
	Value *get_expr_result() { return this; }

	// double: fdouble
	const String *get_string() {
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

	VDouble(Pool& apool, double adouble) : VStateless_object(apool, *double_class), 
		fdouble(adouble) {
	}

	void inc(double increment) { fdouble+=increment; }

private:

	double fdouble;

};

#endif
