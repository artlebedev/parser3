/** @file
	Parser: @b response parser class.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vresponse.h"

volatile const char * IDENT_RESPONSE_C="$Id: response.C,v 1.31 2016/04/06 16:08:19 moko Exp $";

// class

class MResponse: public Methoded {
public:
	MResponse();
public: // Methoded
	bool used_directly() { return false; }
};

// global variable

DECLARE_CLASS_VAR(response, new MResponse);

// methods

static void _clear(Request& r, MethodParams&) {
	GET_SELF(r, VResponse).fields().clear();
}

// constructor

MResponse::MResponse(): Methoded("response") {
	// ^clear[]
	add_native_method("clear", Method::CT_STATIC, _clear, 0, 0);
}
