/** @file
	Parser: @b VOID parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VOID_C="$Date: 2002/08/01 11:41:13 $";

#include "classes.h"
#include "pa_request.h"
#include "pa_vint.h"
#include "pa_vdouble.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"

// class

class MVoid : public Methoded {
public:
	MVoid(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _int(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();
	VVoid *vvoid=static_cast<VVoid *>(r.self);
	r.write_no_lang(*new(pool) VInt(pool, 
		params->size()==0?vvoid->as_int():params->as_int(0, "default must be int", r)));
}

static void _double(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();
	VVoid *vvoid=static_cast<VVoid *>(r.self);
	r.write_no_lang(*new(pool) VDouble(pool, 
		params->size()==0?vvoid->as_double():params->as_double(0, "default must be double", r)));
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
		throw Exception("parser.runtime",
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

	Value& statement=params->as_junction(0, "statement must be code");

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process_to_string(statement);
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection(&method_name));
	Void_sql_event_handlers handlers(pool, statement_string);
	try {
		r.connection(&method_name)->query(
			statement_cstr, 0, 0,
			handlers);
	} catch(const Exception& e) {
		// more specific source [were url]
		throw Exception("sql.execute", 
			&statement_string, 
			"%s", e.comment());
	}
}

// constructor

MVoid::MVoid(Pool& apool) : Methoded(apool, "void") {
	// ^void.int[] 
	// ^void.int(default)
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);

	// ^void.double[] 
	// ^void.double(default)
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);

	// ^sql[query]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 1);
}

// global variable

Methoded *void_class;

// creator

Methoded *MVoid_create(Pool& pool) {
	return void_class=new(pool) MVoid(pool);
}
