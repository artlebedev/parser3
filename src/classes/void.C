/** @file
	Parser: @b VOID parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: void.C,v 1.6 2001/08/09 06:44:04 parser Exp $"; 

#include "classes.h"
#include "pa_request.h"
#include "pa_vint.h"
#include "pa_vdouble.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"

// defines

#define VOID_CLASS_NAME "void"

// class

class MVoid : public Methoded {
public:
	MVoid(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _int(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VVoid *vvoid=static_cast<VVoid *>(r.self);
	Value& value=*new(pool) VVoid(pool, vvoid->as_int());
	r.write_no_lang(value);
}

static void _double(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	VVoid *vvoid=static_cast<VVoid *>(r.self);
	Value& value=*new(pool) VDouble(pool, vvoid->as_double());
	r.write_no_lang(value);
}

#ifndef DOXYGEN
class Void_sql_event_handlers : public SQL_Driver_query_event_handlers {
public:
	Void_sql_event_handlers(Pool& apool, const String& astatement_string) :
		pool(apool), statement_string(astatement_string) {
	}
	void add_column(void *ptr, size_t size) { /* ignore */ }
	void before_rows() {
		// there are some result rows, which is wrong
		PTHROW(0, 0,
			&statement_string,
			"must return nothing");
	}
	void add_row() { /* never */ }
	void add_row_cell(void *ptr, size_t size) { /* never */ }

private:
	Pool& pool;
	const String& statement_string;
};
#endif
static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	if(!r.connection)
		PTHROW(0, 0,
			&method_name,
			"without connect");

	Value& statement=params->as_junction(0, "statement must be code");

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process(statement).as_string();
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection);
	Void_sql_event_handlers handlers(pool, statement_string);
	bool need_rethrow=false; Exception rethrow_me;
	PTRY {
		r.connection->query(
			statement_cstr, 0, 0,
			handlers);
	}
	PCATCH(e) {
		rethrow_me=e;  need_rethrow=true;
	}
	PEND_CATCH
	if(need_rethrow)
		PTHROW(rethrow_me.type(), rethrow_me.code(),
			&statement_string, // setting more specific source [were url]
			rethrow_me.comment());
}

// constructor

MVoid::MVoid(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), VOID_CLASS_NAME));


	// ^VOID.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);

	// ^VOID.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);

	// ^sql[query]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 1);
}

// global variable

Methoded *void_class;

// creator

Methoded *MVoid_create(Pool& pool) {
	return void_class=new(pool) MVoid(pool);
}
