/** @file
	Parser: @b response parser class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vresponse.h"
#include "pa_vfile.h"

volatile const char * IDENT_RESPONSE_C="$Id: response.C,v 1.38 2026/10/14 23:18:32 moko Exp $";

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

static void _stream(Request& r, MethodParams& params) {
	Value& part=params[0];
	// checked here, before headers are sent
	if(!dynamic_cast<VFile*>(&part))
		params.as_string(0, "part must be string or file");
	r.stream(part);
}

// constructor

MResponse::MResponse(): Methoded("response") {
	// ^clear[]
	add_native_method("clear", Method::CT_STATIC, _clear, 0, 0);

	// ^stream[string or file]: the headers go with the first part, the result of @main is not sent then
	add_native_method("stream", Method::CT_STATIC, _stream, 1, 1);
}
