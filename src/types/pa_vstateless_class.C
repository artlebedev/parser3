/**	@file
	Parser: stateless class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

static const char * const IDENT_VSTATELESS_CLASS_C="$Date: 2008/08/14 16:27:40 $";

#include "pa_vstateless_class.h"
#include "pa_vproperty.h"
#include "pa_vstring.h"
#include "pa_vbool.h"

override Value& VStateless_class::as_expr_result(bool /*return_string_as_is=false*/) {
	return *new VBool(as_bool());
}

/// @TODO why?! request must be different ptr from global [used in VStateless_class.add_method]
void VStateless_class::add_method(const String& name, Method& method) {
	if(flocked)
		throw Exception(PARSER_RUNTIME,
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

/// VStateless_class: $CLASS, $CLASS_NAME, $method
Value* VStateless_class::get_element(const String& aname, Value& aself, bool alooking_up) {
	// $CLASS
	if(aname==CLASS_NAME)
		return this;
	// $CLASS_NAME
	if(aname==CLASS_NAMETEXT)
		return new VString(this->name());
	// $method=junction(self+class+method)
	if(Method* method=get_method(aname))
		return new VJunction(aself, method);

	// base monkey
	if(fbase) {
		if(Value* obase=aself.base()) // MXdoc has fbase but does not have object_base[ base() ]
			return fbase->get_element(aname, *obase, alooking_up);
	}

	return 0;
}

void VStateless_class::put_method(const String& aname, Method* amethod){
	fmethods.put(aname, amethod); 
}

Value* VStateless_class::get_default_getter(Value& aself, const String& aname){
	if(fdefault_getter)
		return new VJunction(aself, fdefault_getter, true /*getter*/, (String*)&aname);

	if(fbase)
		if(Value* obase=aself.base())
			return fbase->get_default_getter(*obase, aname);

	return 0;
}

void VStateless_class::set_default_getter(Method* amethod){
	fdefault_getter=amethod;
}

Value* VStateless_class::get_scalar(Value& aself){
	if(fscalar)
		return new VJunction(aself, fscalar, true /*getter*/);

	if(fbase)
		if(Value* obase=aself.base())
			return fbase->get_scalar(*obase);

	return 0;
}

void VStateless_class::set_scalar(Method* amethod){
	fscalar=amethod;
}
