/**	@file
	Parser: @b object class impl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vobject.h"
#include "pa_vhash.h"
#include "pa_vtable.h"

static const char * const IDENT_VOBJECT_C="$Date: 2005/08/09 08:14:55 $";

Value* VObject::as(const char* atype, bool looking_up) { 
	if(!looking_up)
		return get_last_derived().as(atype, true/*the only user*/); // figure out from last_derivate upwards

	// is it me?
	if(Value *result=Value::as(atype, false))
		return result;

	// is it my base?
	if(fbase) {
		if(Value *result=fbase->as(atype, true))
			return result;
	}

	// neither
	return 0;
}

/// VObject: from possible parent, if any
bool VObject::is_defined() const {
	return fbase?fbase->is_defined():Value::is_defined();
}
/// VObject: from possible parent, if any
Value& VObject::as_expr_result(bool) { 
	return fbase?fbase->as_expr_result():Value::as_expr_result();
}
/// VObject: from possible parent, if any
int VObject::as_int() const {
	return fbase?fbase->as_int():Value::as_int();
}
/// VObject: from possible parent, if any
double VObject::as_double() {
	return fbase?fbase->as_double():Value::as_double();
}
/// VObject: from possible parent, if any
bool VObject::as_bool() const { 
	return fbase?fbase->as_bool():Value::as_bool();
}
/// VObject: from possible parent, if any
VFile* VObject::as_vfile(String::Language lang, const Request_charsets *charsets) {
	return fbase?fbase->as_vfile(lang, charsets):
		Value::as_vfile(lang, charsets);
}

/// VObject: from possible parent, if any
HashStringValue* VObject::get_hash() {
	if(Value* vhash=get_last_derived().as(VHASH_TYPE, false))
		return vhash->get_hash();

	return 0;
}
/// VObject: from possible 'table' parent
Table *VObject::get_table() {
	if(Value *vtable=get_last_derived().as(VTABLE_TYPE, false))
		return vtable->get_table();

	return 0;
}

/// VObject: (field)=value;(CLASS)=vclass;(method)=method_ref
Value* VObject::get_element(const String& aname, Value&, bool looking_up) {
	// simple things first: $field=ffields.field
	if(Value* result=ffields.get(aname))
		return result;

	// gets element from last_derivate upwards
	if(!looking_up) {
		// $CLASS
		if(aname==CLASS_NAME)
			return get_class();

		// $virtual_method $virtual_property
		VObject& last_derived=get_last_derived();
		if(Value* result=last_derived.stateless_object__get_element(aname, last_derived))
			return result;
	}

	return 0;
}

const VJunction* VObject::prevent_append_if_exists_in_static_or_base(Value* value, Prevent_append_if_exists_in_static_or_base_info* info)  {
	// $virtual_property, any this/bases $static_property 
	VObject& last_derived=info->_this->get_last_derived();
	if(const VJunction* result=last_derived.stateless_object__put_element(last_derived, *info->name, value))
		return result; // replaced in any(derivate or base) statics fields/properties

	return 0;
}

/// VObject: (field/property)=value
const VJunction* VObject::put_element(Value& /*aself*/, const String& aname, Value* avalue, bool /*areplace*/) {
	Prevent_append_if_exists_in_static_or_base_info info={this, &aname};
	return ffields.replace_maybe_append<const VJunction*>(aname, avalue, 
		prevent_append_if_exists_in_static_or_base, 
		&info);
}
