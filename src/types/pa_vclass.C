/**	@file
	Parser: @b class parser class impl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VCLASS_C="$Date: 2005/07/26 12:43:05 $";

#include "pa_vclass.h"

Property& VClass::add_property(const String& aname) {
	String prop_name=aname.mid(4, aname.length());
	
	Property* result;
	if(Value* value=ffields.get(prop_name)) {
		result=value->get_property();
		if(!result) // can occur in ^process
			throw Exception("parser.compile",
				&prop_name,
				"property can not be created, already exists field (%s) with that name", value->get_class()->name_cstr());
	} else {
		VProperty* vproperty=new VProperty();
		ffields.put(prop_name, vproperty);
		result=&vproperty->get();
	}
	return *result;
}

/// preparing property accessors to fields
void VClass::add_method(const String& aname, Method& amethod)
{
	if(aname.starts_with("get_"))
		add_property(aname).getter=&amethod;
	else if(aname.starts_with("set_") )
		add_property(aname).setter=&amethod;
	
	// NOT under 'else' for backward compatiblilty: 
	// if someone used get_xxx names to name regular methods
	// still register method:
	VStateless_class::add_method(aname, amethod);
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
		if(Property* prop=result->get_property()) { // it is property?
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

static const Method* prevent_overwrite_property(Value* value) {
	if(Property* property=value->get_property()) {
		if(Method* result=property->setter)
			return result;
		
		throw Exception("parser.runtime",
			0,
			"this property has no setter method (is read-only)");
	}

	return 0;
}

/// VClass: (field/property)=value - static values only
const Junction* VClass::put_element(const String& aname, Value* avalue, bool /*replace TODO: maybe remove?*/) {
	try {
		if(fbase)
			if(const Junction* result=fbase->put_element(aname, avalue, true))
				return result; // replaced in base
	} catch(Exception) {  /* allow override parent variables, useful for form descendants */ }

	if(const Method* method=ffields.maybe_put<const Method*>(aname, avalue, prevent_overwrite_property) ) {
		if(method==reinterpret_cast<Method*>(1)) // existed, but not were not property?
			return PUT_ELEMENT_REPLACED_ELEMENT;
		return new Junction(*this, method, true /*is_setter*/);
	}

	return 0; // were simply added [not existed before]
}

/// @returns object of this class
Value* VClass::create_new_value(Pool& apool) { 
	return new VObject(apool, *this);
}
