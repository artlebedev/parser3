/** @file
	Parser: @b cookie class decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCOOKIE_H
#define PA_VCOOKIE_H

static const char* IDENT_VCOOKIE_H="$Date: 2002/08/13 15:55:43 $";

#include "pa_hash.h"
#include "pa_common.h"
#include "pa_value.h"

class Request;

#define COOKIE_CLASS_NAME "cookie"

/// cookie class
class VCookie : public Value {
public: // Value
	
	const char *type() const { return "cookie"; }
	/// VCookie: 0
	VStateless_class *get_class() { return 0; }

	// cookie: CLASS,method,field
	Value *get_element(const String& aname, Value *aself, bool /*looking_down*/);
	// cookie: field
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace);

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
