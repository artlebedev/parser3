/** @file
	Parser: @b hash parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: hash.C,v 1.2 2001/05/21 16:01:10 parser Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vhash.h"
#include "pa_vunknown.h"
#include "pa_sql_connection.h"

// defines

#define HASH_CLASS_NAME "hash"

// class

class MHash : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VHash(pool); }

public:
	MHash(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _default(Request& r, const String&, MethodParams *params) { 
	Pool& pool=r.pool();

	VHash& vhash=*static_cast<VHash *>(r.self);
	if(params->size())
		vhash.set_default(params->get(0)); // info: may be code..
	else {
		Value *default_value=vhash.get_default();
		r.write_assign_lang(default_value?*default_value:*new(pool) VUnknown(pool));
	}
}

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	if(!r.connection)
		PTHROW(0, 0,
			&method_name,
			"without connect");

	Value& statement=params->get_junction(0, "statement must be code");

	ulong limit=0;
	if(params->size()>1) {
		Value& limit_code=params->get_junction(1, "limit must be expression");
		limit=(uint)r.process(limit_code).as_double();
	}

	ulong offset=0;
	if(params->size()>2) {
		Value& offset_code=params->get_junction(2, "offset must be expression");
		offset=(ulong)r.process(offset_code).as_double();
	}

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process(statement).as_string();
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection);
	unsigned int sql_column_count; SQL_Driver::Cell *sql_columns;
	unsigned long sql_row_count; SQL_Driver::Cell **sql_rows;
	bool need_rethrow=false; Exception rethrow_me;
	PTRY {
		r.connection->query(
			statement_cstr, offset, limit,
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

	Hash& rows_hash=static_cast<VHash *>(r.self)->hash();
	rows_hash.clear();	

	if(sql_column_count<=1)
		return;

	Array& columns=*new(pool) Array(pool);
	for(unsigned int i=0+1; i<sql_column_count; i++) {
		String& column=*new(pool) String(pool);
		column.APPEND_TAINTED(
			(const char *)sql_columns[i].ptr, sql_columns[i].size,
			statement_cstr, 0);
		columns+=&column;
	}
	
	for(unsigned long row=0; row<sql_row_count; row++) {
		SQL_Driver::Cell *sql_cells=sql_rows[row];			
		
		VHash& row_vhash=*new(pool) VHash(pool);
		Hash& row_hash=*row_vhash.get_hash();
		String *key=0; // calm, compiler
		String *cell;
		for(unsigned int i=0; i<sql_column_count; i++) {
			cell=new(pool) String(pool);
			cell->APPEND_TAINTED(
				(const char *)sql_cells[i].ptr, sql_cells[i].size,
				statement_cstr, row);
			if(i==0)
				key=cell;
			else
				row_hash.put(*columns.get_string(i-1), new(pool) VString(*cell));
		}
		rows_hash.put(*key, &row_vhash);
	}
}

// constructor

MHash::MHash(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), HASH_CLASS_NAME));

	// ^hash.default[]
	// ^hash.default[hash]
	add_native_method("default", Method::CT_DYNAMIC, _default, 0, 1);

	// ^hash:sql[query][(count[;offset])]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 3);

}

// global variable

Methoded *hash_base_class;

// creator

Methoded *MHash_create(Pool& pool) {
	return hash_base_class=new(pool) MHash(pool);
}
