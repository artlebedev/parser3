/**	@file
	Parser: @b class parser class impl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VCLASS_C="$Date: 2009/04/10 11:41:35 $";

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
void VClass::add_method(const String& aname, Method& amethod) {
	if(aname.starts_with("GET_")){
		if(aname=="GET_DEFAULT")
			set_default_getter(&amethod);
		else
			add_property(aname).getter=&amethod;
	} else if(aname.starts_with("SET_")){
		add_property(aname).setter=&amethod;
	} else if(aname=="GET"){
		set_scalar(&amethod);
	}


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
Value* VClass::get_element(const String& aname, Value& aself, bool alooking_up) {
	bool property_but_no_getter_in_self=false;

	// simple things first: $field=static field/property
	if(Value* result=ffields.get(aname)) {
		Property* prop=result->get_property();

		if(!prop) // just field, not a property
			return result;

		if(Method* method=prop->getter) // has getter
			return new VJunction(aself, method, true /*is_getter*/);

		property_but_no_getter_in_self=true;
	}

	// $CLASS, $method, or other base element
	if(Value* result=VStateless_class::get_element(aname, aself, false))
		return result; // @TODO: this can be SIGNIFICANTLY sped up by caching in ffields! [THOUGH decide about different aself] // what REALLY would speed up things is to join storage of properties/methods/fields of all vobject parents into last descenant [sort of vmt + all fields as in other langs]

	if(property_but_no_getter_in_self)
		throw Exception(PARSER_RUNTIME,
			0,
			"this property has no getter method (@GET_%s[])", aname.cstr());

	// no field or method found: looking for default getter
	if(alooking_up)
		if(Value* result=get_default_getter(aself, aname))
			return result;

	return 0;
}

#define PROPERTY_BUT_NO_SETTER_IN_SELF reinterpret_cast<const VJunction*>(2)
const VJunction* VClass::prevent_overwrite_property(Value* value, Prevent_info* info) {
	if(Property* property=value->get_property()) {
		if(Method* setter=property->setter)
			return new VJunction(*info->self, setter);

		return PROPERTY_BUT_NO_SETTER_IN_SELF;
	}

	return 0;
}
const VJunction* VClass::prevent_append_if_exists_in_base(Value* value, Prevent_info* info)  {
	if(VStateless_class* cbase=info->_this->fbase) {
		if(Value* obase=info->self->base()) // MXdoc has fbase but does not have object_base[ base() ]
			if(const VJunction* result=cbase->put_element(*obase, *info->name, value, true/*try to replace! NEVER overwrite*/))
				return result; // replaced in base
	}

	return 0;
}
/// VClass: (field/property)=value - static values only
const VJunction* VClass::put_element(Value& aself, const String& aname, Value* avalue, bool areplace) {
	Prevent_info info={this, &aself, &aname};
	if(areplace) {
		bool property_but_no_setter_in_self=false;
		// trying to replace it in fields/properties
		if(const VJunction* result=ffields.maybe_replace_never_append<const VJunction*>(aname, avalue, 
			prevent_overwrite_property,
			&info))
			if(result==PROPERTY_BUT_NO_SETTER_IN_SELF)
				property_but_no_setter_in_self=true; // continue to find setter up the tree
			else
				return result; // replaced locally

		// if not found locally, going up the tree
		if(fbase)
			if(Value* obase=aself.base()) // MXdoc has fbase but does not have object_base[ base() ]
				if(const VJunction* result=fbase->put_element(*obase, aname, avalue, true/*try to replace! NEVER overwrite*/))
					return result; // replaced in base

		if(property_but_no_setter_in_self)
			throw Exception(PARSER_RUNTIME,
				0,
				"this property has no setter method (@SET_%s[value])", aname.cstr());

		return 0;
	} else // append if not existed neither in fields nor in base classes
		return ffields.maybe_replace_maybe_append<const VJunction*>(aname, avalue, 
			prevent_overwrite_property,
			prevent_append_if_exists_in_base, 
			&info);
}

/// @returns object of this class
Value* VClass::create_new_value(Pool& apool, HashStringValue& afields) { 
	return new VObject(apool, *this, afields);
}
