/** @file
	Parser: @b response class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vresponse.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_vstring.h"
#include "pa_vdate.h"
#include "pa_vhash.h"

volatile const char * IDENT_PA_VRESPONSE_C="$Id: pa_vresponse.C,v 1.40 2020/12/15 17:10:43 moko Exp $" IDENT_PA_VRESPONSE_H;

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
	if(Value* result=VStateless_class::get_element(aname))
		return result;

	// $field
	return ffields.get(aname.change_case(fcharsets.source(), String::CC_UPPER));
}

const VJunction* VResponse::put_element(const String& aname, Value* avalue) { 
	// guard charset change
	if(aname==CHARSET_NAME)
		fcharsets.set_client(pa_charsets.get(avalue->as_string()));
	else
		ffields.put(aname.change_case(fcharsets.source(), String::CC_UPPER), !avalue || (avalue->is_string() && !avalue->is_defined()) ? 0: avalue);

	return 0;
}
