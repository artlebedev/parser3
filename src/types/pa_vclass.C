/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vclass.C,v 1.3 2001/03/11 08:44:42 paf Exp $
*/

#include "pa_vclass.h"
#include "pa_vstring.h"

Value *VClass::get_element(const String& aname) {
	// $NAME=my name
	if(aname==NAME_NAME)
		return NEW VString(fclass_alias->name());
	// $CLASS=my class=myself
	if(aname==CLASS_NAME)
		return fclass_alias;
	// $BASE=my parent
	if(aname==BASE_NAME)
		return fclass_alias->base();
	// $method=junction(self+class+method)
	if(Junction *junction=get_junction(*this, aname))
		return NEW VJunction(*junction);
	// $field=static field
	return get_field(aname);
}

// object_class, operator_class: (field)=value - static values only
void VClass::put_element(const String& name, Value *value) {
	set_field(name, value);
}

void VClass::add_native_method(
	const char *cstr_name,
	Native_code_ptr native_code,
	int min_numbered_params_count, int max_numbered_params_count) {

	String& name=*NEW String(pool());  name.APPEND_CONST(cstr_name);
	
	Method& method=*NEW Method(pool(),
		name,
		min_numbered_params_count, max_numbered_params_count,
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, native_code
		);
	add_method(name, method);
}