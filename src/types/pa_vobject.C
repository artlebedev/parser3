/**	@file
	Parser: @b object class impl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vobject.h"
#include "pa_vhash.h"
#include "pa_vtable.h"

static const char* IDENT_VOBJECT_C="$Date: 2002/10/31 15:01:56 $";

Value *VObject::as(const char *atype, bool looking_up) { 
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
Value *VObject::as_expr_result(bool) { 
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
VFile *VObject::as_vfile(String::Untaint_lang lang, bool origins_mode) {
	return fbase?fbase->as_vfile(lang, origins_mode):Value::as_vfile(lang, origins_mode);
}

/// VObject: from possible parent, if any
Hash *VObject::get_hash(const String *source) {
	if(Value *vhash=get_last_derived().as(VHASH_TYPE, false))
		return vhash->get_hash(source);

	return 0;
}
/// VObject: from possible 'table' parent
Table *VObject::get_table() {
	if(Value *vtable=get_last_derived().as(VTABLE_TYPE, false))
		return vtable->get_table();

	return 0;
}

/// VObject: (field)=value;(CLASS)=vclass;(method)=method_ref
Value *VObject::get_element(const String& aname, Value& aself, bool looking_up) {
	// simple things first: $field=ffields.field
	if(Value *result=static_cast<Value *>(ffields.get(aname)))
		return result;

	// gets element from last_derivate upwards
	if(!looking_up) {
		// $CLASS
		if(aname==CLASS_NAME)
			return get_class();

		// $virtual_method
		VObject& last_derived=get_last_derived();
		if(Value *result=last_derived.stateless_object__get_element(aname, last_derived))
			return result;
	}

	// up the tree for other $virtual_field try...
	if(fbase)
		if(Value *result=fbase->get_element(aname, *fbase, true))
			return result;

	return 0;
}
Value *VObject::stateless_object__get_element(const String& aname, Value& aself) {
	return VStateless_object::get_element(aname, aself, false);
}

/// VObject: (field)=value
bool VObject::put_element(const String& aname, Value *avalue, bool replace) {
	//copied from VStateless_class, maybe not needed try {
		if(fbase && fbase->put_element(aname, avalue, true))
			return true; // replaced in base
	//} catch(Exception) {  /* allow override parent variables, useful for form descendants */ }

	if(replace)
		return ffields.put_replace(aname, avalue);
	else {
		ffields.put(aname, avalue);
		return false;
	}
}

