/** @file
	Parser: @b response class.

	Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VRESPONSE_C="$Date: 2003/11/20 16:34:30 $";

#include "pa_vresponse.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_vstring.h"
#include "pa_vdate.h"

Value* VResponse::get_element(const String& aname, Value& aself, bool looking_up) {
	// $charset
	if(aname==CHARSET_NAME)
		return new VString(*new String(fcharsets.client().NAME(), String::L_TAINTED));

	// $method
	if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
		return result;
	
	// $field
	return ffields.get(aname.change_case(fcharsets.source(), String::CC_LOWER));
}

bool VResponse::put_element(const String& aname, Value* avalue, bool /*replace*/) { 
	// guard charset change
	if(aname==CHARSET_NAME)
		fcharsets.set_client(charsets.get(avalue->as_string().
			change_case(UTF8_charset, String::CC_UPPER)));
	else
		ffields.put(
			aname.change_case(fcharsets.source(), String::CC_LOWER), 
			avalue);

	return true;
}
