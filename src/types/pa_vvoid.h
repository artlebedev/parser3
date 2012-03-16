/** @file
	Parser: @b void parser class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VVOID_H
#define PA_VVOID_H

#define IDENT_PA_VVOID_H "$Id: pa_vvoid.h,v 1.35 2012/03/16 09:24:19 moko Exp $"

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
	/// VVoid: methods
	override Value* get_element(const String& aname) {
		// methods
		if(Value* result=VStateless_object::get_element(aname))
			return result;

		return 0;
	}

	inline static VVoid *get(){
		static VVoid singleton;
		return &singleton;
	}
};

#endif
