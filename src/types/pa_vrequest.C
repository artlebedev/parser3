/** @file
	Parser: @b request class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VREQUEST_C="$Date: 2002/08/13 15:55:44 $";

#include "pa_vrequest.h"
#include "pa_request.h"
#include "pa_charsets.h"
#include "pa_charset.h"

// request: CLASS,method,field
Value *VRequest::get_element(const String& name, Value * /*aself*/, bool /*looking_down*/) {
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

bool VRequest::put_element(const String& aname, Value *avalue, bool replace) {
	// guard charset change
	if(aname==CHARSET_NAME) {
		pool().set_source_charset(charsets->get_charset(avalue->as_string()));
		return true;
	} else
		return Value::put_element(aname, avalue, replace);
}
