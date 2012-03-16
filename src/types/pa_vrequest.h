/** @file
	Parser: @b request class decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

#define IDENT_PA_VREQUEST_H "$Id: pa_vrequest.h,v 1.39 2012/03/16 09:24:19 moko Exp $"

// includes

#include "pa_common.h"
#include "pa_value.h"

// defines

#define REQUEST_CLASS_NAME "request"
#define REQUEST_ARGV_ELEMENT_NAME "argv"
#define POST_CHARSET_NAME "post-charset"
#define POST_BODY_NAME "post-body"

static const String request_class_name(REQUEST_CLASS_NAME);

// forwards

class Request_info;
class VForm;

/// request class
class VRequest: public Value {

	Request_info& finfo;
	Request_charsets& fcharsets;
	HashStringValue fargv;
	VForm& fform;

public: // Value
	
	override const char* type() const { return REQUEST_CLASS_NAME; }
	/// VRequest: 0
	override VStateless_class *get_class() { return 0; }

	/// request: CLASS,CLASS_NAME,field
	override Value* get_element(const String& name);

	/// request: (key)=value
	override const VJunction* put_element(const String& name, Value* value, bool replace);

public: // usage

	VRequest(Request_info& ainfo, Request_charsets& acharsets, VForm& aform);

};

#endif
