/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstateless_class.C,v 1.6 2001/03/24 09:44:20 paf Exp $
*/

#include "pa_vstateless_class.h"
#include "pa_vstring.h"

void VStateless_class::add_native_method(
	const char *cstr_name,
	Native_code_ptr native_code,
	int min_numbered_params_count, int max_numbered_params_count) {

	String& name=*NEW String(pool(), cstr_name);
	
	Method& method=*NEW Method(pool(),
		name,
		min_numbered_params_count, max_numbered_params_count,
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, native_code
		);
	add_method(name, method);
}
