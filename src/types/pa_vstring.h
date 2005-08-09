/**	@file
	Parser: @b string parser class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

static const char * const IDENT_VSTRING_H="$Date: 2005/08/09 08:14:56 $";

// includes

#include "pa_vstateless_object.h"
#include "pa_vdouble.h"

// externs

extern Methoded* string_class;

/// value of type 'string'. implemented with @c String
class VString: public VStateless_object {
public: // Value

	override const char* type() const { return "string"; }
	override VStateless_class *get_class() { return string_class; }
	/// VString: eq ''=false, ne ''=true
	override bool is_defined() const { return !fstring->is_empty(); }
	/// VString: 0 or !0
	override bool as_bool() const { return as_double()!=0; }
	/// VString: true
	override bool is_string() const { return true; }
	/// VString: fstring as VDouble or this depending on return_string_as_is
	override Value& as_expr_result(bool return_string_as_is=false) { 
		if(return_string_as_is)
			return *this;
		else
			return *new VDouble(as_double());
	}
	/// VString: fstring
	override const String* get_string() { return fstring; };
	/// VString: fstring
	override double as_double() const { return fstring->as_double(); }
	/// VString: fstring
	override int as_int() const { return fstring->as_int(); }

	/// VString: vfile
	override VFile* as_vfile(String::Language lang=String::L_UNSPECIFIED,
		const Request_charsets *charsets=0);

	/// VString: $method
	override Value* get_element(const String& aname, Value& aself, bool looking_up) {
		// $method
		if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
			return result;

		// bad $string.field
		return bark("%s method not found", &aname);  
	}

public: // usage

	VString(): fstring(new String) {}
	VString(const String& avalue): fstring(&avalue) {}

	const String& string() const { return *fstring; }
	void set_string(const String& astring) { fstring=&astring; }

private:
	const String* fstring;

};

#endif
