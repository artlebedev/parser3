/*
  $Id: pa_vstring.h,v 1.7 2001/03/06 15:02:47 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_value.h"

class VString : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "string"; }
	// string: value
	String *get_string() { return &string; };
	// string: value
	//void put_string(String *astring) { string=astring; }

	// string: value
	double get_double() { return atof(string.cstr()); }

public: // usage

	VString(Pool& apool) : Value(apool), 
		string(*new(apool) String(apool)) {
	}

	VString(String& astring) : Value(astring.pool()), 
		string(astring) {
	}

private:
	String& string;

};

#endif
