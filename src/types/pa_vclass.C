/**	@file
	Parser: @b class parser class impl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vclass.h"
#include "pa_vobject.h"

volatile const char * IDENT_PA_VCLASS_C="$Id: pa_vclass.C,v 1.62 2021/01/02 23:01:11 moko Exp $" IDENT_PA_VCLASS_H;

#ifdef OBJECT_PROTOTYPE
	bool VClass::prototype = true;
#endif
#ifdef CLASS_GETTER_UNPROTECTED
	bool VClass::getter_protected = true;
#endif

Property& VClass::get_property(const String& aname) {
	Property* result=ffields.get(aname);
	if (result) {
		if (!result->getter && !result->setter) {
			// replacing field with property
			result=new Property();
		} else {
			// cloning existing property to avoid ancestor modification
			result=new Property(*result);
		}
	} else {
		// creating new property
		result=new Property();
	}
	ffields.put(aname, result);
	return *result;
}

void VClass::real_set_method(const String& aname, Method* amethod) {
	if(aname.starts_with("GET_")){
		if(aname=="GET_DEFAULT")
			set_default_getter(amethod);
		else
			get_property(aname.mid(4, aname.length())).getter=amethod;
	} else if(aname.starts_with("SET_")){
		if(aname=="SET_DEFAULT")
			set_default_setter(amethod);
		else
			get_property(aname.mid(4, aname.length())).setter=amethod;
	} else if(aname=="GET"){
		set_scalar(amethod);
	}

	// NOT under 'else' for backward compatiblilty: 
	// if someone used get_xxx names to name regular methods
	// still register method:
	VStateless_class::real_set_method(aname, amethod);
}

void VClass::set_base(VStateless_class* abase){
	VStateless_class::set_base(abase);
	if(abase) {
		if(HashStringProperty *props=abase->get_properties())
			ffields.merge_dont_replace(*props);
		else
			throw Exception("parser.compile",
				0,
				"Class %s base class (%s) is not user-defined", type(), abase->type());
	}
}

Value* VClass::as(const char* atype) {
	Value* result=Value::as(atype);
	return result!=0 ? result : fbase ? fbase->as(atype) : 0;
}

/// VClass: $CLASS, (field/property)=STATIC value;(method)=method_ref with self=object_class
Value* VClass::get_element(Value& aself, const String& aname) {
	// simple things first: $field=static field/property
	if(Property* prop=ffields.get(aname)) {
		if(prop->getter)
			return new VJunction(aself, prop->getter, true /*is_getter*/);

		if(prop->setter){
			if(Value *result=get_default_getter(aself, aname))
				return result;
			throw Exception(PARSER_RUNTIME,	0, "this property has no getter method (@GET_%s[])", aname.cstr());
		}
		 
		// just field, can be 0 as we don't remove 
		return prop->value;
	}

	// $CLASS, $method, or other base element
	if(Value* result=VStateless_class::get_element(aself, aname))
		return result;

	// no field or method found: looking for default getter
	return get_default_getter(aself, aname);
}

static void add_field(
		HashStringProperty::key_type key, 
		HashStringProperty::value_type prop,
		HashStringValue* result
){
	if(prop->value)
		result->put(key, prop->value);
}

HashStringValue* VClass::get_hash() {
	HashStringValue* result=new HashStringValue();
	ffields.for_each(add_field, result);
	return result;
}

/// VClass: (field/property)=value - static values only
const VJunction* VClass::put_element(Value& aself, const String& aname, Value* avalue) {
	if(Property* prop=ffields.get(aname)) {
		if (prop->setter)
			return new VJunction(aself, prop->setter);

		if(prop->getter){
			if(VJunction *result=get_default_setter(aself, aname))
				return result;
#ifdef CLASS_GETTER_UNPROTECTED
			if(!getter_protected)
				prop->getter=0;
			else
#endif
				throw Exception(PARSER_RUNTIME,	0, "this property has no setter method (@SET_%s[value])", aname.cstr());
		}

		// just field, value can be 0 and unlike usual we don't remove it
		prop->value=avalue;
	} else {
		if(VJunction *result=get_default_setter(aself, aname))
			return result;

		prop=new Property();
		prop->value=avalue;
		ffields.put(aname, prop);

		Array_iterator<VStateless_class *> i(fderived);
		while(i.has_next()) {
			HashStringProperty *props=i.next()->get_properties();
			if(props)
				props->put_dont_replace(aname, prop);
		}
	}
	return 0;
}

/// part of put_element
const VJunction* VClass::put_element_replace_only(Value& aself, const String& aname, Value* avalue) {
	if(Property* prop=ffields.get(aname)) {
		if (prop->setter)
			return new VJunction(aself, prop->setter);
#ifdef OBJECT_PROTOTYPE
		if(!prototype)
#endif
		{
			if(prop->getter){
				if(VJunction *result=get_default_setter(aself, aname))
					return result;
				throw Exception(PARSER_RUNTIME,	0, "this property has no setter method (@SET_%s[value])", aname.cstr());
			}
			// just field, value can be 0 and unlike usual we don't remove it
			prop->value=avalue;
			return PUT_ELEMENT_REPLACED_FIELD;
		}
	}
	return 0;
}

/// @returns object of this class
Value* VClass::create_new_value(Pool&) { 
	return new VObject(*this);
}

const String* VClass::get_json_string(Json_options& options){
	if(options.default_method){
		return default_method_2_json_string(*options.default_method, options);
	}
	return options.hash_json_string(get_hash());
}
