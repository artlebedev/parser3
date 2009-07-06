/** @file
	Parser: @b void parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VVOID_H
#define PA_VVOID_H

static const char * const IDENT_VVOID_H="$Date: 2009/07/06 12:10:09 $";

#include "classes.h"
#include "pa_vstateless_object.h"
#include "pa_globals.h"

extern Methoded* void_class;

/// value of type 'void'. ex: usually $sjfklsjfksjdfk has this type
class VVoid: public VStateless_object {
public: // Value

	override const char* type() const { return "void"; }
	override VStateless_class *get_class() { return void_class; }

	/// VVoid: true [the only one, that reports true]
	override bool is_void() const { return true; }
	/// VVoid: ""
	override const String* get_string() { return &void_string_value; }
	/// VVoid: false
	override bool as_bool() const { return false; }
	/// VVoid: false
	override bool is_defined() const { return false; }
	/// VVoid: 0
	override double as_double() const { return 0; }
	/// VVoid: 0
	override int as_int() const { return 0; }
	/// VVoid: this
	override Value& as_expr_result(bool /*return_string_as_is=false*/) { return *this; }

	/// VVoid: empty vfile
	override VFile* as_vfile(String::Language lang, const Request_charsets *charsets);

	/// VVoid: methods
	override Value* get_element(const String& aname, Value& aself, bool looking_up) {
		// methods
		if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
			return result;

		return 0;
	}

	/// VVoid: bark
	override const VJunction* put_element(Value& aself, const String& aname, Value* avalue, bool areplace) { 
		return Value::put_element(aself, aname, avalue, areplace);
	}

	inline static VVoid *get(){
		static VVoid singleton;
		return &singleton;
	}

private:

	static const String void_string_value;

};

#endif
