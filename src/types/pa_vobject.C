/**	@file
	Parser: @b object class impl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vobject.h"
#include "pa_vhash.h"
#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vmethod_frame.h"
#include "pa_request.h"

static const char * const IDENT_VOBJECT_C="$Date: 2008/08/26 11:53:43 $";

Value* VObject::get_scalar_value() const {
	VObject* unconst_this=const_cast<VObject*>(this);
	VObject& last_derived=unconst_this->get_last_derived();
	if(Value* scalar=unconst_this->get_scalar(last_derived))
		if(Junction* junction=scalar->get_junction())
			if(const Method *method=junction->method){
				VMethodFrame frame(*junction, 0/*no caller*/);
				frame.set_self(last_derived);
				return &pa_thread_request().execute_method(frame, *method).as_value();
			}

	return 0;
}

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
	if(Value* value=get_scalar_value())
		return value->is_defined();
	return fbase?fbase->is_defined():Value::is_defined();
}
/// VObject: from possible parent, if any
Value& VObject::as_expr_result(bool) {
	if(Value* value=get_scalar_value())
		return value->as_expr_result();
	return fbase?fbase->as_expr_result():Value::as_expr_result();
}
/// VObject: from possible parent, if any
int VObject::as_int() const {
	if(Value* value=get_scalar_value())
		return value->as_int();
	return fbase?fbase->as_int():Value::as_int();
}
/// VObject: from possible parent, if any
double VObject::as_double() const {
	if(Value* value=get_scalar_value())
		return value->as_double();
	return fbase?fbase->as_double():Value::as_double();
}
/// VObject: from possible parent, if any
bool VObject::as_bool() const { 
	if(Value* value=get_scalar_value())
		return value->as_bool();
	return fbase?fbase->as_bool():Value::as_bool();
}
/// VObject: from possible parent, if any
VFile* VObject::as_vfile(String::Language lang, const Request_charsets *charsets) {
	if(Value* value=get_scalar_value())
		return value->as_vfile(lang, charsets);
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
Value* VObject::get_element(const String& aname, Value&, bool alooking_up) {
	// simple things first: $field=ffields.field
	if(Value* result=ffields.get(aname))
		return result;

	// gets element from last_derivate upwards
	if(alooking_up) {
		VObject& last_derived=get_last_derived();
		// $CLASS
		if(aname==CLASS_NAME)
			return last_derived.get_class();
		// $CLASS_NAME
		if(aname==CLASS_NAMETEXT)
			return new VString(last_derived.get_class()->name());
		// $virtual_method $virtual_property
		if(Value* result=last_derived.stateless_object__get_element(aname, last_derived))
			return result;

		if(Value* result=last_derived.get_default_getter(last_derived, aname))
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
const VJunction* VObject::put_element(Value& /*aself*/, const String& aname, Value* avalue, bool /*areplace*/){
	Prevent_append_if_exists_in_static_or_base_info info={this, &aname};
	return ffields.replace_maybe_append<const VJunction*>(aname, avalue, 
		prevent_append_if_exists_in_static_or_base, 
		&info);
}
