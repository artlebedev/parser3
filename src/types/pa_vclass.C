/**	@file
	Parser: @b class parser class impl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VCLASS_C="$Date: 2002/08/13 15:55:43 $";

#include "pa_vclass.h"

/// VClass: (field)=STATIC value;(method)=method_ref with self=object_class
Value *VClass::get_element(const String& aname, Value *aself, bool looking_down) {
	// $method or other base element
	if(Value *result=VStateless_class::get_element(aname, aself, looking_down))
		return result;

	// $field=static field
	if(Value *result=static_cast<Value *>(ffields.get(aname)))
		return result;

	return 0;
}

/// VClass: (field)=value - static values only
/*override*/ bool VClass::put_element(const String& aname, Value *avalue, bool replace) {
	try {
		if(fbase && fbase->put_element(aname, avalue, true))
			return true; // replaced in base
	} catch(Exception) { /* ignore "can not store to stateless_class errors */ }

	if(replace)
		return ffields.put_replace(aname, avalue);
	else {
		ffields.put(aname, avalue);
		return false;
	}
}

/// @returns object of this class
/*override*/ Value *VClass::create_new_value(Pool& ) { 
	return NEW VObject(pool(), *this);
}
