/*
  $Id: pa_vunknown.h,v 1.3 2001/03/06 15:30:48 paf Exp $
*/

#ifndef PA_VUNKNOWN_H
#define PA_VUNKNOWN_H

#include "pa_value.h"

class VUnknown : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "unknown"; }

	// unknown: ""
	String *get_string() { return &empty; }
	bool get_bool() { return false; }

public: // usage

	VUnknown(Pool& apool) : Value(apool),
		empty(apool) {
	}

private:
	String empty;

};

#endif
