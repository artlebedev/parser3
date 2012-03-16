/** @file
	Parser: response class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

#define IDENT_PA_VRESPONSE_H "$Id: pa_vresponse.h,v 1.43 2012/03/16 09:24:19 moko Exp $"

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
class VResponse: public VStateless_object {

	Request_info& finfo;
	Request_charsets& fcharsets;

	HashStringValue ffields;

public: // Value
	
	override const char* type() const { return RESPONSE_CLASS_NAME; }
	override VStateless_class *get_class() { return response_class; }

	/// Response: ffields
	override HashStringValue* get_hash() { return &ffields; }

	/// Response: method,fields
	override Value* get_element(const String& aname);

	/// Response: (attribute)=value
	override const VJunction* put_element(const String& name, Value* value, bool replace);

public:	// usage

	VResponse(Request_info& ainfo, Request_charsets& acharsets): 
		finfo(ainfo), fcharsets(acharsets) {}

	/// used in pa_request.C
	HashStringValue& fields() { return ffields; }

};


#endif
