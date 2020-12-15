/** @file
	Parser: @b double parser class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VDOUBLE_H
#define PA_VDOUBLE_H

#define IDENT_PA_VDOUBLE_H "$Id: pa_vdouble.h,v 1.71 2020/12/15 17:10:39 moko Exp $"

// includes

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VDOUBLE_TYPE "double"

// double validation defines

#ifdef _MSC_VER
#include <float.h>
#define pa_isnan(d) _isnan(d)
#define pa_finite(d) _finite(d)
#else
#define pa_isnan(d) isnan(d)
#ifdef HAVE_ISFINITE
#define pa_finite(d) isfinite(d)
#else
#define pa_finite(d) finite(d)
#endif
#endif

// externs

extern Methoded* double_class;

/// value of type 'double'. implemented with @c double
class VDouble: public VStateless_object {
public: // Value

	override const char* type() const { return VDOUBLE_TYPE; }
	override VStateless_class *get_class() { return double_class; }

	/// VDouble: true
	override bool is_evaluated_expr() const { return true; }
	/// VDouble: clone
	override Value& as_expr_result() { return *new VDouble(fdouble); }

	/// VDouble: fdouble
	override const String* get_string() {
		char local_buf[MAX_NUMBER];
		size_t length=snprintf(local_buf, MAX_NUMBER, "%.15g", fdouble);
		return new String(pa_strdup(local_buf, length));
	}
	/// VDouble: fdouble
	override double as_double() const { return fdouble; }
	/// VDouble: fdouble
	override int as_int() const { return get_int(); }
	/// VDouble: 0 or !0
	override bool as_bool() const { return fdouble!=0; }
	/// VInt: json-string
	override const String* get_json_string(Json_options&) { return get_string(); }

public: // usage

	VDouble(double adouble): fdouble(adouble == 0 ? 0 : adouble) {
		if(!pa_finite(adouble))
			throw Exception("number.format", 0, pa_isnan(adouble) ? "invalid number (double)" : "out of range (double)");
	}

	int get_int() const { return (int)trunc(fdouble); }
	double get_double() const { return fdouble; }

	void inc(double increment) { fdouble+=increment; }
	
	void mul(double k) { fdouble*=k; }
	
	void div(double d) {
		if(d == 0)
			throw Exception("number.zerodivision", 0, "Division by zero");
		fdouble/=d; 
	}
	
	void mod(int d) {
		if(d == 0)
			throw Exception("number.zerodivision", 0, "Modulus by zero");
		fdouble=((int)fdouble)%d;
	}

private:

	double fdouble;

};

#endif
