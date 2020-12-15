/** @file
	Parser: @b void parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VVOID_H
#define PA_VVOID_H

#define IDENT_PA_VVOID_H "$Id: pa_vvoid.h,v 1.45 2020/12/15 17:10:44 moko Exp $"

#define STRICT_VARS

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_globals.h"
#include "pa_vstring.h"

extern Methoded* void_class;

/// value of type 'void'. ex: usually $sjfklsjfksjdfk has this type
class VVoid: public VString {
public: // Value

	override const char* type() const { return "void"; }
	override VStateless_class *get_class() { return void_class; }

	/// VVoid: true [the only one, that reports true]
	override bool is_void() const { return true; }

	/// VVoid: json-string ("null")
	override const String* get_json_string(Json_options& options) {
		if(options.fvoid == Json_options::V_STRING){
			static const String singleton_json_empty_string(String("\"\""));
			return &singleton_json_empty_string;
		} else {
			static const String singleton_json_null(String("null"));
			return &singleton_json_null;
		}
	}

#ifdef STRICT_VARS
	static bool strict_vars;
#define CHECK_STRICT if(strict_vars) throw Exception(PARSER_RUNTIME, 0, "Use of uninitialized value");
#else
#define CHECK_STRICT
#endif

	/// VVoid: with OPTIMIZE_SINGLE_STRING_WRITE it allows void to survive in [$void]
	override bool is_string() const {
		return true;
	}

	override const String* get_string() {
		CHECK_STRICT
		return VString::get_string();
	}

	override Value& as_expr_result() {
		CHECK_STRICT
		return VString::as_expr_result();
	}

	inline static VVoid *get(){
		static VVoid singleton;
		return &singleton;
	}
};

#endif
