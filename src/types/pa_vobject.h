/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

static const char * const IDENT_VOBJECT_H="$Date: 2009/09/17 23:31:42 $";

// includes

#include "pa_vjunction.h"
#include "pa_vclass.h"
#include "pa_vstateless_object.h"
#include "pa_vfile.h"

// defines

#define BASE_NAME "BASE"

/**	parser class instance, stores 
	- class VObject::fclass;
	- fields VObject::ffields (dynamic, not static, which are stored in class).
*/
class VObject: public Value {

	VStateless_class& fclass;
	HashStringValue ffields;

public: // Value
	
	const char* type() const { return fclass.name_cstr(); }
	override Value* as(const char* atype);

	/// VObject: fclass
	override VStateless_class *get_class() { return &fclass; }

	override bool is_defined() const;
	override Value& as_expr_result(bool);
	override int as_int() const;
	override double as_double() const;
	override bool as_bool() const;
	override VFile* as_vfile(String::Language lang, const Request_charsets *charsets=0);
	
	override HashStringValue* get_hash();
	override Table *get_table();

	override Value* get_element(const String& aname);
	override const VJunction* put_element(const String& name, Value* value, bool replace);

public: // creation

	VObject(VStateless_class& aclass): fclass(aclass){}

private:

	Value* get_scalar_value(char* as_something) const;

};

#endif
