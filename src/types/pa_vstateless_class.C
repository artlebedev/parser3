/**	@file
	Parser: stateless class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

static const char * const IDENT_VSTATELESS_CLASS_C="$Date: 2004/02/11 15:33:19 $";

#include "pa_vstateless_class.h"

/// @TODO why? request must be different ptr from global [used in VStateless_class.add_method]
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
		return new VJunction(new Junction(aself, method, 0, 0, 0, 0));

	// base monkey
	if(fbase)
		if(Value* lbase=aself.base()) // one check would be enough...
			return fbase->get_element(aname, *lbase, looking_up);

	return 0;
}
