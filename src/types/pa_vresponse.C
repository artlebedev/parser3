/** @file
	Parser: @b response class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_vresponse.h"
#include "pa_request_charsets.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_vstring.h"
#include "pa_vdate.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"

volatile const char * IDENT_PA_VRESPONSE_C="$Id: pa_vresponse.C,v 1.47 2026/10/16 13:55:16 moko Exp $" IDENT_PA_VRESPONSE_H;

// defines

#define REQUEST_HEADERS_ELEMENT_NAME "headers"
#define UNTAINT_ELEMENT_NAME "untaint"
#define STARTED_ELEMENT_NAME "started"

Value* VResponse::get_element(const String& aname) {
	// $charset
	if(aname==CHARSET_NAME)
		return new VString(fcharsets.client().NAME());

	// $headers
	if(aname==REQUEST_HEADERS_ELEMENT_NAME)
		return new VHash(ffields);

	// $untaint
	if(aname==UNTAINT_ELEMENT_NAME)
		return new VString(*new String(String::lang2name(lang)));

	// $started
	if(aname==STARTED_ELEMENT_NAME) {
		if(started==STREAM)
			return new VString(*new String("stream"));
		if(started==WEBSOCKET)
			return new VString(*new String("websocket"));
		return VVoid::get();
	}

	MAYBE_GET_SPECIAL_OR_METHOD(VStateless_class::get_element(aname));

	// $field
	return ffields.get(aname.change_case(fcharsets.source(), String::CC_UPPER));
}

const VJunction* VResponse::put_element(const String& aname, Value* avalue) { 
	// guard charset change
	if(aname==CHARSET_NAME)
		fcharsets.set_client(pa_charsets.get(avalue->as_string()));
	else if(aname==UNTAINT_ELEMENT_NAME)
		lang=String::name2lang(avalue->as_string());
	else if(aname==STARTED_ELEMENT_NAME)
		return Value::put_element(aname, avalue);
	else
		ffields.put(aname.change_case(fcharsets.source(), String::CC_UPPER), !avalue || (avalue->is_string() && !avalue->is_defined()) ? 0: avalue);

	return 0;
}
