/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: double.C,v 1.10 2001/03/12 09:08:45 paf Exp $
*/

#include "pa_request.h"
#include "_double.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// global var

VClass *double_class;

// methods

static void _int(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	Value& value=*new(pool) VInt(pool, static_cast<int>(vdouble->get_double()));
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _double(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	Value& value=*new(pool) VDouble(pool, vdouble->get_double());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _inc(Request& r, const String&, Array *params) {
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	double increment=params->size()?
		r.process(
			*static_cast<Value *>(params->get(0)),
			0/*no name*/,
			false/*don't intercept string*/).get_double():1;
	vdouble->inc(increment);
}

void initialize_double_class(Pool& pool, VClass& vclass) {
	// ^double.int[]
	vclass.add_native_method("int", _int, 0, 0);

	// ^double.double[]
	vclass.add_native_method("double", _double, 0, 0);
	
	// ^double.inc[]
	// ^double.inc[offset]
	vclass.add_native_method("inc", _inc, 0, 1);
}

