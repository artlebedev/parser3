/*
  $Id: pa_vstring.h,v 1.12 2001/03/08 17:14:51 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_value.h"

class VString : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "string"; }
	// fvalue: fvalue
	String *get_string() { return &fvalue; };
	// fvalue: fvalue
	double get_double() { return atof(fvalue.cstr()); }
	// fvalue: empty or not
	bool get_bool() { return fvalue.size()!=0; };

public: // usage

	VString(Pool& apool) : Value(apool), 
		fvalue(*new(apool) String(apool)) {
	}

	VString(String& avalue) : Value(avalue.pool()), 
		fvalue(avalue) {
	}

private:
	String& fvalue;

};

#endif
