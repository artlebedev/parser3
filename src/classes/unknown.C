/** @file
	Parser: @b unknown parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: unknown.C,v 1.2 2001/04/15 13:12:18 paf Exp $
*/

#include "pa_request.h"
#include "_unknown.h"
#include "pa_vint.h"
#include "pa_vdouble.h"
#include "pa_vunknown.h"

// global var

VStateless_class *unknown_class;

// methods

static void _int(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vunknown=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VInt(pool, (int)vunknown->as_double());
	r.write_no_lang(value);
}

static void _double(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vunknown=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VDouble(pool, vunknown->as_double());
	r.write_no_lang(value);
}


// initialize

void initialize_unknown_class(Pool& pool, VStateless_class& vclass) {
	// ^unknown.int[]
	vclass.add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);

	// ^unknown.double[]
	vclass.add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);
}
