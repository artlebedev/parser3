/** @file
	Parser: @b string parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: string.C,v 1.38 2001/04/15 13:12:18 paf Exp $
*/

#include "pa_request.h"
#include "_string.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_string.h"

// global var

VStateless_class *string_class;

// methods

static void _length(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VDouble(pool, r.self->get_string()->size());
	r.write_no_lang(value);
}

static void _int(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, (int)r.self->as_double());
	r.write_no_lang(value);
}

static void _double(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VDouble(pool, r.self->as_double());
	r.write_no_lang(value);
}

/// ^string.format{format}
/*not static*/void _string_format(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& fmt=params->get_junction(0, "fmt must be code");

	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	char *buf=format(pool, r.self->as_double(), r.process(fmt).as_string().cstr());
	
	r.write_no_lang(String(pool, buf));
}

static void _left(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	size_t n=(size_t)r.process(params->get(0)).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.mid(0, n)));
}

static void _right(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	size_t n=(size_t)r.process(params->get(0)).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.mid(string.size()-n, string.size())));
}

static void _mid(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	size_t p=(size_t)r.process(params->get(0)).as_double();
	size_t n=(size_t)r.process(params->get(1)).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.mid(p, p+n)));
}

static void _pos(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& substr=params->get_no_junction(0, "substr must not be code");
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VInt(pool, string.pos(substr.as_string())));
}

static void split_list(Request& r, const String& method_name, MethodParams *params,
					   const String& string, 
					   Array& result) {
	Pool& pool=r.pool();

	Value& delim_value=params->get_no_junction(0, "delimiter must not be code");

	string.split(result, 0, delim_value.as_string(), String::UL_CLEAN, -1);
}

static void _lsplit(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& string=*static_cast<VString *>(r.self)->get_string();

	Array& row=*new(pool) Array(pool);
	split_list(r, method_name, params, string, row);

	Table& table=*new(pool) Table(pool, &string, 
		0/*nameless*/, 1/*row preallocate(and only)*/);
	table+=&row;

	r.write_no_lang(*new(pool) VTable(pool, &table));
}

static void _rsplit(Request& r, const String& method_name, MethodParams *params) {
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

static void search_action(Table& table, Array *row, int, int, void *) {
	if(row)
		table+=row;
}

struct Replace_action_info {
	Request *request;  const String *origin;
	const String *src;  String *dest;
	Value *replacement_code;
	const String *post_match;
};
static void replace_action(Table& table, Array *row, int start, int finish, 
							   void *info) {
	Replace_action_info& ai=*static_cast<Replace_action_info *>(info);
	if(row) { // begin&middle
		// piece from last match['start'] to beginning of this match['finish']
		if(start!=finish)
			*ai.dest << ai.src->mid(start, finish);//ai.dest->APPEND_CONST("-");
		// store found parts in one-record Vtable
		if(table.size()) // middle
			table.put(0, row);
		else // begin
			table+=row;
		{ // execute 'replacement_code' in 'table' context
			VTable& vtable=*new(table.pool()) VTable(table.pool(), &table);
			vtable.set_name(*ai.origin);

			Junction *junction=ai.replacement_code->get_junction();
			junction->rcontext=junction->root=&vtable;
			Value& replaced=ai.request->process(*ai.replacement_code, ai.origin, false);

			/*
			ai.dest->APPEND_CONST("(");
				*ai.dest << *(String *)row->get(1/*match* /);
			ai.dest->APPEND_CONST(")");
			*/
			*ai.dest << replaced.as_string();
		}
		ai.post_match=(String *)row->get(2/*post_match*/);
	} else // end
		*ai.dest << *ai.post_match;
}

/** search/replace
	^string.match[regexp][options]
	^string.match[regexp][options]{replacement-code}
*/
static void _match(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& src=*static_cast<VString *>(r.self)->get_string();

	Value& regexp=params->get_no_junction(0, "regexp must not be code");

	const String *options=
		params->size()>1?
		&params->get_no_junction(1, "options must not be code").as_string():0;

	Value *result;
	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	Table *table;
	if(params->size()<3) { // search
		if(src.match(&method_name, 
			regexp.as_string(), options,
			&table,
			search_action, 0)) {
			// matched
			if(table->columns()->size()==3 && // just matched[3=pre/match/post], no substrings
				table->size()==1)  // just one row, not /g_lobal search
				result=new(pool) VBool(pool, true);
			else // table of pre/match/post+substrings
				result=new(pool) VTable(pool, table);
		} else // not matched [not global]
			result=new(pool) VBool(pool, false);
	} else { // replace
		Value& replacement_code=params->get_junction(2, "replacement code must be code");

		String& dest=*new(pool) String(pool);
		Replace_action_info replace_action_info={
			&r, &method_name,
			&src, &dest,
			&replacement_code,
			&src
		};
		src.match(&method_name, 
			r.process(regexp).as_string(), options,
			&table,
			replace_action, &replace_action_info);
		result=new(pool) VString(dest);
	}
	result->set_name(method_name);
	r.write_assign_lang(*result);
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

	// ^string.match[regexp][options]
	// ^string.match[regexp][options]{replacement-code}
	vclass.add_native_method("match", Method::CT_DYNAMIC, _match, 1, 3);
}	

