/** @file
	Parser: unknown Parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vunknown.h,v 1.7 2001/03/28 14:41:33 paf Exp $
*/

#ifndef PA_VUNKNOWN_H
#define PA_VUNKNOWN_H

#include "pa_value.h"
#include "pa_globals.h"

/// value of type 'unknown'. ex: usually $sjfklsjfksjdfk has this type
class VUnknown : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "unknown"; }

	// unknown: ""
	const String *get_string() { return empty_string; }
	// unknown: false
	bool is_defined() { return false; }
	// VUnknown: 0
	double as_double() { return 0; }
	// unknown: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

	// unknown: false
	bool as_bool() { return false; }

public: // usage

	VUnknown(Pool& apool) : Value(apool) {
	}

};

#endif
