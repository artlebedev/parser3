/** @file
	Parser: @b int parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: int.C,v 1.19 2001/04/15 13:12:17 paf Exp $
*/

#include "pa_request.h"
#include "_int.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "_string.h"

// global var

VStateless_class *int_class;

// methods

static void _int(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VInt(pool, vint->get_int());
	r.write_no_lang(value);
}

static void _double(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vint=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VDouble(pool, vint->as_double());
	r.write_no_lang(value);
}

typedef void (*vint_op_func_ptr)(VInt& vint, double param);

static void __inc(VInt& vint, double param) { vint.inc((int)param); }
static void __dec(VInt& vint, double param) { vint.inc((int)-param); }
static void __mul(VInt& vint, double param) { vint.mul(param); }
static void __div(VInt& vint, double param) { vint.div(param); }
static void __mod(VInt& vint, double param) { vint.mod((int)param); }

static void vint_op(Request& r, MethodParams *params, 
					 vint_op_func_ptr func) {
	VInt *vint=static_cast<VInt *>(r.self);
	double param=params->size()?
		r.process(
			params->get(0),
			0/*no name*/,
			false/*don't intercept string*/).as_double():1;
	(*func)(*vint, param);
}

static void _inc(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__inc); }
static void _dec(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__dec); }
static void _mul(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__mul); }
static void _div(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__div); }
static void _mod(Request& r, const String&, MethodParams *params) { vint_op(r, params, &__mod); }

// initialize

void initialize_int_class(Pool& pool, VStateless_class& vclass) {
	// ^int.int[]
	vclass.add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);

	// ^int.double[]
	vclass.add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);

	// ^int.inc[] 
	// ^int.inc[offset]
	vclass.add_native_method("inc", Method::CT_DYNAMIC, _inc, 0, 1);
	// ^int.dec[] 
	// ^int.dec[offset]
	vclass.add_native_method("dec", Method::CT_DYNAMIC, _dec, 0, 1);
	// ^int.mul[k] 
	vclass.add_native_method("mul", Method::CT_DYNAMIC, _mul, 1, 1);
	// ^int.div[d]
	vclass.add_native_method("div", Method::CT_DYNAMIC, _div, 1, 1);
	// ^int.mod[offset]
	vclass.add_native_method("mod", Method::CT_DYNAMIC, _mod, 1, 1);


	// ^int.format{format}
	vclass.add_native_method("format", Method::CT_DYNAMIC, _string_format, 1, 1);
}
