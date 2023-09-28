/** @file
	Parser: @b int parser class decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VINT_H
#define PA_VINT_H

#define IDENT_PA_VINT_H "$Id: pa_vint.h,v 1.57 2023/09/28 01:46:02 moko Exp $"

// include

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VINT_TYPE "int"

extern Methoded* int_class;

/// value of type 'int'. implemented with @c int
class VInt: public VStateless_object {
public: // Value

	override const char* type() const { return VINT_TYPE; }
	override VStateless_class *get_class() { return int_class; }

	/// VInt: true
	override bool is_evaluated_expr() const { return true; }
	/// VInt: clone
	override Value& as_expr_result() { return *new VInt(finteger); }

	/// VInt: finteger
	override const String* get_string() {
		char local_buf[MAX_NUMBER];
		size_t length=snprintf(local_buf, MAX_NUMBER, "%d", finteger);
		return new String(pa_strdup(local_buf, length));
	}
	/// VInt: finteger
	override double as_double() const { return as_int(); }
	/// VInt: finteger
	override int as_int() const { return finteger; }
	/// VInt: 0 or !0
	override bool as_bool() const { return finteger!=0; }
	/// VInt: json-string
	override const String* get_json_string(Json_options&) { return get_string(); }

	/// VInt: $method
	override Value* get_element(const String& aname) {
		// $method (CLASS, CLASS_NAME only if no OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL)
		if(Value* result=VStateless_object::get_element(aname))
			return result;

		// bad $int.field
#ifdef FEATURE_GET_ELEMENT4CALL
		return Value::get_element(aname);
	}

	override Value* get_element4call(const String& aname) {
		// $method
		if(Value* result=VStateless_object::get_element(aname))
			return result;
#endif
		return bark("%s method not found", &aname);
	}

public: // usage

	VInt(int ainteger): finteger(ainteger) {}

	int get_int() { return finteger; }
	void set_int(int ainteger) { finteger=ainteger; }

	void inc(int increment) { finteger+=increment; }
	
	void mul(double k) { finteger=(int)(finteger*k); }
	
	void div(double d) {
		if(d == 0)
			throw Exception("number.zerodivision", 0, "Division by zero");
		finteger=(int)(finteger/d); 
	}
	
	void mod(int d) {
		if(d == 0)
			throw Exception("number.zerodivision", 0, "Modulus by zero");
		finteger%=d; 
	}

private:

	int finteger;

};

#endif
