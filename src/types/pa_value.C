/** @file
	Parser: Value class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VALUE_C="$Date: 2002/08/01 11:41:21 $";

#include "pa_value.h"
#include "pa_vstateless_class.h"

/// call this before invoking to ensure proper actual numbered params count
void Method::check_actual_numbered_params(
	Value& self, const String& actual_name, Array *actual_numbered_params) const {

	int actual_count=actual_numbered_params?actual_numbered_params->size():0;
	if(actual_count<min_numbered_params_count) // not proper count? bark
		throw Exception("parser.runtime",
			&actual_name,
			"native method of %s (%s) accepts minimum %d parameter(s) (%d present)", 
				self.get_class()->name_cstr(),
				self.type(),
				min_numbered_params_count,
				actual_count);

}
