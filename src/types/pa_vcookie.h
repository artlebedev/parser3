/** @file
	Parser: @b cookie class decls.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCOOKIE_H
#define PA_VCOOKIE_H

#define IDENT_PA_VCOOKIE_H "$Id: pa_vcookie.h,v 1.38 2013/10/04 21:21:56 moko Exp $"

#include "pa_hash.h"
#include "pa_common.h"
#include "pa_value.h"
#include "pa_sapi.h"

#define COOKIE_CLASS_NAME "cookie"
static const String cookie_class_name(COOKIE_CLASS_NAME);

// forwards
class Request_info;
class Request_charsets;

/// cookie class
class VCookie: public Value {

	HashStringValue before;
	HashStringValue after;
	HashStringValue deleted;

public: // Value
	
	override const char* type() const { return COOKIE_CLASS_NAME; }
	/// VCookie: 0
	override VStateless_class *get_class() { return 0; }

	// cookie: CLASS, CLASS_NAME, field
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
