/** @file
	Parser: @b request class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: pa_vrequest.C,v 1.13 2001/12/15 21:28:22 paf Exp $
*/

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
	if(name==CHARSET_NAME) {
		const String& charset_name=value->as_string();
		if(Charset *charset=(Charset *)charsets->get(charset_name))
			pool().set_source_charset(*charset);
		else
			throw Exception(0, 0,
				&charset_name,
				"unknown charset");
	} else
		Value::put_element(name, value);
}
