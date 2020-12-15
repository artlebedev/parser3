/** @file
	Parser: @b VOID parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"

volatile const char * IDENT_VOID_C="$Id: void.C,v 1.58 2020/12/15 17:10:29 moko Exp $";

// class

class MVoid: public Methoded {
public:
	MVoid();
};

// void is inherited from string, thus global variable declared in string.C

// methods

#ifndef DOXYGEN
class Void_sql_event_handlers: public SQL_Driver_query_event_handlers {
public:
	bool add_column(SQL_Error& /*error*/, const char* /*str*/, size_t /*length*/) { /* ignore */ return false; }
	bool before_rows(SQL_Error& error) {
		// there are some result rows, which is wrong
		error=SQL_Error("must return nothing");
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
	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1, "sql options")) {
			int valid_options=0;
			if(Value* vbind=options->get(sql_bind_name)) {
				valid_options++;
				bind=vbind->get_hash();
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	SQL_Driver::Placeholder* placeholders=0;
	uint placeholders_count=0;
	if(bind)
		placeholders_count=marshal_binds(*bind, placeholders);

	const String& statement_string=r.process_to_string(statement);
	const char* statement_cstr=statement_string.untaint_cstr(String::L_SQL, r.connection());

	Void_sql_event_handlers handlers;
	r.connection()->query(statement_cstr, placeholders_count, placeholders, 0, SQL_NO_LIMIT, handlers, statement_string);

	if(bind)
		unmarshal_bind_updates(*bind, placeholders_count, placeholders);
}

// constructor

MVoid::MVoid(): Methoded("void") {
	set_base(string_class);

	// ^void:sql{query}
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);

	// all other methods are inherinted from empty string
}
