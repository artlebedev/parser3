/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: double.C,v 1.17 2001/03/16 09:52:57 paf Exp $
*/

#include "pa_request.h"
#include "_double.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "_string.h"

// global var

VStateless_class *double_class;

// methods

static void _int(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	Value& value=*new(pool) VInt(pool, (int)vdouble->get_double());
	r.write_no_lang(value);
}

static void _double(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	Value& value=*new(pool) VDouble(pool, vdouble->get_double());
	r.write_no_lang(value);
}

typedef void (*vdouble_op_func_ptr)(VDouble& vdouble, double param);

static void __inc(VDouble& vdouble, double param) { vdouble.inc(param); }
static void __dec(VDouble& vdouble, double param) { vdouble.inc(-param); }
static void __mul(VDouble& vdouble, double param) { vdouble.mul(param); }
static void __div(VDouble& vdouble, double param) { vdouble.div(param); }
static void __mod(VDouble& vdouble, double param) { vdouble.mod((int)param); }

static void vdouble_op(Request& r, Array *params, 
					   vdouble_op_func_ptr func) {
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	double param=params->size()?
		r.process(
			*static_cast<Value *>(params->get(0)),
			0/*no name*/,
			false/*don't intercept string*/).get_double():1/*used in inc/dec*/;
	(*func)(*vdouble, param);
}

static void _inc(Request& r, const String&, Array *params) { vdouble_op(r, params, &__inc); }
static void _dec(Request& r, const String&, Array *params) { vdouble_op(r, params, &__dec); }
static void _mul(Request& r, const String&, Array *params) { vdouble_op(r, params, &__mul); }
static void _div(Request& r, const String&, Array *params) { vdouble_op(r, params, &__div); }
static void _mod(Request& r, const String&, Array *params) { vdouble_op(r, params, &__mod); }

// initialize

void initialize_double_class(Pool& pool, VStateless_class& vclass) {
	// ^double.int[]
	vclass.add_native_method("int", _int, 0, 0);

	// ^double.double[]
	vclass.add_native_method("double", _double, 0, 0);
	
	// ^double.inc[] 
	// ^double.inc[offset]
	vclass.add_native_method("inc", _inc, 0, 1);
	// ^double.dec[] 
	// ^double.dec[offset]
	vclass.add_native_method("dec", _dec, 0, 1);
	// ^double.mul[k] 
	vclass.add_native_method("mul", _mul, 1, 1);
	// ^double.div[d]
	vclass.add_native_method("div", _div, 1, 1);
	// ^double.mod[offset]
	vclass.add_native_method("mod", _mod, 1, 1);

	// ^double.format[]
	vclass.add_native_method("format", _string_format, 1, 1);
}
