/** @file
	Parser: @b nothing parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: nothing.C,v 1.1 2001/05/21 17:11:57 parser Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vint.h"
#include "pa_vdouble.h"
#include "pa_vunknown.h"

// defines

#define NOTHING_CLASS_NAME "nothing"

// class

class MNothing : public Methoded {
public:
	MNothing(Pool& pool);
public: // Methoded
	bool used_directly() { return false; }
};

// methods

static void _int(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vunknown=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VInt(pool, vunknown->as_int());
	r.write_no_lang(value);
}

static void _double(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vunknown=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VDouble(pool, vunknown->as_double());
	r.write_no_lang(value);
}

// constructor

MNothing::MNothing(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), NOTHING_CLASS_NAME));


	// ^nothing.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);

	// ^nothing.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);
}

// global variable

Methoded *unknown_class;

// creator

Methoded *MNothing_create(Pool& pool) {
	return unknown_class=new(pool) MNothing(pool);
}
