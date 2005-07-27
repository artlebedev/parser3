/**	@file
	Parser: @b object class impl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vobject.h"
#include "pa_vhash.h"
#include "pa_vtable.h"

static const char * const IDENT_VOBJECT_C="$Date: 2005/07/27 06:15:34 $";

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

		// $virtual_method
		VObject& last_derived=get_last_derived();
		if(Value* result=last_derived.stateless_object__get_element(aname, last_derived))
			return result;
	}

	// up the tree for other $virtual_field try...
	if(fbase)
		if(Value* result=fbase->get_element(aname, *fbase, true))
			return result;

	return 0;
}
Value* VObject::stateless_object__get_element(const String& aname, Value& aself) {
	return VStateless_object::get_element(aname, aself, false);
}

// from pa_vclass.C
const Method* pa_prevent_overwrite_property(Value* value);

/// VObject: (field/property)=value
const Junction* VObject::put_element(const String& aname, Value* avalue, bool replace) {
	if(fbase)
		if(const Junction* result=fbase->put_element(aname, avalue, true))
			return result; // replaced in base dynamic(NOT static!) fields

	if(replace) {
		// we're in some parent, we should NOT try to insert there, only IF that field/property existed there
		if(const Method* method=ffields.maybe_put_replaced<const Method*>(aname, avalue, pa_prevent_overwrite_property) ) {
			if(method==reinterpret_cast<const Method*>(1)) // existed, but not were not property?
				return PUT_ELEMENT_REPLACED_ELEMENT;
			return new Junction(*this, method, true /*is_setter*/);
		}

		return 0; // NOT replaced/putted anything to parent [there were NO such field/property for us to fill]
	} else {
		if(const Junction* result=VStateless_object::put_element(aname, avalue, true))
			return result; // replaced in base statics fields

		ffields.put(aname, avalue);
		return 0; // were simply added [not existed before]
	}
}

