/** @file
	Parser: @b response parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: response.C,v 1.6 2001/04/03 08:23:06 paf Exp $
*/

#include "pa_request.h"
#include "_response.h"

// global var

VStateless_class *response_class;

// methods

static void _clear(Request& r, const String& method_name, Array *params) {
	r.self/*VResponse*/->get_hash()/*sure not 0*/->clear();
}

// initialize

void initialize_response_class(Pool& pool, VStateless_class& vclass) {
	// ^clear[]
	vclass.add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 0);
}
