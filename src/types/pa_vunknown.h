/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vunknown.h,v 1.4 2001/03/18 20:31:30 paf Exp $
*/

#ifndef PA_VUNKNOWN_H
#define PA_VUNKNOWN_H

#include "pa_value.h"
#include "pa_globals.h"

class VUnknown : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "unknown"; }

	// unknown: ""
	const String *get_string() { return empty_string; }
	// unknown: false
	bool get_defined() { return false; }
	// unknown: this
	Value *get_expr_result() { return this; }
	// unknown: false
	bool get_bool() { return false; }

public: // usage

	VUnknown(Pool& apool) : Value(apool) {
	}

};

#endif
