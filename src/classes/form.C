/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: form.C,v 1.1 2001/03/13 17:54:12 paf Exp $
*/

#include "pa_request.h"
#include "_form.h"
#include "pa_vform.h"

// global var

VStateless_class *form_base_class;

// methods

static void _fields(Request& r, const String&, Array *params) { 
	Pool& pool=r.pool();
	VForm *vform=static_cast<VForm *>(r.self);
	r.write_no_lang(*new(pool) VHash(pool, &vform->fields())); 
}

// initialize

void initialize_form_base_class(Pool& pool, VStateless_class& vclass) {
	// ^form:fields[]
	vclass.add_native_method("fields", _fields, 0, 0);
}
