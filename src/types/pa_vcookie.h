/** @file
	Parser: cookie class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vcookie.h,v 1.6 2001/03/19 22:11:11 paf Exp $
*/

#ifndef PA_VCOOKIE_H
#define PA_VCOOKIE_H

#include "pa_common.h"
#include "pa_value.h"

class Request;

/// cookie class
class VCookie : public Value {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "cookie"; }

	// cookie: CLASS,BASE,method,field
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
