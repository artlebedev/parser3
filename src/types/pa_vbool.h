/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vbool.h,v 1.4 2001/03/19 16:57:24 paf Exp $
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

#include "pa_common.h"
#include "pa_value.h"

#define MAX_BOOL_AS_STRING 20

/// VBool
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
