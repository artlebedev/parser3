/** @file
	Parser: @b string parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: string.C,v 1.24 2001/04/03 08:23:06 paf Exp $
*/

#include "pa_request.h"
#include "_string.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "pa_vtable.h"

// global var

VStateless_class *string_class;

// methods

static void _length(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VDouble(pool, r.self->get_string()->size());
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

/// ^string.format{format}
/*not static*/void _string_format(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value& fmt=*static_cast<Value *>(params->get(0));
	// forcing {this param type}
	r.fail_if_junction_(false, fmt, method_name, "fmt must be junction");

	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	char *buf=format(pool, r.self->as_double(), r.process(fmt).as_string().cstr());
	
	r.write_no_lang(String(pool, buf));
}

static void _left(Request& r, const String&, Array *params) {
	Pool& pool=r.pool();

	size_t n=(size_t)r.process(*static_cast<Value *>(params->get(0))).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.piece(0, n)));
}

static void _right(Request& r, const String&, Array *params) {
	Pool& pool=r.pool();

	size_t n=(size_t)r.process(*static_cast<Value *>(params->get(0))).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.piece(string.size()-n, string.size())));
}

static void _mid(Request& r, const String&, Array *params) {
	Pool& pool=r.pool();

	size_t p=(size_t)r.process(*static_cast<Value *>(params->get(0))).as_double();
	size_t n=(size_t)r.process(*static_cast<Value *>(params->get(1))).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.piece(p, p+n)));
}

static void _pos(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value& substr=*static_cast<Value *>(params->get(0));
	// forcing [this param type]
	r.fail_if_junction_(true, substr, method_name, "substr must not be junction");
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VInt(pool, string.pos(substr.as_string())));
}

static void split_list(Request& r, const String& method_name, Array *params,
					   const String& string, 
					   Array& result) {
	Pool& pool=r.pool();

	Value& delim_value=*static_cast<Value *>(params->get(0));
	// forcing [this param type]
	r.fail_if_junction_(true, delim_value, method_name, "delimiter must not be junction");

	string.split(result, 0, delim_value.as_string(), String::UL_CLEAN, -1);
}

static void _lsplit(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	const String& string=*static_cast<VString *>(r.self)->get_string();

	Array& row=*new(pool) Array(pool);
	split_list(r, method_name, params, string, row);

	Table& table=*new(pool) Table(pool, &string, 
		0/*nameless*/, 1/*row preallocate(and only)*/);
	table+=&row;

	r.write_no_lang(*new(pool) VTable(pool, &table));
}

static void _rsplit(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	const String& string=*static_cast<VString *>(r.self)->get_string();

	Array list(pool);
	split_list(r, method_name, params, string, list);

	Array& row=*new(pool) Array(pool);
	for(int i=list.size(); --i>=0; )
		row+=list.get(i);

	Table& table=*new(pool) Table(pool, &string, 
		0/*nameless*/, 1/*row preallocate(and only)*/);
	table+=&row;

	r.write_no_lang(*new(pool) VTable(pool, &table));
}

/// ^string.match{regexp}[options]
static void _match(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	const String& string=*static_cast<VString *>(r.self)->get_string();

	Value& regexp=*static_cast<Value *>(params->get(0));
	// forcing {this param type}
	r.fail_if_junction_(false, regexp, method_name, "regexp must be junction");

	Value& options=*static_cast<Value *>(params->get(1));
	// forcing {this param type}
	r.fail_if_junction_(true, options, method_name, "options must not be junction");

	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	Table& table=string.match(&method_name, 
		r.process(regexp).as_string(), options.as_string());

	r.write_no_lang(*new(pool) VTable(pool, &table));
}

// initialize

void initialize_string_class(Pool& pool, VStateless_class& vclass) {
	// ^string.length[]
	vclass.add_native_method("length", Method::CT_DYNAMIC, _length, 0, 0);
	
	// ^string.int[]
	vclass.add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);
	
	// ^string.double[]
	vclass.add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);

	// ^string.format{format}
	vclass.add_native_method("format", Method::CT_DYNAMIC, _string_format, 1, 1);

	// ^string.left(n)
	vclass.add_native_method("left", Method::CT_DYNAMIC, _left, 1, 1);
	// ^string.right(n)
	vclass.add_native_method("right", Method::CT_DYNAMIC, _right, 1, 1);
	// ^string.mid(p;n)
	vclass.add_native_method("mid", Method::CT_DYNAMIC, _mid, 2, 2);

	// ^string.pos[substr]
	vclass.add_native_method("pos", Method::CT_DYNAMIC, _pos, 1, 1);

	// ^string.lsplit[delim]
	vclass.add_native_method("lsplit", Method::CT_DYNAMIC, _lsplit, 1, 1);
	// ^string.rsplit[delim]
	vclass.add_native_method("rsplit", Method::CT_DYNAMIC, _rsplit, 1, 1);

	// ^string.match{regexp}[options]
	vclass.add_native_method("match", Method::CT_DYNAMIC, _match, 2, 2);
}	

