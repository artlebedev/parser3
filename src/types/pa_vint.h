/** @file
	Parser: @b int parser class decl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VINT_H
#define PA_VINT_H

#define IDENT_PA_VINT_H "$Id: pa_vint.h,v 1.63 2026/01/06 16:36:39 moko Exp $"

// include

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VINT_TYPE "int"

extern Methoded* int_class;

/// value of type 'int'. implemented with @c int
class VInt: public VSimple_stateless_object {
public: // Value

	override const char* type() const { return VINT_TYPE; }
	override VStateless_class *get_class() { return int_class; }

	/// VInt: true
	override bool is_evaluated_expr() const { return true; }
	/// VInt: clone
	override Value& as_expr_result() { return *new VInt(finteger); }

	/// VInt: finteger
	override const String* get_string() {
		return new String(pa_itoa(finteger));
	}
	/// VInt: finteger
	override double as_double() const { return finteger; }
	/// VInt: finteger
	override int as_int() const { return check4int(finteger); }
	/// VInt: finteger
	override pa_wint as_wint() const { return finteger; }
	/// VInt: 0 or !0
	override bool as_bool() const { return finteger!=0; }
	/// VInt: json-string
	override const String* get_json_string(Json_options&) { return get_string(); }

public: // usage

	VInt(pa_wint ainteger): finteger(ainteger) {}

	pa_wint get_int() { return finteger; }
	void set_int(pa_wint ainteger) { finteger=ainteger; }

	void inc(double increment) { finteger=clip2wint(finteger+increment); }
	
	void mul(double k) { finteger=clip2wint(finteger*k); }
	
	void div(double d) {
		if(d == 0)
			throw Exception("number.zerodivision", 0, "Division by zero");
		finteger=clip2wint(finteger/d);
	}
	
	void mod(pa_wint d) {
		if(d == 0)
			throw Exception("number.zerodivision", 0, "Modulus by zero");
		finteger%=d;
	}

private:

	pa_wint finteger;

};

#endif
