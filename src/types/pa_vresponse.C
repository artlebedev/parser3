/** @file
	Parser: @b response class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: pa_vresponse.C,v 1.1 2001/12/15 21:28:22 paf Exp $
*/

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
	if(name==CHARSET_NAME) {
		const String& charset_name=value->as_string();
		if(Charset *charset=(Charset *)charsets->get(charset_name))
			pool().set_client_charset(*charset);
		else
			throw Exception(0, 0,
				&charset_name,
				"unknown charset");
	} else
		ffields.put(name, value);		
}

