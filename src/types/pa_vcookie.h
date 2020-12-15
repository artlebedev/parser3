/** @file
	Parser: @b cookie class decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCOOKIE_H
#define PA_VCOOKIE_H

#define IDENT_PA_VCOOKIE_H "$Id: pa_vcookie.h,v 1.43 2020/12/15 17:10:39 moko Exp $"

#include "pa_hash.h"
#include "pa_common.h"
#include "pa_vstateless_class.h"
#include "pa_sapi.h"

// forwards
class Request_info;
class Request_charsets;

/// cookie class
class VCookie: public VStateless_class {

	HashStringValue before;
	HashStringValue after;
	HashStringValue deleted;

public: // Value
	
	override const char* type() const { return "cookie"; }

	// cookie: field
	override Value* get_element(const String& aname);
	// cookie: field
	override const VJunction* put_element(const String& name, Value* value);

public: // usage

	VCookie(Request_charsets& acharsets, Request_info& arequest_info);
	void output_result(SAPI_Info& sapi_info);

private:
	Request_charsets& fcharsets;
	Request_info& frequest_info;

private:
	Charset* filled_source;
	Charset* filled_client;

	bool should_refill();
	void refill();
};

#endif
