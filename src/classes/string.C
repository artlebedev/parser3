/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: string.C,v 1.6 2001/03/11 12:04:43 paf Exp $
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
	vclass.add_native_method("length", _length, 0, 0);
	
	// ^string.int[]
	vclass.add_native_method("int", _int, 0, 0);
	
	// ^string.double[]
	vclass.add_native_method("double", _double, 0, 0);
}	

