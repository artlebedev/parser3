/*
  $Id: pa_vstring.h,v 1.1 2001/02/22 09:36:18 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include "pa_value.h"

class VString : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	/*virtual*/ const char *type() const { return "VString"; }
	// string: value
	/*virtual*/ String *get_string() { return string; };
	// string: value
	void put_string(String *astring) { string=astring; }

public: // usage

	VString(String *astring) : Value(astring->pool()), 
		string(astring) {
	}

private:
	String *string;

};

#endif
