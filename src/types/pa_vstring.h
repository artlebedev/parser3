/**	@file
	Parser: @b string parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

static const char* IDENT_VSTRING_H="$Date: 2002/08/12 10:32:54 $";

#include <stdlib.h>

#include "pa_vstateless_object.h"
#include "pa_vdouble.h"

extern Methoded *string_class;

/// value of type 'string'. implemented with @c String
class VString : public VStateless_object {
public: // Value

	const char *type() const { return "string"; }
	VStateless_class *get_class() { return string_class; }
	/// VString: eq ''=false, ne ''=true
	bool is_defined() const { return !fstring->is_empty(); }
	/// VString: 0 or !0
	bool as_bool() const { return as_double()!=0; }
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
	const String *get_string() { return fstring; };
	/// VString: fstring
	double as_double() const { return fstring->as_double(); }
	/// VString: fstring
	int as_int() const { return fstring->as_int(); }

	/// VString: vfile
	VFile *as_vfile(String::Untaint_lang lang=String::UL_UNSPECIFIED,
		bool origins_mode=false);

	/// VString: $method
	Value *get_element(const String& name) {
		// $method
		if(Value *result=VStateless_object::get_element(name))
			return result;

		// bad $string.field
		bark("(%s) does not have fields",
			"%s method not found", &name);  return 0;
	}

public: // usage

/*	VString(Pool& apool) : VStateless_object(apool), 
		fstring(new(apool) String(apool)) {
	}
*/
	VString(const String& avalue) : VStateless_object(avalue.pool()),
		fstring(&avalue) {
	}

	const String& string() const { return *fstring; }
	void set_string(const String& astring) { fstring=&astring; }

private:
	const String *fstring;

};

#endif
