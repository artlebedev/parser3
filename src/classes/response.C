/** @file
	Parser: @b response parser class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_RESPONSE_C="$Date: 2005/08/09 08:14:48 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vresponse.h"

// class

class MResponse: public Methoded {
public:
	MResponse();
public: // Methoded
	bool used_directly() { return false; }
};

// global variable

DECLARE_CLASS_VAR(response, new MResponse, 0);

// methods

static void _clear(Request& r, MethodParams&) {
	GET_SELF(r, VResponse).fields().clear();
}

// constructor

MResponse::MResponse(): Methoded("response") {
	// ^clear[]
	add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 0);
}
