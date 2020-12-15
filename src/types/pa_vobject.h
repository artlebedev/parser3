/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

#define IDENT_PA_VOBJECT_H "$Id: pa_vobject.h,v 1.68 2020/12/15 17:10:42 moko Exp $"

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

	VClass& fclass;
	HashStringValue ffields;

	enum State {
		IS_GETTER_ACTIVE = 0x01,
		IS_SETTER_ACTIVE = 0x02
	};

	int state; // default setter & getter state

public: // Value
	
	const char* type() const { return fclass.type(); }
	override Value* as(const char* atype);

	/// VObject: fclass
	override VStateless_class *get_class() { return &fclass; }

	override bool is_defined() const;
	override Value& as_expr_result();
	override int as_int() const;
	override double as_double() const;
	override bool as_bool() const;
	override VFile* as_vfile(String::Language lang, const Request_charsets *charsets=0);
	
	override HashStringValue* get_hash();
	override Table *get_table();
	override HashStringValue* get_fields() { return &ffields; }
	override HashStringValue* get_fields_reference() { return &ffields; }

	override Value* get_element(const String& aname);
	override const VJunction* put_element(const String& name, Value* value);
#ifdef FEATURE_GET_ELEMENT4CALL
	override Value* get_element4call(const String& aname);
#endif

	override const String* get_json_string(Json_options& options);

	/// VObject default getter & setter support
	override void enable_default_getter(){ state |= IS_GETTER_ACTIVE; }
	override void enable_default_setter(){ if(fclass.has_default_setter()) state |= IS_SETTER_ACTIVE; }
	override void disable_default_getter(){ state &= ~IS_GETTER_ACTIVE; }
	override void disable_default_setter(){ state &= ~IS_SETTER_ACTIVE; }
	override bool is_enabled_default_getter(){ return (state & IS_GETTER_ACTIVE) > 0; }
	override bool is_enabled_default_setter(){ return (state & IS_SETTER_ACTIVE) > 0; }

public: // creation

	VObject(VClass& aclass): fclass(aclass), state(IS_GETTER_ACTIVE){}

private:

	Value* get_scalar_value(const char* as_something) const;
};

///	Auto-objects used for temporarily disabling setter/getter

class Temp_disable_default_getter {
	Value& fwhere;
public:
	Temp_disable_default_getter(Value& awhere) : fwhere(awhere) {
		fwhere.disable_default_getter();
	}
	~Temp_disable_default_getter() { 
		fwhere.enable_default_getter();
	}
};

class Temp_disable_default_setter {
	Value& fwhere;
public:
	Temp_disable_default_setter(Value& awhere) : fwhere(awhere) {
		fwhere.disable_default_setter();
	}
	~Temp_disable_default_setter() { 
		fwhere.enable_default_setter();
	}
};

#endif
