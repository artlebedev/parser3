/** @file
	Parser: double parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdouble.h,v 1.14 2001/03/27 16:35:56 paf Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "_double.h"

/// value of type 'double'. implemented with @c double
class VDouble : public VStateless_object {
public: // Value

	/// all: for error reporting after fail(), etc
	const char *type() const { return "double"; }
	/// VDouble: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

	/// VDouble: fdouble
	const String *get_string() {
		char *buf=(char *)pool().malloc(MAX_NUMBER);
		snprintf(buf, MAX_NUMBER, "%g", fdouble);
		return NEW String(pool(), buf);
	}
	/// VDouble: fdouble
	double as_double() { return fdouble; }
	/// VDouble: 0 or !0
	bool as_bool() { return fdouble!=0; }

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool are_static_calls_disabled() { return true; }

public: // usage

	VDouble(Pool& apool, double adouble) : VStateless_object(apool, *double_class), 
		fdouble(adouble) {
	}

	void inc(double increment) { fdouble+=increment; }
	void mul(double k) { fdouble*=k; }
	void div(double d) { fdouble/=d; }
	void mod(int d) { fdouble=((int)fdouble)%d; }

private:

	double fdouble;

};

#endif
