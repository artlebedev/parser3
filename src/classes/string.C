/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: string.C,v 1.19 2001/03/29 17:32:57 paf Exp $
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

/*not static*/void _string_format(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value& fmt=*static_cast<Value *>(params->get(0));
	// forcing [this param type]
	r.fail_if_junction_(true, fmt, method_name, "fmt must not be junction");

	char *buf=format(pool, r.self->as_double(), fmt.as_string().cstr());
	
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
					   const String& string, Array& list) {
	Pool& pool=r.pool();

	Value& delim_value=*static_cast<Value *>(params->get(0));
	// forcing [this param type]
	r.fail_if_junction_(true, delim_value, method_name, "delimiter must not be junction");
	const String& delim=delim_value.as_string();
	
	if(delim.size()) {
		size_t pos_after=0;
		int pos_before;
		while((pos_before=string.pos(delim, pos_after))>=0) { // we have 'delim' in 'string'?
			list+=&string.piece(pos_after, pos_before);
			pos_after=pos_before+delim.size();
		}
		// last piece
		if(pos_after<string.size()) 
			list+=&string.piece(pos_after, string.size());
	} else { // empty delim
		list+=&string;
	}
}

static void _lsplit(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	const String& string=*static_cast<VString *>(r.self)->get_string();

	Array list(pool);
	split_list(r, method_name, params, string, list);
	Table& table=*new(pool) Table(pool, &string, 0);

	int size=list.size();
	for(int i=0; i<size; i++) {
		Array& row=*new(pool) Array(pool, 1);
		table+=&(row+=list.quick_get(i));
	}

	r.write_no_lang(*new(pool) VTable(pool, &table));
}

static void _rsplit(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	const String& string=*static_cast<VString *>(r.self)->get_string();

	Array list(pool);
	split_list(r, method_name, params, string, list);
	Table& table=*new(pool) Table(pool, &string, 0);

	for(int i=list.size(); --i>=0; ) {
		Array& row=*new(pool) Array(pool, 1);
		table+=&(row+=list.get(i));
	}

	r.write_no_lang(*new(pool) VTable(pool, &table));
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

	// ^string.left(n)
	vclass.add_native_method("left", _left, 1, 1);
	// ^string.right(n)
	vclass.add_native_method("right", _right, 1, 1);
	// ^string.mid(p;n)
	vclass.add_native_method("mid", _mid, 2, 2);

	// ^string.pos[substr]
	vclass.add_native_method("pos", _pos, 1, 1);

	// ^string.lsplit[delim]
	vclass.add_native_method("lsplit", _lsplit, 1, 1);
	// ^string.rsplit[delim]
	vclass.add_native_method("rsplit", _rsplit, 1, 1);
}	

