/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstateless_class.C,v 1.3 2001/03/13 20:02:11 paf Exp $
*/

#include "pa_vstateless_class.h"
#include "pa_vstring.h"

Value *VStateless_class::get_element(const String& aname) {
	// $CLASS=my class=myself
	if(aname==CLASS_NAME)
		return fclass_alias;
	// $BASE=my parent
	if(aname==BASE_NAME)
		return fclass_alias->base();
	// $method=junction(self+class+method)
	if(Junction *junction=get_junction(*this, aname))
		return NEW VJunction(*junction);

	return 0;
}

void VStateless_class::add_native_method(
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

void VStateless_class::put_method(const String& aname, Method *amethod) {
	if(read_only)
		THROW(0, 0,
			&aname,
			"can not add method to read-only '%s' class",
				name().cstr());
	
	fmethods.put(aname, amethod); 
}