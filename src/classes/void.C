/** @file
	Parser: @b VOID parser class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VOID_C="$Date: 2004/02/11 15:33:13 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vint.h"
#include "pa_vdouble.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"

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

#ifndef DOXYGEN
class Void_sql_event_handlers: public SQL_Driver_query_event_handlers {
	const String& statement_string;
public:
	Void_sql_event_handlers(const String& astatement_string): statement_string(astatement_string) {}
	bool add_column(SQL_Error& /*error*/, const char* /*str*/, size_t /*length*/) { /* ignore */ return false; }
	bool before_rows(SQL_Error& error) {
		// there are some result rows, which is wrong
		error=SQL_Error("parser.runtime",
			/*statement_string,*/
			"must return nothing");
		return true;
	}
	bool add_row(SQL_Error& /*error*/) { /* never */ return false; }
	bool add_row_cell(SQL_Error& /*error*/, const char* /*str*/, size_t /*length*/) { /* never */ return false; }

};
#endif
static void _sql(Request& r, MethodParams& params) {
	Value& statement=params.as_junction(0, "statement must be code");

	Temp_lang temp_lang(r, String::L_SQL);
	const String& statement_string=r.process_to_string(statement);
	const char* statement_cstr=
		statement_string.cstr(String::L_UNSPECIFIED, r.connection());
	Void_sql_event_handlers handlers(statement_string);
	r.connection()->query(
		statement_cstr, 0, 0,
		handlers,
		statement_string);
}

// constructor

MVoid::MVoid(): Methoded("void") {
	// ^void.length[] 
	add_native_method("length", Method::CT_DYNAMIC, _length, 0, 0);

	// ^void.pos[substr]
	add_native_method("pos", Method::CT_DYNAMIC, _pos, 1, 1);

	// ^void.int[] 
	// ^void.int(default)
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);

	// ^void.double[] 
	// ^void.double(default)
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);

	// ^sql[query]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 1);
}
