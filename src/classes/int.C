/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: int.C,v 1.13 2001/03/13 11:52:44 paf Exp $
*/

#include "pa_request.h"
#include "_int.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "_string.h"

// global var

VClass *int_class;

// methods

static void _int(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VInt(pool, vint->get_int());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _double(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VDouble(pool, vint->get_double());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

typedef void (*vint_op_func_ptr)(VInt& vint, double param);

static void __inc(VInt& vint, double param) { vint.inc((int)param); }
static void __dec(VInt& vint, double param) { vint.inc((int)-param); }
static void __mul(VInt& vint, double param) { vint.mul(param); }
static void __div(VInt& vint, double param) { vint.div(param); }
static void __mod(VInt& vint, double param) { vint.mod((int)param); }

static void vint_op(Request& r, Array *params, 
					 vint_op_func_ptr func) {
	VInt *vint=static_cast<VInt *>(r.self);
	double param=params->size()?
		r.process(
			*static_cast<Value *>(params->get(0)),
			0/*no name*/,
			false/*don't intercept string*/).get_double():1;
	(*func)(*vint, param);
}

static void _inc(Request& r, const String&, Array *params) { vint_op(r, params, &__inc); }
static void _dec(Request& r, const String&, Array *params) { vint_op(r, params, &__dec); }
static void _mul(Request& r, const String&, Array *params) { vint_op(r, params, &__mul); }
static void _div(Request& r, const String&, Array *params) { vint_op(r, params, &__div); }
static void _mod(Request& r, const String&, Array *params) { vint_op(r, params, &__mod); }

void initialize_int_class(Pool& pool, VClass& vclass) {
	// ^int.int[]
	vclass.add_native_method("int", _int, 0, 0);

	// ^int.double[]
	vclass.add_native_method("double", _double, 0, 0);

	// ^int.inc[] 
	// ^int.inc[offset]
	vclass.add_native_method("inc", _inc, 0, 1);
	// ^int.dec[] 
	// ^int.dec[offset]
	vclass.add_native_method("dec", _dec, 0, 1);
	// ^int.mul[k] 
	vclass.add_native_method("mul", _mul, 1, 1);
	// ^int.div[d]
	vclass.add_native_method("div", _div, 1, 1);
	// ^int.mod[offset]
	vclass.add_native_method("mod", _mod, 1, 1);


	// ^int.format[]
	vclass.add_native_method("format", _string_format, 1, 1);
}
