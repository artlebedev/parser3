/**	@file
	Parser: stateless class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

static const char* IDENT_VSTATELESS_CLASS_C="$Date: 2002/08/01 11:41:24 $";

#include "pa_vstateless_class.h"
#include "pa_vstring.h"

void VStateless_class::add_native_method(
	const char *cstr_name,
	Method::Call_type call_type,
	Native_code_ptr native_code,
	int min_numbered_params_count, int max_numbered_params_count) {

	String& name=*NEW String(pool(), cstr_name);
	
	Method& method=*NEW Method(pool(),
		name,
		call_type,
		min_numbered_params_count, max_numbered_params_count,
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, native_code
		);
	add_method(name, method);
}
