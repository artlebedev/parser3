/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: string.C,v 1.14 2001/03/29 09:31:42 paf Exp $
*/

#include "pa_request.h"
#include "_string.h"
#include "pa_vdouble.h"
#include "pa_vint.h"

// global var

VStateless_class *string_class;

// methods

static void _length(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VDouble(pool, r.self->as_string().size());
	r.write_no_lang(value);
}

static void _int(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, (int)r.self->as_double());
	r.write_no_lang(value);
}

static void _double(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VDouble(pool, r.self->as_double());
	r.write_no_lang(value);
}

void _string_format(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value& fmt=*static_cast<Value *>(params->get(0));
	// forcing ^format[this param type]
	r.fail_if_junction_(true, fmt, 
		method_name, "fmt must not be junction");

	char *buf=format(pool, r.self->as_double(), fmt.as_string().cstr());
	
	r.write_no_lang(String(pool, buf));
}

// initialize

void initialize_string_class(Pool& pool, VStateless_class& vclass) {
	// ^string.length[]
	vclass.add_native_method("length", _length, 0, 0);
	
	// ^string.int[]
	vclass.add_native_method("int", _int, 0, 0);
	
	// ^string.double[]
	vclass.add_native_method("double", _double, 0, 0);

	// ^string.format[]
	vclass.add_native_method("format", _string_format, 1, 1);

	// ^string.toupper[]
	//vclass.add_native_method("toupper", _toupper, 0, 0);
}	

