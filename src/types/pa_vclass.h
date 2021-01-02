/**	@file
	Parser: @b class parser class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#define IDENT_PA_VCLASS_H "$Id: pa_vclass.h,v 1.74 2021/01/02 23:01:11 moko Exp $"

#define OBJECT_PROTOTYPE
#define CLASS_GETTER_UNPROTECTED

// includes

#include "pa_vstateless_class.h"
#include "pa_vjunction.h"

/**	stores 
- static fields, getters & setters: VClass::ffields
*/
class VClass: public VStateless_class {
public: // Value
	
	override const char* type() const {
		if(!ftype)
			throw Exception(PARSER_RUNTIME, 0, "getting type of nameless class");
		return ftype;
	}

	/// VClass: true
	override bool as_bool() const { return true; }
	override Value* as(const char* atype);

	override Value* get_element(Value& aself, const String& aname);
	override const VJunction* put_element(Value& self, const String& name, Value* value);

	/// indicator value meaning that put_element_replace_only overwritten static field in non-prototype mode
	#define PUT_ELEMENT_REPLACED_FIELD reinterpret_cast<const VJunction*>(1)

	// for VObject::put_element
	const VJunction* put_element_replace_only(Value& self, const String& name, Value* value);

	override const String* get_json_string(Json_options& options);

	override Value* create_new_value(Pool&);

	override HashStringValue *get_hash();
	override HashStringValue* get_fields() { return get_hash(); };

public:

#ifdef OBJECT_PROTOTYPE
	static bool prototype;
#endif
#ifdef CLASS_GETTER_UNPROTECTED
	static bool getter_protected;
#endif

	// VStateless_class
	override void real_set_method(const String& aname, Method* amethod);
	override HashStringProperty* get_properties(){ return &ffields; };
	override void set_base(VStateless_class* abase);

	/// VClass default getter & setter support
	override void enable_default_getter(){ state |= IS_GETTER_ACTIVE; }
	override void enable_default_setter(){ if(has_default_setter()) state |= IS_SETTER_ACTIVE; }
	override void disable_default_getter(){ state &= ~IS_GETTER_ACTIVE; }
	override void disable_default_setter(){ state &= ~IS_SETTER_ACTIVE; }
	override bool is_enabled_default_getter(){ return (state & IS_GETTER_ACTIVE) > 0; }
	override bool is_enabled_default_setter(){ return (state & IS_SETTER_ACTIVE) > 0; }

private:

	const char* ftype;
	const String& ffilespec;

	enum State {
		IS_GETTER_ACTIVE = 0x01,
		IS_SETTER_ACTIVE = 0x02
	};

	int state; // default setter & getter state

	HashStringProperty ffields;

	Property& get_property(const String& aname);

public:

	VClass(const char* atype, const String* afilespec=0) : ftype(atype), ffilespec(afilespec ? *afilespec : String::Empty), state(IS_GETTER_ACTIVE){}

	const String& get_filespec(){ return ffilespec; }

};

#endif
