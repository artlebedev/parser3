/** @file
	Parser: @b nothing parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: nothing.C,v 1.4 2001/05/21 17:19:49 parser Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vint.h"
#include "pa_vdouble.h"
#include "pa_vnothing.h"
#include "pa_sql_connection.h"

// defines

#define NOTHING_CLASS_NAME "nothing"

// class

class MNothing : public Methoded {
public:
	MNothing(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _int(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vunknown=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VInt(pool, vunknown->as_int());
	r.write_no_lang(value);
}

static void _double(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VInt *vunknown=static_cast<VInt *>(r.self);
	Value& value=*new(pool) VDouble(pool, vunknown->as_double());
	r.write_no_lang(value);
}

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	if(!r.connection)
		PTHROW(0, 0,
			&method_name,
			"without connect");

	Value& statement=params->get_junction(0, "statement must be code");

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process(statement).as_string();
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection);
	unsigned int sql_column_count; SQL_Driver::Cell *sql_columns;
	unsigned long sql_row_count; SQL_Driver::Cell **sql_rows;
	bool need_rethrow=false; Exception rethrow_me;
	PTRY {
		r.connection->query(
			statement_cstr, 0, 0,
			&sql_column_count, &sql_columns,
			&sql_row_count, &sql_rows);
	}
	PCATCH(e) { // connect/process problem
		rethrow_me=e;  need_rethrow=true;
	}
	PEND_CATCH
	if(need_rethrow)
		PTHROW(rethrow_me.type(), rethrow_me.code(),
			&statement_string, // setting more specific source [were url]
			rethrow_me.comment());

	// there are some result rows, which is wrong
	if(sql_row_count)
		PTHROW(0, 0,
			&statement_string,
			"must not return result");
}

// constructor

MNothing::MNothing(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), NOTHING_CLASS_NAME));


	// ^nothing.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);

	// ^nothing.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);

	// ^sql[query]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 1);
}

// global variable

Methoded *nothing_class;

// creator

Methoded *MNothing_create(Pool& pool) {
	return nothing_class=new(pool) MNothing(pool);
}
