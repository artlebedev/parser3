/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstring.h,v 1.2 2001/03/11 08:16:38 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_value.h"
#include "pa_vobject.h"
#include "pa_vdouble.h"
#include "_string.h"

class VString : public VObject {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "string"; }
	// string: fvalue as VDouble
	Value *get_expr_result() { return NEW VDouble(pool(), get_double()); }
	// string: fvalue
	const String *get_string() { return &fvalue; };
	// string: fvalue
	double get_double() { return atof(fvalue.cstr()); }
	// string: empty or not
	bool get_bool() { return fvalue.size()!=0; };

public: // usage

	VString(Pool& apool) : VObject(apool, *string_class), 
		fvalue(*new(string_class->pool()) String(string_class->pool())) {
	}

	VString(const String& avalue) : VObject(avalue.pool(), *string_class),
		fvalue(avalue) {
	}

	const String& value() { return fvalue; }

private:
	const String& fvalue;

};

#endif
