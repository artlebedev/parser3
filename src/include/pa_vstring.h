/*
  $Id: pa_vstring.h,v 1.13 2001/03/09 04:47:27 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_value.h"
#include "pa_vobject.h"
#include "classes/_string.h"

class VString : public VObject {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "string"; }
	// value: fvalue
	String *get_string() { return &fvalue; };
	// value: fvalue
	double get_double() { return atof(fvalue.cstr()); }
	// value: empty or not
	bool get_bool() { return fvalue.size()!=0; };

public: // usage

	VString() : VObject(*string_class), 
		fvalue(*new(string_class->pool()) String(string_class->pool())) {
	}

	VString(String& avalue) : VObject(*string_class),
		fvalue(avalue) {
	}

private:
	String& fvalue;

};

#endif
