/** @file
	Parser: response class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

#define IDENT_PA_VRESPONSE_H "$Id: pa_vresponse.h,v 1.53 2026/10/16 13:55:16 moko Exp $"

#include "pa_vstateless_object.h"
#include "pa_string.h"
#include "classes.h"

// defines
#define RESPONSE_CLASS_NAME "response"

// forwards

class Response;
class Request_info;
class Request_charsets;

// externals

extern Methoded* response_class;

/// value of type 'response'
class VResponse: public VStateless_class {

	Request_info& finfo;
	Request_charsets& fcharsets;

	HashStringValue ffields;

public: // Value
	
	override const char* type() const { return RESPONSE_CLASS_NAME; }

	/// Response: ffields
	override HashStringValue* get_hash() { return &ffields; }

	/// Response: method,fields
	override Value* get_element(const String& aname);

	/// Response: (attribute)=value
	override const VJunction* put_element(const String& name, Value* value);

public:	// usage

	/// $response:untaint, response data untainting lang
	String::Language lang;
	/// $response:untaint is reset to it in @unhandled_exception
	const String::Language default_lang;

	/// if not UNSTARTED, the headers are already sent and the result of @main or exception is not sent
	enum Started { UNSTARTED=0, STREAM, WEBSOCKET };
	/// $response:started
	Started started;

	VResponse(Request_info& ainfo, Request_charsets& acharsets, String::Language alang): VStateless_class(response_class), finfo(ainfo), fcharsets(acharsets), lang(alang), default_lang(alang), started(UNSTARTED) {}

	/// used in pa_request.C
	HashStringValue& fields() { return ffields; }

};


#endif
