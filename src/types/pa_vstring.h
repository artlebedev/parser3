/**	@file
	Parser: @b string parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstring.h,v 1.30 2001/08/10 10:24:09 parser Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_vstateless_object.h"
#include "pa_vdouble.h"

extern Methoded *string_class;

/// value of type 'string'. implemented with @c String
class VString : public VStateless_object {
public: // Value

	const char *type() const { return "string"; }
	/// - VString: eq ''=false, ne ''=true
	bool is_defined() const { return size()!=0; }
	/// VString: true
	bool is_string() const { return true; }
	/// VString: fstring as VDouble or this depending on return_string_as_is
	Value *as_expr_result(bool return_string_as_is=false) { 
		if(return_string_as_is)
			return this;
		else
			return NEW VDouble(pool(), as_double()); 
	}
	/// VString: fstring
	const String *get_string() { return &fstring; };
	/// VString: fstring
	double as_double() { return fstring.as_double(); }
	/// VString: fstring
	int as_int() { return fstring.as_int(); }

	/// VString: vfile
	VFile *as_vfile(String::Untaint_lang lang=String::UL_UNSPECIFIED,
		bool origins_mode=false);

	/// VString: $method
	Value *get_element(const String& name) {
		// $method
		if(Value *result=VStateless_object::get_element(name))
			return result;

		// bad $string.field
		bark("(%s) does not have fields");  return 0;
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VString(Pool& apool) : VStateless_object(apool, *string_class), 
		fstring(*new(apool) String(apool)) {
	}

	VString(const String& avalue) : VStateless_object(avalue.pool(), *string_class),
		fstring(avalue) {
	}

	const String& string() { return fstring; }

private:
	const String fstring;

};

#endif
