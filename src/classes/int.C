/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: int.C,v 1.9 2001/03/11 12:04:42 paf Exp $
*/

#include "pa_request.h"
#include "_int.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// global var

VClass *int_class;

// methods

static void _int(Request& r, Array *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VInt(pool, vint->get_int());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _double(Request& r, Array *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VDouble(pool, vint->get_double());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _inc(Request& r, Array *params) {
	VInt *vint=static_cast<VInt *>(r.self);
	int increment=params->size()?
		static_cast<int>(r.autocalc(
			*static_cast<Value *>(params->get(0)),
			0/*no name*/,
			false/*don't intercept string*/).get_double()):1;
	vint->inc(increment);
}

void initialize_int_class(Pool& pool, VClass& vclass) {
	// ^int.int[]
	vclass.add_native_method("int", _int, 0, 0);

	// ^int.double[]
	vclass.add_native_method("double", _double, 0, 0);

	// ^int.inc[] 
	// ^int.inc[offset]
	vclass.add_native_method("inc", _inc, 0, 1);
}
