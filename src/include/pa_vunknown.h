/*
  $Id: pa_vunknown.h,v 1.1 2001/02/23 17:12:57 paf Exp $
*/

#ifndef PA_VUNKNOWN_H
#define PA_VUNKNOWN_H

#include "pa_value.h"

class VUnknown : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "Unknown"; }

	// unknown: ""
	virtual String *get_string() { return &empty; }

public: // usage

	VUnknown(Pool& apool) : Value(apool),
		empty(apool) {
	}

private:
	String empty;

};

#endif
