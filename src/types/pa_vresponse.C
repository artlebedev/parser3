/** @file
	Parser: @b response class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VRESPONSE_C="$Date: 2002/08/14 14:18:31 $";

#include "pa_vresponse.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_vstring.h"

Value *VResponse::get_element(const String& aname, Value *aself, bool looking_up) {
	// $charset
	if(aname==CHARSET_NAME)
		return NEW VString(pool().get_client_charset().name());

	// $method
	if(Value *result=VStateless_object::get_element(aname, aself, looking_up))
		return result;
	
	// $field
	return static_cast<Value *>(ffields.get(aname));
}

bool VResponse::put_element(const String& aname, Value *avalue, bool /*replace*/) { 
	// guard charset change
	if(aname==CHARSET_NAME)
		pool().set_client_charset(charsets->get_charset(avalue->as_string()));
	else
		ffields.put(aname, avalue);

	return true;
}

