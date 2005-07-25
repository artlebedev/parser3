/**	@file
	Parser: @b class parser class impl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VCLASS_C="$Date: 2005/07/25 07:44:02 $";

#include "pa_vclass.h"

/// preparing property accessors to fields
VClass::VClass()
{
	fill_properties(ffields);
}

Value* VClass::as(const char* atype, bool looking_up) {
	if(Value* result=Value::as(atype, looking_up))
		return result;
	else
		return fbase?fbase->as(atype, looking_up):0;
}

/// VClass: $CLASS, (field/property)=STATIC value;(method)=method_ref with self=object_class
Value* VClass::get_element(const String& aname, Value& aself, bool looking_up) {
	// simple things first: $field=static field/property
	if(Value* result=ffields.get(aname)) {
		if(Property* prop=result->get_property()) // it is property?
		{
			Method* method=prop->getter;
			if(!method)
				throw Exception("parser.runtime",
					&aname,
					"this property has no getter method (get_%s)", aname.cstr());

			return new VJunction(new Junction(aself, method, true /*is_getter*/));
		}
		return result;
	}

	// $CLASS, $method, or other base element
	if(Value* result=VStateless_class::get_element(aname, aself, looking_up))
		return result; // TODO: this can be SIGNIFICANTLY sped up by caching in ffields! [THOUGH decide about different aself] // what REALLY would speed up things is to join storage of properties/methods/fields of all vobject parents into last descenant [sort of vmt + all fields as in other langs]

	return 0;
}

/// VClass: (field)=value - static values only
const Method* VClass::put_element(const String& aname, Value* avalue, bool replace) {
	try {
		if(fbase && fbase->put_element(aname, avalue, true))
			return PUT_ELEMENT_REPLACED_ELEMENT; // replaced in base
	} catch(Exception) {  /* allow override parent variables, useful for form descendants */ }

	if(replace)
		return ffields.put_replace(aname, avalue)? PUT_ELEMENT_REPLACED_ELEMENT: 0;
	else {
		ffields.put(aname, avalue);
		return 0;
	}
}

/// @returns object of this class
Value* VClass::create_new_value(Pool& apool) { 
	return new VObject(apool, *this);
}
