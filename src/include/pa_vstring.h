/*
  $Id: pa_vstring.h,v 1.14 2001/03/09 08:19:48 paf Exp $
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#include <stdlib.h>

#include "pa_value.h"
#include "pa_vobject.h"
#include "pa_vdouble.h"
#include "classes/_string.h"

class VString : public VObject {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "string"; }
	// string: fvalue as VDouble
	Value *get_expr_result() { return NEW VDouble(pool(), get_double()); }
	// string: fvalue
	String *get_string() { return &fvalue; };
	// string: fvalue
	double get_double() { return atof(fvalue.cstr()); }
	// string: empty or not
	bool get_bool() { return fvalue.size()!=0; };

public: // usage

	VString(Pool& apool) : VObject(apool, *string_class), 
		fvalue(*new(string_class->pool()) String(string_class->pool())) {
	}

	VString(String& avalue) : VObject(avalue.pool(), *string_class),
		fvalue(avalue) {
	}

private:
	String& fvalue;

};

#endif
