/** @file
	Parser: response class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

static const char * const IDENT_VRESPONSE_H="$Date: 2005/08/09 08:14:55 $";

#include "pa_vstateless_object.h"
#include "pa_string.h"
#include "classes.h"

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
	
	override const char* type() const { return "response"; }
	override VStateless_class *get_class() { return response_class; }

	/// Response: ffields
	override HashStringValue* get_hash() { return &ffields; }

	/// Response: method,fields
	override Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);

	/// Response: (attribute)=value
	override const VJunction* put_element(Value& self, const String& name, Value* value, bool replace);

public:	// usage

	VResponse(Request_info& ainfo, Request_charsets& acharsets): 
		finfo(ainfo), fcharsets(acharsets) {}

	/// used in pa_request.C
	HashStringValue& fields() { return ffields; }

};


#endif
