/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: string.C,v 1.5 2001/03/11 08:16:31 paf Exp $
*/

#include "pa_request.h"
#include "_string.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// global var

VClass *string_class;

// methods

static void _length(Request& r, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VDouble(pool, r.self->as_string().size());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _int(Request& r, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, static_cast<int>(r.self->get_double()));
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _double(Request& r, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VDouble(pool, r.self->get_double());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

void initialize_string_class(Pool& pool, VClass& vclass) {
	// ^string.length[]
	String& LENGTH_NAME=*new(pool) String(pool);
	LENGTH_NAME.APPEND_CONST("length");

	Method& LENGTH_METHOD=*new(pool) Method(pool,
		LENGTH_NAME,
		0, 0, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _length
	);
	vclass.add_method(LENGTH_NAME, LENGTH_METHOD);

	// ^string.int[]
	String& INT_NAME=*new(pool) String(pool);
	INT_NAME.APPEND_CONST("int");

	Method& INT_METHOD=*new(pool) Method(pool,
		INT_NAME,
		0, 0, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _int
	);
	vclass.add_method(INT_NAME, INT_METHOD);

	// ^string.double[]
	String& DOUBLE_NAME=*new(pool) String(pool);
	DOUBLE_NAME.APPEND_CONST("double");

	Method& DOUBLE_METHOD=*new(pool) Method(pool,
		DOUBLE_NAME,
		0, 0, // min,max numbered_params_count
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, _double
	);
	vclass.add_method(DOUBLE_NAME, DOUBLE_METHOD);
}	

