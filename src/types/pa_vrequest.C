/** @file
	Parser: @b request class.

	Copyright(c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VREQUEST_C="$Date: 2005/07/15 06:16:42 $";

#include "pa_vrequest.h"
#include "pa_request_info.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"
#include "pa_vstring.h"

// defines

#define DOCUMENT_ROOT_NAME "document-root"


// request: CLASS,method,field
Value* VRequest::get_element(const String& aname, Value&  /*aself*/, bool /*looking_up*/) {
	// $charset
	if(aname==CHARSET_NAME)
		return new VString(*new String(fcharsets.source().NAME(), String::L_TAINTED));
	
	// $query $uri $document-root $body
	const char* buf;
	size_t size=0;
	if(aname=="query")
		buf=finfo.query_string;
	else if(aname=="uri")
		buf=finfo.uri;
	else if(aname==DOCUMENT_ROOT_NAME)
		buf=finfo.document_root;
	else if(aname=="body") {
		buf=finfo.post_data;
		size=finfo.post_size;
	} else
		return bark("%s field not found", &aname);

	return new VString(*new String(buf?buf:"", size, true));
}

const Method* VRequest::put_element(const String& aname, Value* avalue, bool replace) {
	// $charset
	if(aname==CHARSET_NAME) {
		fcharsets.set_source(charsets.get(avalue->as_string().
			change_case(UTF8_charset, String::CC_UPPER)));
		return PUT_ELEMENT_REPLACED_ELEMENT;
	} 

	// $document-root
	if(aname==DOCUMENT_ROOT_NAME) {
		finfo.document_root=avalue->as_string().cstr(String::L_FILE_SPEC);
		return PUT_ELEMENT_REPLACED_ELEMENT;
	} 

	return Value::put_element(aname, avalue, replace);
}
