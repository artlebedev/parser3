/** @file
	Parser: @b unknown parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vunknown.h,v 1.12 2001/05/07 14:00:55 paf Exp $
*/

#ifndef PA_VUNKNOWN_H
#define PA_VUNKNOWN_H

#include "pa_vstateless_object.h"
#include "pa_globals.h"

extern Methoded *unknown_class;

/// value of type 'unknown'. ex: usually $sjfklsjfksjdfk has this type
class VUnknown : public VStateless_object {
public: // Value

	const char *type() const { return "unknown"; }

	/// VUnknown: ""
	const String *get_string() { return empty_string; }
	/// VUnknown: false
	bool is_defined() const { return false; }
	/// VUnknown: 0
	double as_double() { return 0; }
	/// VUnknown: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

	/// VUnknown: false
	bool as_bool() { return false; }

	/// VUnknown: methods
	Value *get_element(const String& name) {
		// methods
		if(Value *result=VStateless_object::get_element(name))
			return result;

		return NEW VUnknown(pool());
	}

public: // usage

	VUnknown(Pool& apool) : VStateless_object(apool, *unknown_class) {
	}

};

#endif
