/** @file
	Parser: @b double parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdouble.h,v 1.25 2001/09/24 14:34:25 parser Exp $
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

extern Methoded *double_class;

/// value of type 'double'. implemented with @c double
class VDouble : public VStateless_object {
public: // Value

	const char *type() const { return "double"; }
	/// VDouble: clone
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VDouble(pool(), fdouble); }

	/// VDouble: fdouble
	const String *get_string() {
		char *buf=(char *)pool().malloc(MAX_NUMBER);
		String *result=NEW String(pool());
		result->APPEND_CLEAN(
			buf, snprintf(buf, MAX_NUMBER, "%g", fdouble), 
			name().origin().file, name().origin().line);
		return result;
	}
	/// VDouble: fdouble
	double as_double() const { return fdouble; }
	/// VDouble: fdouble
	int as_int() const { return (int)fdouble; }
	/// VDouble: 0 or !0
	bool as_bool() const { return fdouble!=0; }

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

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
