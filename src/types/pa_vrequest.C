/** @file
	Parser: @b request class.

	Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VREQUEST_C="$Date: 2003/08/19 12:07:35 $";

#include "pa_vrequest.h"
#include "pa_request_info.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"
#include "pa_vstring.h"

// request: CLASS,method,field
Value* VRequest::get_element(const String& aname, Value&  /*aself*/, bool /*looking_up*/) {
	// $charset
	if(aname==CHARSET_NAME)
		return new VString(*new String(fcharsets.source().NAME(), String::L_TAINTED));
	else {	
		// $query $uri $body
		const char* buf;
		size_t size=0;
		if(aname=="query")
			buf=finfo.query_string;
		else if(aname=="uri")
			buf=finfo.uri;
		else if(aname=="body") {
			buf=finfo.post_data;
			size=finfo.post_size;
		} else
			return bark("%s field not found", &aname);

		return new VString(*new String(buf?buf:"", size, true));
	}
}

bool VRequest::put_element(const String& aname, Value* avalue, bool replace) {
	// guard charset change
	if(aname==CHARSET_NAME) {
		fcharsets.set_source(charsets.get(avalue->as_string().
			change_case(UTF8_charset, String::CC_UPPER)));
		return true;
	} else
		return Value::put_element(aname, avalue, replace);
}
