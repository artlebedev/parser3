/**	@file
	Parser: @b object class impl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vobject.h"

static const char* IDENT_VOBJECT_C="$Date: 2002/08/13 15:35:45 $";

/// VObject: true, todo: z base table can be 33
Value *VObject::as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }
/// VObject: true, todo: z base table can be false	
bool VObject::as_bool() const { return true; }

/// VObject: (field)=value;(CLASS)=vclass;(method)=method_ref
Value *VObject::get_element(const String& aname, Value *aself) {
	// gets element from last_derivate upwards
	if(aself) {
		// $CLASS
		if(aname==CLASS_NAME)
			return get_class();

		// for first call, pass call to last derived VObject
		return get_last_derived()->get_element(aname, 0/*mark this call as 'not first'*/);
	}

	// $method, $CLASS_field
	{
		Temp_base temp_base(*get_class(), 0);
		if(Value *result=VStateless_object::get_element(aname, this))
			return result;
	}

	// $field=ffields.field
	if(Value *result=static_cast<Value *>(ffields.get(aname)))
		return result;

	// up the tree...
	if(fbase)
		if(Value *result=fbase->get_element(aname, fbase))
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
	} catch(Exception) { 
		/* ignore "can not store to table&co errors for nonexistent elements */ 
		bool error;
		try {
			error=get_element(aname, this)!=0;
		} catch(Exception) { 
			error=false;
		}
		if(error)
			/*re*/throw;
	}

	// could not put to any base of last child
	return false;
}

