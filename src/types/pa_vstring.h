/*
	Parser: string parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstring.h,v 1.20 2001/04/03 06:23:08 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_vstateless_object.h"
#include "pa_vdouble.h"
#include "_string.h"

/// value of type 'string'. implemented with @c String
class VString : public VStateless_object {
public: // Value

	/// all: for error reporting after fail(), etc
	const char *type() const { return "string"; }
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
	double as_double() { 
		double result;
		const char *cstr=fstring.cstr();
		char *error_pos=0;
		// 0xABC
		if(cstr[0]=='0' && (cstr[1]=='x' || cstr[1]=='X'))
			result=(double)(unsigned long)strtol(cstr, &error_pos, 0);
		else
			result=strtod(cstr, &error_pos);

		if(error_pos && *error_pos)
			THROW(0, 0,
				&fstring,
				"invalid number");

		return result;
	}

	/// VString: vfile
	const VFile *as_vfile() const;

	/// VString: $CLASS,$BASE,$method
	Value *get_element(const String& name) {
		// $CLASS,$BASE,$method
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
	const String& fstring;

};

#endif
