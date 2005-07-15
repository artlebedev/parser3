/** @file
	Parser: @b cookie class decls.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCOOKIE_H
#define PA_VCOOKIE_H

static const char * const IDENT_VCOOKIE_H="$Date: 2005/07/15 06:16:42 $";

#include "pa_hash.h"
#include "pa_common.h"
#include "pa_value.h"
#include "pa_request_info.h"
#include "pa_sapi.h"

#define COOKIE_CLASS_NAME "cookie"

/// cookie class
class VCookie: public Value {

	HashStringValue before, after, deleted;

public: // Value
	
	override const char* type() const { return "cookie"; }
	/// VCookie: 0
	override VStateless_class *get_class() { return 0; }

	// cookie: CLASS,method,field
	override Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);
	// cookie: field
	override const Method* put_element(const String& aname, Value* avalue, bool replace);

public: // usage

	void fill_fields(Request_info& request_info);
	void output_result(SAPI_Info& sapi_info);

};

#endif
