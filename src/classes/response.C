/** @file
	Parser: @b response parser class.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_RESPONSE_C="$Date: 2003/01/21 15:51:07 $";

#include "classes.h"
#include "pa_request.h"

// class

class MResponse : public Methoded {
public:
	MResponse(Pool& pool);
public: // Methoded
	bool used_directly() { return false; }
};

// methods

static void _clear(Request& r, const String& method_name, MethodParams *) {
	r.get_self()/*VResponse*/->get_hash(&method_name)/*sure not 0*/->clear();
}

// constructor

MResponse::MResponse(Pool& apool) : Methoded(apool,  "response") {
	// ^clear[]
	add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 0);
}
// global variable

Methoded *response_class;

// creator

Methoded *MResponse_create(Pool& pool) {
	return response_class=new(pool) MResponse(pool);
}
