/** @file
	Parser: @b request class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VREQUEST_C="$Date: 2002/08/01 11:41:24 $";

#include "pa_vrequest.h"
#include "pa_request.h"
#include "pa_charsets.h"
#include "pa_charset.h"

// request: CLASS,method,field
Value *VRequest::get_element(const String& name) {
	// $charset
	if(name==CHARSET_NAME)
		return NEW VString(pool().get_source_charset().name());
	else {	
		// $query $uri  
		const char *cstr=0;
		if(name=="query")
			cstr=frequest.info.query_string;
		else if(name=="uri")
			cstr=frequest.info.uri;

		String& string=*NEW String(pool());
		string.APPEND_TAINTED(cstr?cstr:"", 0, "request", 0);
		return NEW VString(string);
	}
}

void VRequest::put_element(const String& name, Value *value) {
	// guard charset change
	if(name==CHARSET_NAME)
		pool().set_source_charset(charsets->get_charset(value->as_string()));
	else
		Value::put_element(name, value);
}
