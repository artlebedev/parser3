/** @file
	Parser: @b response class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VRESPONSE_C="$Date: 2002/08/01 11:41:24 $";

#include "pa_vresponse.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_vstring.h"

Value *VResponse::get_element(const String& name) {
	// $charset
	if(name==CHARSET_NAME)
		return NEW VString(pool().get_client_charset().name());
	else {
		// $method
		if(Value *result=VStateless_object::get_element(name))
			return result;
		
		// $field
		return static_cast<Value *>(ffields.get(name));
	}
}

void VResponse::put_element(const String& name, Value *value) { 
	// guard charset change
	if(name==CHARSET_NAME)
		pool().set_client_charset(charsets->get_charset(value->as_string()));
	else
		ffields.put(name, value);		
}

