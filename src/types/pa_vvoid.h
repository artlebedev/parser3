/** @file
	Parser: @b void parser class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VVOID_H
#define PA_VVOID_H

#define IDENT_PA_VVOID_H "$Id: pa_vvoid.h,v 1.36 2012/05/07 20:05:10 moko Exp $"

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
	/// VVoid: with OPTIMIZE_SINGLE_STRING_WRITE it allows void to survive in [$void]
	override bool is_string() const { return true; }

	/// VVoid: json-string ("null")
	override const String* get_json_string(Json_options*) {
		static const String singleton_json_null(String("null"));
		return &singleton_json_null;
	}

#ifdef STRICT_VARS
	static bool strict_vars;
#define CHECK_STRICT if(strict_vars) throw Exception(PARSER_RUNTIME, 0, "Use of uninitialized value");
#else
#define CHECK_STRICT
#endif

	/// VVoid: methods
	override Value* get_element(const String& aname) {
		// methods
		if(Value* result=VStateless_object::get_element(aname))
			return result;

#ifdef STRICT_VARS
		if(strict_vars)
			throw Exception(PARSER_RUNTIME, 0, "Use of uninitialized value field");
#endif
		return 0;
	}

	override Value& as_expr_result(bool return_string_as_is=false) { 
		CHECK_STRICT
		return VString::as_expr_result(return_string_as_is);
	}
	
	override const String* get_string() {
		CHECK_STRICT
		return VString::get_string();
	}

	override double as_double() const { 
		CHECK_STRICT
		return VString::as_double(); 
	}

	override int as_int() const { 
		CHECK_STRICT
		return VString::as_int(); 
	}

	inline static VVoid *get(){
		static VVoid singleton;
		return &singleton;
	}
};

#endif
