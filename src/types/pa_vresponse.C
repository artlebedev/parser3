/** @file
	Parser: @b response class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vresponse.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_vstring.h"
#include "pa_vdate.h"
#include "pa_vhash.h"

volatile const char * IDENT_PA_VRESPONSE_C="$Id: pa_vresponse.C,v 1.30 2012/03/16 09:24:19 moko Exp $" IDENT_PA_VRESPONSE_H;

// defines

#define REQUEST_HEADERS_ELEMENT_NAME "headers"

Value* VResponse::get_element(const String& aname) {
	// $charset
	if(aname==CHARSET_NAME)
		return new VString(*new String(fcharsets.client().NAME(), String::L_TAINTED));

	// $headers
	if(aname==REQUEST_HEADERS_ELEMENT_NAME)
		return new VHash(ffields);
	
	// $method
	if(Value* result=VStateless_object::get_element(aname))
		return result;

	// $field
	return ffields.get(aname.change_case(fcharsets.source(), String::CC_LOWER));
}

const VJunction* VResponse::put_element(const String& aname, Value* avalue, bool /*areplace*/) { 
	// guard charset change
	if(aname==CHARSET_NAME)
		fcharsets.set_client(charsets.get(avalue->as_string().
			change_case(UTF8_charset, String::CC_UPPER)));
	else
		ffields.put(
			aname.change_case(fcharsets.source(), String::CC_LOWER), 
			avalue && avalue->is_void()? 0: avalue);

	return PUT_ELEMENT_REPLACED_ELEMENT;
}
