/*
  $Id: pa_vunknown.h,v 1.6 2001/03/08 17:08:13 paf Exp $
*/

#ifndef PA_VUNKNOWN_H
#define PA_VUNKNOWN_H

#include "pa_value.h"

class VUnknown : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "unknown"; }
	// clone
	Value *clone() const { return NEW VUnknown(pool()); };

	// unknown: ""
	String *get_string() { return &empty; }
	// unknown: false
	bool get_defined() { return false; }
	// unknown: false
	bool get_bool() { return false; }

public: // usage

	VUnknown(Pool& apool) : Value(apool),
		empty(apool) {
	}

private:
	String empty;

};

#endif
