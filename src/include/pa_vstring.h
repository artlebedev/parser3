/*
  $Id: pa_vstring.h,v 1.9 2001/03/06 17:03:17 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_value.h"

class VString : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "string"; }
	// value: value
	String *get_string() { return &value; };
	// value: value
	double get_double() { return atof(value.cstr()); }
	// value: empty or not
	bool get_bool() { return value.size()!=0; };

public: // usage

	VString(Pool& apool) : Value(apool), 
		value(*new(apool) String(apool)) {
	}

	VString(String& avalue) : Value(avalue.pool()), 
		value(avalue) {
	}

private:
	String& value;

};

#endif
