/*
  $Id: pa_vbool.h,v 1.6 2001/03/09 08:19:47 paf Exp $
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

#include "pa_value.h"
#include "pa_common.h"

#define MAX_BOOL_AS_STRING 20

class VBool : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "bool"; }
	// bool: this
	Value *get_expr_result() { return this; }
	// bool: fbool
	bool get_bool() { return fbool; }
	// bool: fbool
	double get_double() { return fbool; }

public: // usage

	VBool(Pool& apool, bool abool) : Value(apool), 
		fbool(abool) {
	}

private:

	bool fbool;

};

#endif
