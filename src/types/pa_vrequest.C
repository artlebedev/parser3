/*
	Parser
	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vrequest.C,v 1.1 2001/03/16 12:30:26 paf Exp $
*/

#include "pa_vrequest.h"
#include "pa_request.h"

// request: CLASS,BASE,method,field
Value *VRequest::get_element(const String& aname) {
	// $CLASS,$BASE,$method
	if(Value *result=VStateless_class::get_element(aname))
		return result;
	
	// $query $uri 
	const char *cstr=0;
	if(aname=="query")
		cstr=frequest.info.query_string;
	if(aname=="uri")
		cstr=frequest.info.uri;
	if(!cstr)
		return 0;
	
	String& string=*NEW String(pool());
	string.APPEND_TAINTED(cstr, 0, "request", 0);
	return NEW VString(string);
}

