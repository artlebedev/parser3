/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstring.h,v 1.12 2001/03/26 08:27:28 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_vstateless_object.h"
#include "pa_vdouble.h"
#include "_string.h"

class VString : public VStateless_object {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "string"; }
	// string: fstring as VDouble
	Value *get_expr_result() { return NEW VDouble(pool(), get_double()); }
	// string: fstring
	const String *get_string() { return &fstring; };
	// string: fstring
	double get_double() { 
		double result;
		const char *cstr=fstring.cstr();
		char *error_pos=0;
		// 0xABC
		if(cstr[0]=='0' && (cstr[1]=='x' || cstr[1]=='X'))
			result=(double)(unsigned long)strtol(cstr, &error_pos, 0);
		else
			result=strtod(cstr, &error_pos);

		if(error_pos&&*error_pos)
			THROW(0, 0,
				&fstring,
				"invalid number");

		return result;
	}

	// string: $CLASS,$BASE,$method
	Value *get_element(const String& name) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_object::get_element(name))
			return result;

		// bad $string.field
		bark("(%s) does not have fields");  return 0;
	}

protected: // VAliased

	VStateless_class *get_class_alias() { return 0; }

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
