/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: cookie.C,v 1.1 2001/03/18 20:31:24 paf Exp $
*/

#include "pa_request.h"
#include "_cookie.h"

// global var

VStateless_class *cookie_base_class;

// methods

static void _clear(Request& r, const String& method_name, Array *params) {
	//r.self/*VResponse*/->get_hash()/*sure not 0*/->clear();
}

// initialize

void initialize_cookie_base_class(Pool& pool, VStateless_class& vclass) {
	// ^clear[]
	vclass.add_native_method("clear", _clear, 0, 0);
}
