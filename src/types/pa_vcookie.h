/** @file
	Parser: @b cookie class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vcookie.h,v 1.13 2001/11/05 11:46:32 paf Exp $
*/

#ifndef PA_VCOOKIE_H
#define PA_VCOOKIE_H

#include "pa_hash.h"
#include "pa_common.h"
#include "pa_value.h"

class Request;

#define COOKIE_CLASS_NAME "cookie"

/// cookie class
class VCookie : public Value {
public: // Value
	
	const char *type() const { return "cookie"; }

	// cookie: CLASS,method,field
	Value *get_element(const String& aname);
	// cookie: field
	void put_element(const String& aname, Value *avalue);

public: // usage

	VCookie(Pool& apool) : Value(apool),
		before(apool), after(apool), deleted(apool) {
	}

	void fill_fields(Request& request);
	void output_result();

private:

	Hash before, after, deleted;

};

#endif
