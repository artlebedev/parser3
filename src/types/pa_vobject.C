/**	@file
	Parser: @b object class impl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vobject.h"

static const char* IDENT_VOBJECT_C="$Date: 2002/08/14 14:18:30 $";

bool VObject::is(const char *atype, bool looking_up) const { 
	if(!looking_up)
		return get_last_derived_const()->is(atype, true/*the only user*/); // figure out from last_derivate upwards

	// is it me?
	if(Value::is(atype, false))
		return true;

	// is it my base?
	if(fbase) {
		if(fbase->is(atype, true))
			return true;
	}

	// neither
	return false;
}

/// VObject: true, todo: z base table can be 33
Value *VObject::as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }
/// VObject: true, todo: z base table can be false	
bool VObject::as_bool() const { return true; }

/// VObject: (field)=value;(CLASS)=vclass;(method)=method_ref
Value *VObject::get_element(const String& aname, Value * /*aself*/, bool looking_up) {
	// gets element from last_derivate upwards
	if(!looking_up) {
		// $CLASS
		if(aname==CLASS_NAME)
			return get_class();

		// for first call, pass call to last derived VObject
		return get_last_derived()->get_element(aname, 
			0, true/*the only user*/);
	}

	// $method, $CLASS_field
	{
		Temp_base temp_base(*get_class(), 0);
		if(Value *result=VStateless_object::get_element(aname, this, true))
			return result;
	}

	// $field=ffields.field
	if(Value *result=static_cast<Value *>(ffields.get(aname)))
		return result;

	// up the tree...
	if(fbase)
		if(Value *result=fbase->get_element(aname, fbase, true))
			return result;

	return 0;
}

/// VObject: (field)=value
bool VObject::put_element(const String& aname, Value *avalue, bool replace) {
	// replaces element to last_derivate upwards or stores it in self
	// speed1:
	//   will not check for '$CLASS(subst)' trick
	//   will hope that user ain't THAT self-hating person
	// speed2:
	//   will not check for '$method_name(subst)' trick
	//   -same-

	if(!replace) { 
		// for first call, pass call to last derived VObject
		if(get_last_derived()->put_element(aname, avalue, true))
			return true;

		ffields.put(aname, avalue);
		return false;
	}

	// replace
	// upwards: copied from VClass::put_element...

	try {
		if(fbase && fbase->put_element(aname, avalue, true))
			return true; // replaced in base

		return ffields.put_replace(aname, avalue);
	} catch(Exception) { /* allow override parent variables, useful for form descendants */ }

	// could not put to any base of last child
	return false;
}

