/** @file
	Parser: @b nothing parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vnothing.h,v 1.1 2001/05/21 17:11:57 parser Exp $
*/

#ifndef PA_VNOTHING_H
#define PA_VNOTHING_H

#include "pa_vstateless_object.h"
#include "pa_globals.h"

extern Methoded *nothing_class;

/// value of type 'nothing'. ex: usually $sjfklsjfksjdfk has this type
class VNothing : public VStateless_object {
public: // Value

	const char *type() const { return "nothing"; }

	/// VNothing: ""
	const String *get_string() { return empty_string; }
	/// VNothing: false
	bool is_defined() const { return false; }
	/// VNothing: 0
	double as_double() { return 0; }
	/// VNothing: 0
	int as_int() { return 0; }
	/// VNothing: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

	/// VNothing: false
	bool as_bool() { return false; }

	/// VNothing: methods
	Value *get_element(const String& name) {
		// methods
		if(Value *result=VStateless_object::get_element(name))
			return result;

		return NEW VNothing(pool());
	}

public: // usage

	VNothing(Pool& apool) : VStateless_object(apool, *nothing_class) {
	}

};

#endif
