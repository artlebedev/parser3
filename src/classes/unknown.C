/** @file
	Parser: @b unknown parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: unknown.C,v 1.7 2001/05/11 17:45:10 parser Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vint.h"
#include "pa_vdouble.h"
#include "pa_vunknown.h"

// defines

#define UNKNOWN_CLASS_NAME "UNKNOWN"

// class

class MUnknown : public Methoded {
public:
	MUnknown(Pool& pool);
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

MUnknown::MUnknown(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), UNKNOWN_CLASS_NAME));


	// ^unknown.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);

	// ^unknown.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);
}

// global variable

Methoded *unknown_class;

// creator

Methoded *MUnknown_create(Pool& pool) {
	return unknown_class=new(pool) MUnknown(pool);
}
