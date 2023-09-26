/** @file
	Parser: @b request class decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

#define IDENT_PA_VREQUEST_H "$Id: pa_vrequest.h,v 1.47 2023/09/26 20:49:13 moko Exp $"

// includes

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_vstateless_class.h"

// defines

#define REQUEST_ARGV_ELEMENT_NAME "argv"
#define REQUEST_BODY_CHARSET_NAME "body-charset"
#define REQUEST_BODY_BODY_NAME "body-file"
#define POST_CHARSET_NAME "post-charset"
#define POST_BODY_NAME "post-body"

// forwards

class Request_info;
class VForm;

/// request class
class VRequest: public VStateless_class {

	Request_info& finfo;
	SAPI_Info& fsapi_info;
	Request_charsets& fcharsets;
	HashStringValue fargv;
	VForm& fform;

public: // Value
	
	override const char* type() const { return "request"; }

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
