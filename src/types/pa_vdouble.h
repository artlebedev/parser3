/** @file
	Parser: @b double parser class decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

static const char * const IDENT_VDOUBLE_H="$Date: 2003/11/20 16:34:28 $";

// includes

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// externs

extern Methoded* double_class;

/// value of type 'double'. implemented with @c double
class VDouble: public VStateless_object {
public: // Value

	override const char* type() const { return "double"; }
	override VStateless_class *get_class() { return double_class; }

	/// VDouble: clone
	override Value& as_expr_result(bool ) { return *new VDouble(fdouble); }

	/** VDouble: fdouble 
	*/
	override const String* get_string() {
		char local_buf[MAX_NUMBER];
		size_t length=snprintf(local_buf, MAX_NUMBER, "%g", fdouble);
		return new String(strdup(local_buf, length), length);
	}
	/// VDouble: fdouble
	override double as_double() const { return fdouble; }
	/// VDouble: fdouble
	override int as_int() const { return (int)round(fdouble); }
	/// VDouble: 0 or !0
	override bool as_bool() const { return fdouble!=0; }

public: // usage

	VDouble(double adouble): fdouble(adouble) {}

	void inc(double increment) { fdouble+=increment; }
	void mul(double k) { fdouble*=k; }
	void div(double d) { fdouble/=d; }
	void mod(int d) { fdouble=((int)fdouble)%d; }

private:

	double fdouble;

};

#endif
