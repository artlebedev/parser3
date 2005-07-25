/**	@file
	Parser: stateless class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

static const char * const IDENT_VSTATELESS_CLASS_C="$Date: 2005/07/25 07:44:02 $";

#include "pa_vstateless_class.h"
#include "pa_vproperty.h"

/// @TODO why?! request must be different ptr from global [used in VStateless_class.add_method]
void VStateless_class::add_method(const String& name, Method& method) {
	if(flocked)
		throw Exception("parser.runtime",
			&name,
			"can not add method to system class (maybe you have forgotten .CLASS in ^process[$caller.CLASS]{...}?)");

	put_method(name, &method);
}

void VStateless_class::add_native_method(
	const char* cstr_name,
	Method::Call_type call_type,
	NativeCodePtr native_code,
	int min_numbered_params_count, int max_numbered_params_count) {

	const String& name=*new String(cstr_name);
	
	Method& method=*new Method(
		call_type,
		min_numbered_params_count, max_numbered_params_count,
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, native_code
		);
	add_method(name, method);
}

/// VStateless_class: $CLASS, $method
Value* VStateless_class::get_element(const String& aname, Value& aself, bool looking_up) {
	// $CLASS
	if(aname==CLASS_NAME)
		return this;
	// $method=junction(self+class+method)
	if(Method* method=get_method(aname))
		return new VJunction(new Junction(aself, method));

	// base monkey
	if(fbase)
		if(Value* lbase=aself.base()) // one check would be enough...
			return fbase->get_element(aname, *lbase, looking_up);

	return 0;
}

inline Property& register_property(const String& aname, Hash<const String::Body, Property*>& aprops) {
	String prop_name=aname.mid(4, aname.length());
	Property* result=aprops.get(prop_name);
	if(!result) {
		result=new Property();
		aprops.put(prop_name, result);
	}
	return *result;
}

void VStateless_class::put_method(const String& aname, Method* amethod) {
	if(aname.starts_with("get_"))
		register_property(aname, fprops).getter=amethod;
	else if(aname.starts_with("put_") )
		register_property(aname, fprops).setter=amethod;
	else
		fmethods.put(aname, amethod); 
}

static void forward_cache_properties_one(const String::Body akey, Property* avalue, 
										 HashStringValue* acache) {
	acache->put(akey, new VProperty(*avalue));
}

void VStateless_class::fill_properties(HashStringValue& acache) {
	// base monkey
	if(fbase)
		fbase->fill_properties(acache);

	fprops.for_each(forward_cache_properties_one, &acache);
}
