/**	@file
	Parser: @b class parser class impl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VCLASS_C="$Date: 2003/07/24 11:31:25 $";

#include "pa_vclass.h"

Value* VClass::as(const char* atype, bool looking_up) {
	if(Value* result=Value::as(atype, looking_up))
		return result;
	else
		return fbase?fbase->as(atype, looking_up):0;
}

/// VClass: $CLASS, (field)=STATIC value;(method)=method_ref with self=object_class
Value* VClass::get_element(const String& aname, Value& aself, bool looking_up) {
	// simple things first: $field=static field
	if(Value* result=ffields.get(aname))
		return result;

	// $CLASS, $method, or other base element
	if(Value* result=VStateless_class::get_element(aname, aself, looking_up))
		return result;

	return 0;
}

/// VClass: (field)=value - static values only
bool VClass::put_element(const String& aname, Value* avalue, bool replace) {
	try {
		if(fbase && fbase->put_element(aname, avalue, true))
			return true; // replaced in base
	} catch(Exception) {  /* allow override parent variables, useful for form descendants */ }

	if(replace)
		return ffields.put_replace(aname, avalue);
	else {
		ffields.put(aname, avalue);
		return false;
	}
}

/// @returns object of this class
Value* VClass::create_new_value() { 
	return new VObject(*this);
}
