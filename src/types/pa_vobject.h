/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

static const char * const IDENT_VOBJECT_H="$Date: 2009/07/07 05:48:24 $";

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
	- links to base/derived instances [VObject::fbase, VObject::fderived]
*/
class VObject: public VStateless_object {

	VStateless_class& fclass;
	HashStringValue& ffields;
	VObject* fderived;
	Value* fbase;

public: // Value
	
	const char* type() const { return fclass.name_cstr(); }
	override Value* as(const char* atype, bool looking_up);

	/// VObject: fclass
	override VStateless_class *get_class() { return &fclass; }
	override VStateless_class *get_last_derived_class() { return get_last_derived().get_class(); }
	override Value* base() { return fbase; }

	override bool is_defined() const;
	override Value& as_expr_result(bool);
	override int as_int() const;
	override double as_double() const;
	override bool as_bool() const;
	override VFile* as_vfile(String::Language lang, const Request_charsets *charsets=0);
	
	override HashStringValue* get_hash();
	override Table *get_table();

	override Value* get_element(const String& aname, Value& aself, bool alooking_up);
	override const VJunction* put_element(Value& self, const String& name, Value* value, bool replace);

	/// VObject: remember derived [the only client]
	override VObject* set_derived(VObject* aderived) { 
		VObject* result=fderived;
		fderived=aderived;
		return result;
	}

public: // creation

	VObject(Pool& apool, VStateless_class& aclass, HashStringValue* afields): fclass(aclass), ffields(*afields), fderived(0) {
		if(VStateless_class* base_class=fclass.base_class())
			(fbase=base_class->create_new_value(apool, afields))->set_derived(this);
	}

private:

	VObject& get_last_derived() {
		return fderived?fderived->get_last_derived():*this;
	}

	Value* stateless_object__get_element(const String& aname, Value& aself) {
		return VStateless_object::get_element(aname, aself, false);
	}

	Value* get_scalar_value() const;

	const VJunction* stateless_object__put_element(Value& aself, const String& aname, Value* avalue) {
		return VStateless_object::put_element(aself, aname, avalue, true/*try to replace! NEVER overwrite*/);
	}

	struct Prevent_append_if_exists_in_static_or_base_info {
		VObject* _this;
		const String* name;
	};
	static const VJunction* prevent_append_if_exists_in_static_or_base(Value* value, 
		Prevent_append_if_exists_in_static_or_base_info* info);

};

class Temp_derived {
	Value& fvalue;
	VObject* fsaved_derived;
public:
	Temp_derived(Value& avalue, VObject* aderived) : 
		fvalue(avalue),
		fsaved_derived(avalue.set_derived(aderived)) {}
	~Temp_derived() { fvalue.set_derived(fsaved_derived); }
};

#endif
