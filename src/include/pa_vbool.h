/*
  $Id: pa_vbool.h,v 1.4 2001/03/08 16:54:25 paf Exp $
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

#include "pa_value.h"
#include "pa_common.h"
#include "pa_bool.h"

#define MAX_BOOL_AS_STRING 20

class VBool : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "bool"; }
	// clone
	Value *clone() const { return NEW VBool(fbool); }
	// bool: fbool
	bool get_bool() { return fbool.value; }
	// bool: fbool
	double get_double() { return fbool.value; }

public: // usage

	VBool(Bool& abool) : Value(abool.pool()), 
		fbool(abool) {
	}

private:

	Bool& fbool;

};

#endif
