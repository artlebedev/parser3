/** @file
	Parser: @b response parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: response.C,v 1.14 2001/09/26 10:32:25 parser Exp $
*/

#include "classes.h"
#include "pa_request.h"

// defines

#define RESPONSE_CLASS_NAME "response"

// class

class MResponse : public Methoded {
public:
	MResponse(Pool& pool);
public: // Methoded
	bool used_directly() { return false; }
};

// methods

static void _clear(Request& r, const String&, MethodParams *) {
	r.self/*VResponse*/->get_hash()/*sure not 0*/->clear();
}

// constructor

MResponse::MResponse(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), RESPONSE_CLASS_NAME));


	// ^clear[]
	add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 0);
}
// global variable

Methoded *response_class;

// creator

Methoded *MResponse_create(Pool& pool) {
	return response_class=new(pool) MResponse(pool);
}
