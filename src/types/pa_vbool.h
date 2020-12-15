/** @file
	Parser: @b bool class decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VBOOL_H
#define PA_VBOOL_H

#define IDENT_PA_VBOOL_H "$Id: pa_vbool.h,v 1.38 2020/12/15 17:10:39 moko Exp $"

// include

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VBOOL_TYPE "bool"
#define MAX_BOOL_AS_STRING 20

extern Methoded* bool_class;

// VBool
class VBool: public VStateless_object {
public: // Value

	override const char* type() const { return VBOOL_TYPE; }
	/// VBool: 0
	override VStateless_class *get_class() { return bool_class; }

	/// VBool: true
	override bool is_evaluated_expr() const { return true; }
	/// VBool: clone
	override Value& as_expr_result() { return *this; }

	/// VBool: true
	virtual bool is_defined() const { return true; }

	/// VBool: fbool
	override double as_double() const { return fbool ? 1 : 0; }
	/// VBool: fbool
	override int as_int() const { return fbool ? 1 : 0; }
	/// VBool: fbool
	override bool as_bool() const { return fbool; }

	override bool is_bool() const { return true; }

	/// VBool: json-string ("true"|"false")
	override const String* get_json_string(Json_options&) {
		static const String singleton_json_true(String("true")), singleton_json_false(String("false"));
		return fbool ? &singleton_json_true : &singleton_json_false;
	}
	
	inline static VBool &get(bool abool){
		static VBool singleton_true(true), singleton_false(false);
		return abool?singleton_true:singleton_false;
	}

public: // usage

	VBool(bool abool): fbool(abool) {}
	bool get_bool() { return fbool; }

private:

	bool fbool;

};

#endif
