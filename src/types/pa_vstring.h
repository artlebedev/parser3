/**	@file
	Parser: @b string parser class decl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VSTRING_H
#define PA_VSTRING_H

#define IDENT_PA_VSTRING_H "$Id: pa_vstring.h,v 1.83 2025/05/27 15:10:24 moko Exp $"

// includes

#include "pa_vstateless_object.h"
#include "pa_vdouble.h"

// externs

extern Methoded* string_class;

/// value of type 'string'. implemented with @c String
class VString: public VStateless_object {
public: // Value

	override const char* type() const { return "string"; }
	override VStateless_class *get_class() { return string_class; }
	/// VString: eq ''=false, ne ''=true
	override bool is_defined() const { return !fstring->is_empty(); }
	/// VString: 0 or !0
	override bool as_bool() const { return as_double()!=0; }
	/// VString: true
	override bool is_string() const { return true; }

	/// VString: VDouble
	override Value& as_expr_result() { return *new VDouble(fstring->as_double()); }
	/// VString: fstring
	override const String* get_string() { return fstring; };
	/// VString: fstring
	override double as_double() const { return fstring->as_double(); }
	/// VString: fstring
	override int as_int() const { return fstring->as_int(); }

	/// VString: vfile
	override VFile* as_vfile(String::Language lang, const Request_charsets *charsets=0);

	/// VString: json string
	override const String* get_json_string(Json_options&) { 
		String* result = new String();
		result->append_quoted(fstring);
		return result;
	}

	/// VString: $method
	override Value* get_element(const String& aname) {
#if !defined(FEATURE_GET_ELEMENT4CALL) || !defined(OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL)
		// CLASS, CLASS_NAME and normally hidden $method
		if(Value* result=VStateless_object::get_element(aname))
			return result;
#endif
		// void, empty or whitespace string are hash compatible
		if (fstring->is_empty() || fstring->trim().is_empty())
			return 0;

		// bad $string.field
		return Value::get_element(aname);
	}

public: // usage

	VString(): fstring(&String::Empty) {}
	VString(const String& avalue): fstring(&avalue) {}
	/// VString is L_TAINTED by default, opposite to String, which is L_CLEAN by default
	VString(const char *avalue, String::Language alang=String::L_TAINTED): fstring(new String(avalue, alang)) {}
	VString(String::Body avalue, String::Language alang=String::L_TAINTED): fstring(new String(avalue, alang)) {}

	const String& string() const { return *fstring; }
	void set_string(const String& astring) { fstring=&astring; }

	inline static VString* empty(){
		static VString singleton;
		return &singleton;
	}
	// L_CLEAN as nothing to escape in digits
	static VString* uitoa(size_t aindex);

private:
	const String* fstring;

};

#endif
