/** @file
	Parser: @b request class decl.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

#define IDENT_PA_VREQUEST_H "$Id: pa_vrequest.h,v 1.43 2015/10/26 01:22:02 moko Exp $"

// includes

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_value.h"

// defines

#define REQUEST_CLASS_NAME "request"
#define REQUEST_ARGV_ELEMENT_NAME "argv"
#define REQUEST_BODY_CHARSET_NAME "body-charset"
#define REQUEST_BODY_BODY_NAME "body-file"
#define POST_CHARSET_NAME "post-charset"
#define POST_BODY_NAME "post-body"

static const String request_class_name(REQUEST_CLASS_NAME);

// forwards

class Request_info;
class VForm;

/// request class
class VRequest: public Value {

	Request_info& finfo;
	SAPI_Info& fsapi_info;
	Request_charsets& fcharsets;
	HashStringValue fargv;
	VForm& fform;

public: // Value
	
	override const char* type() const { return REQUEST_CLASS_NAME; }
	/// VRequest: 0
	override VStateless_class *get_class() { return 0; }

	/// request: field
	override Value* get_element(const String& name);

	/// request: (key)=value
	override const VJunction* put_element(const String& name, Value* value);

public: // usage

	VRequest(Request_info& ainfo, Request_charsets& acharsets, VForm& aform, SAPI_Info& asapi_info);

private:
	HashStringValue ffields;
	void fill();
};

#endif
