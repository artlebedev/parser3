/**	@file
	Parser: @b string parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vstring.h,v 1.43 2002/04/18 14:35:13 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_vstateless_object.h"
#include "pa_vdouble.h"

extern Methoded *string_class;

/**	the string object of string class. 
	specialized light version for VString
	
	"of some class" means "with some set of methods".
*/
class VStateless_string_object : public VAliased {
public: // Value
	
	/// VStateless_string_object: fclass_real
	VStateless_class *get_class() { return string_class; }

	/// VStateless_string_object: +$method
	Value *get_element(const String& name) {
		// $CLASS << actually abscent due to VString::hide_class=true, but for possible future VAliased::get_element implementation changes...
		if(Value *result=VAliased::get_element(name))
			return result;

		// $method=junction(self+class+method)
		if(Junction *junction=string_class->get_junction(*this, name))
			return NEW VJunction(*junction);

		return 0;
	}

public: // creation

	VStateless_string_object(Pool& apool) : VAliased(apool) {}

};


/// value of type 'string'. implemented with @c String
class VString : public VStateless_string_object {
public: // Value

	const char *type() const { return "string"; }
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
		if(Value *result=VStateless_string_object::get_element(name))
			return result;

		// bad $string.field
		bark("(%s) does not have fields",
			"%s method not found", &name);  return 0;
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

/*	VString(Pool& apool) : VStateless_string_object(apool), 
		fstring(new(apool) String(apool)) {
	}
*/
	VString(const String& avalue) : VStateless_string_object(avalue.pool()),
		fstring(&avalue) {
	}

	const String& string() { return *fstring; }
	void set_string(const String& astring) { fstring=&astring; }

private:
	const String *fstring;

};

#endif
