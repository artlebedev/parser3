/** @file
	Parser: @b request class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

static const char * const IDENT_VREQUEST_H="$Date: 2008/06/07 17:28:50 $";

// includes

#include "pa_common.h"
#include "pa_value.h"

// defines

#define REQUEST_CLASS_NAME "request"
#define REQUEST_ARGV_ELEMENT_NAME "argv"
#define POST_CHARSET_NAME "post-charset"

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

	/// request: fields
	override Value* get_element(const String& name, Value& aself, bool /*looking_up*/);

	/// request: (key)=value
	override const VJunction* put_element(Value& self, const String& name, Value* value, bool replace);

public: // usage

	VRequest(Request_info& ainfo, Request_charsets& acharsets, VForm& aform);

};

#endif
