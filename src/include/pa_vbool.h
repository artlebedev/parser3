/*
  $Id: pa_vbool.h,v 1.2 2001/03/08 12:19:19 paf Exp $
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
	// bool: fvalue
	String *get_string() {
		String *result=NEW String(pool());
		result->APPEND_CONST(fvalue?"TRUE":"FALSE");
		return result;
	};
	// bool: fvalue
	bool get_bool() { return fvalue; };
	// bool: fvalue
	double get_double() { return fvalue; };

public: // usage

	VBool(Pool& apool, bool avalue/*=false*/) : Value(apool), 
		fvalue(avalue) {
	}

private:

	bool fvalue;

};

#endif
