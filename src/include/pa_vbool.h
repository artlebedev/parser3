/*
  $Id: pa_vbool.h,v 1.1 2001/03/06 15:55:46 paf Exp $
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
	// bool: value
	String *get_string() {
		String *result=NEW String(pool());
		result->APPEND_CONST(value?"TRUE":"FALSE");
		return result;
	};
	// bool: value
	bool get_bool() { return value; };
	// bool: value
	double get_double() { return value; };

public: // usage

	VBool(Pool& apool, bool avalue/*=false*/) : Value(apool), 
		value(avalue) {
	}

private:

	bool value;

};

#endif
