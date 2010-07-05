/** @file
	Parser: @b VOID parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VOID_C="$Date: 2010/07/05 05:54:46 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vint.h"
#include "pa_vdouble.h"
#include "pa_vvoid.h"
#include "pa_vbool.h"
#include "pa_sql_connection.h"

// externs

extern String sql_bind_name;

// class

class MVoid: public Methoded {
public:
	MVoid();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(void, new MVoid, 0);

// methods

static void _length(Request& r, MethodParams&) {
	// always zero
	r.write_no_lang(*new VInt(0));
}

static void _pos(Request& r, MethodParams& params) {
	// just checking for consistency
	params.as_no_junction(0, "substr must not be code");
	if(params.count()>1){
		ssize_t offset=params.as_int(1, "n must be int", r);
		if(offset<0)
			throw Exception(PARSER_RUNTIME,
				0, 
				"n(%d) must be >=0", offset);
	}
	// never found
	r.write_no_lang(*new VInt(-1));
}

static void _int(Request& r, MethodParams& params) {
	VVoid& vvoid=GET_SELF(r, VVoid);
	r.write_no_lang(*new VInt(
		params.count()==0?vvoid.as_int():params.as_int(0, "default must be int", r)));
}

static void _double(Request& r, MethodParams& params) {
	VVoid& vvoid=GET_SELF(r, VVoid);
	r.write_no_lang(*new VDouble(
		params.count()==0?vvoid.as_double():params.as_double(0, "default must be double", r)));
}

static void _bool(Request& r, MethodParams& params) {
	VVoid& vvoid=GET_SELF(r, VVoid);
	r.write_no_lang(VBool::get(params.count()==0?vvoid.as_bool():params.as_bool(0, "default must be bool", r)));
}

#ifndef DOXYGEN
class Void_sql_event_handlers: public SQL_Driver_query_event_handlers {
	const String& statement_string;
public:
	Void_sql_event_handlers(const String& astatement_string): statement_string(astatement_string) {}
	bool add_column(SQL_Error& /*error*/, const char* /*str*/, size_t /*length*/) { /* ignore */ return false; }
	bool before_rows(SQL_Error& error) {
		// there are some result rows, which is wrong
		error=SQL_Error(PARSER_RUNTIME,
			/*statement_string,*/
			"must return nothing");
		return true;
	}
	bool add_row(SQL_Error& /*error*/) { /* never */ return false; }
	bool add_row_cell(SQL_Error& /*error*/, const char* /*str*/, size_t /*length*/) { /* never */ return false; }

};
#endif

extern int marshal_binds(HashStringValue& hash, SQL_Driver::Placeholder*& placeholders);
extern void unmarshal_bind_updates(HashStringValue& hash, int placeholder_count, SQL_Driver::Placeholder* placeholders);

static void _sql(Request& r, MethodParams& params) {
	Value& statement=params.as_junction(0, "statement must be code");

	HashStringValue* bind=0;
	if(params.count()>1) {
		Value& voptions=params.as_no_junction(1, "options must be hash, not code");
		if(voptions.is_defined() && !voptions.is_string())
			if(HashStringValue* options=voptions.get_hash()) {
				int valid_options=0;
				if(Value* vbind=options->get(sql_bind_name)) {
					valid_options++;
					bind=vbind->get_hash();
				}
				if(valid_options!=options->count())
					throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			} else
				throw Exception(PARSER_RUNTIME, 0, OPTIONS_MUST_BE_HASH);
	}

	SQL_Driver::Placeholder* placeholders=0;
	uint placeholders_count=0;
	if(bind)
		placeholders_count=marshal_binds(*bind, placeholders);

	Temp_lang temp_lang(r, String::L_SQL);
	const String& statement_string=r.process_to_string(statement);
	const char* statement_cstr=statement_string.untaint_cstr(r.flang, r.connection());

	Void_sql_event_handlers handlers(statement_string);
	r.connection()->query(
		statement_cstr, 
		placeholders_count, placeholders,
		0, SQL_NO_LIMIT,
		handlers,
		statement_string);

	if(bind)
		unmarshal_bind_updates(*bind, placeholders_count, placeholders);
}

static void _left_right(Request& r, MethodParams& params) {
	ssize_t sn=params.as_int(0, "n must be int", r);
	if(sn<0)
		throw Exception(PARSER_RUNTIME,
			0, 
			"n(%d) must be >=0", sn);

	// return nothing
}

static void _mid(Request& r, MethodParams& params) {
	ssize_t sbegin=params.as_int(0, "p must be int", r);
	if(sbegin<0)
		throw Exception(PARSER_RUNTIME,
			0, 
			"p(%d) must be >=0", sbegin);

	if(params.count()>1) {
		ssize_t sn=params.as_int(1, "n must be int", r);
		if(sn<0)
			throw Exception(PARSER_RUNTIME,
				0, 
				"n(%d) must be >=0", sn);
	}

	// return nothing
}

// constructor

MVoid::MVoid(): Methoded("void") {
	// ^void.length[] 
	add_native_method("length", Method::CT_DYNAMIC, _length, 0, 0);

	// ^void.pos[substr]
	// ^void.pos[substr](n)
	add_native_method("pos", Method::CT_DYNAMIC, _pos, 1, 2);

	// ^void.int[]
	// ^void.int(default)
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);

	// ^void.double[]
	// ^void.double(default)
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);

	// ^void.bool[]
	// ^void.bool(default)
	add_native_method("bool", Method::CT_DYNAMIC, _bool, 0, 1);

	// ^void:sql{query}
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);

	// ^void.left() ^void.right()
	add_native_method("left", Method::CT_DYNAMIC, _left_right, 1, 1);
	add_native_method("right", Method::CT_DYNAMIC, _left_right, 1, 1);

	// ^void.mid(p;n)
	add_native_method("mid", Method::CT_DYNAMIC, _mid, 1, 2);
}
