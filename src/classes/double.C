/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: double.C,v 1.5 2001/03/11 08:16:31 paf Exp $
*/

#include "pa_request.h"
#include "_double.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// global var

VClass *double_class;

// methods

static void _int(Request& r, Array *) {
	Pool& pool=r.pool();
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	Value& value=*new(pool) VInt(pool, static_cast<int>(vdouble->get_double()));
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _double(Request& r, Array *) {
	Pool& pool=r.pool();
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	Value& value=*new(pool) VDouble(pool, vdouble->get_double());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _inc(Request& r, Array *params) {
	VDouble *vdouble=static_cast<VDouble *>(r.self);
	double increment=params->size()?static_cast<Value *>(params->get(0))->get_double():1;
	vdouble->inc(increment);
}

void initialize_double_class(Pool& pool, VClass& vclass) {
	// ^double.int[]
	String& INT_NAME=*new(pool) String(pool);
	INT_NAME.APPEND_CONST("int");

	Method& INT_METHOD=*new(pool) Method(pool,
		INT_NAME,
		0, 0, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _int
	);
	vclass.add_method(INT_NAME, INT_METHOD);

	// ^double.double[]
	String& DOUBLE_NAME=*new(pool) String(pool);
	DOUBLE_NAME.APPEND_CONST("double");

	Method& DOUBLE_METHOD=*new(pool) Method(pool,
		DOUBLE_NAME,
		0, 0, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _double
	);
	vclass.add_method(DOUBLE_NAME, DOUBLE_METHOD);

	// ^double.inc[] ^double.inc[offset]
	String& INC_NAME=*new(pool) String(pool);
	INC_NAME.APPEND_CONST("inc");

	Method& INC_METHOD=*new(pool) Method(pool,
		INC_NAME,
		0, 1, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _inc
	);
	vclass.add_method(INC_NAME, INC_METHOD);
}

