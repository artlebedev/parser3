/** @file
	Parser: @b request class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: pa_vrequest.C,v 1.11 2001/11/05 11:46:33 paf Exp $
*/

#include "pa_vrequest.h"
#include "pa_request.h"

// request: CLASS,method,field
Value *VRequest::get_element(const String& aname) {
	// $query $uri 
	const char *cstr=0;
	if(aname=="query")
		cstr=frequest.info.query_string;
	if(aname=="uri")
		cstr=frequest.info.uri;
	
	String& string=*NEW String(pool());
	string.APPEND_TAINTED(cstr?cstr:"", 0, "request", 0);
	return NEW VString(string);
}

