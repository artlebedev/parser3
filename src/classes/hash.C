/** @file
	Parser: @b hash parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: hash.C,v 1.11 2001/07/23 11:19:25 parser Exp $
*/
static const char *RCSId="$Id: hash.C,v 1.11 2001/07/23 11:19:25 parser Exp $"; 

#include "classes.h"
#include "pa_request.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"
#include "pa_vtable.h"

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
		r.write_assign_lang(default_value?*default_value:*new(pool) VVoid(pool));
	}
}

#ifndef DOXYGEN
class Hash_sql_event_handlers : public SQL_Driver_query_event_handlers {
public:
	Hash_sql_event_handlers(Pool& apool, const String& amethod_name,
		const String& astatement_string, const char *astatement_cstr,
		Hash& arows_hash) :
		pool(apool), 
		method_name(amethod_name),
		statement_string(astatement_string),
		statement_cstr(astatement_cstr),
		rows_hash(arows_hash),
		columns(pool),
		row_index(0) {
	}
	void add_column(void *ptr, size_t size) {
		String *column=new(pool) String(pool);
		column->APPEND_TAINTED(
			(const char *)ptr, size, 
			statement_cstr, 0);
		columns+=column;
	}
	void before_rows() { 
		if(columns.size()<=1)
			PTHROW(0, 0,
				&method_name,
				"column count must be more than 1 to create a hash");
	}
	void add_row() {
		column_index=0;
	}
	void add_row_cell(void *ptr, size_t size) {
		String *cell=new(pool) String(pool);
		if(size)
			cell->APPEND_TAINTED(
				(const char *)ptr, size, 
				statement_cstr, row_index++);
		if(column_index==0) {
			VHash *row_vhash=new(pool) VHash(pool);
			row_hash=row_vhash->get_hash();
			rows_hash.put(*cell, row_vhash);
		} else
			row_hash->put(*columns.get_string(column_index), new(pool) VString(*cell));
	}

private:
	Pool& pool;
	const String& method_name;
	const String& statement_string; const char *statement_cstr;
	Hash& rows_hash;
	Hash *row_hash;
	int column_index;
	Array columns;
	int row_index;
};
#endif

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	if(!r.connection)
		PTHROW(0, 0,
			&method_name,
			"without connect");

	Value& statement=params->as_junction(0, "statement must be code");

	ulong limit=0;
	if(params->size()>1) {
		Value& limit_code=params->as_junction(1, "limit must be expression");
		limit=(uint)r.process(limit_code).as_double();
	}

	ulong offset=0;
	if(params->size()>2) {
		Value& offset_code=params->as_junction(2, "offset must be expression");
		offset=(ulong)r.process(offset_code).as_double();
	}

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process(statement).as_string();
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection);
	Hash& hash=static_cast<VHash *>(r.self)->hash();
	hash.clear();	
	Hash_sql_event_handlers handlers(pool, method_name,
		statement_string, statement_cstr, hash);
	bool need_rethrow=false; Exception rethrow_me;
	PTRY {
		r.connection->query(
			statement_cstr, offset, limit,
			handlers);
	}
	PCATCH(e) { // query problem
		rethrow_me=e;  need_rethrow=true;
	}
	PEND_CATCH
	if(need_rethrow)
		PTHROW(rethrow_me.type(), rethrow_me.code(),
			&statement_string, // setting more specific source [were url]
			rethrow_me.comment());
}

static void keys_collector(const Hash::Key& key, Hash::Val *value, void *info) {
	Table& table=*static_cast<Table *>(info);
	Pool& pool=table.pool();

	Array& row=*new(pool) Array(pool);
	row+=&key;
	table+=&row;
}
static void _keys(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();

	Array& columns=*new(pool) Array(pool);
	columns+=new(pool) String(pool, "key");
	Table& table=*new(pool) Table(pool, &method_name, &columns);

	static_cast<VHash *>(r.self)->hash().for_each(keys_collector, &table);

	VTable& result=*new(pool) VTable(pool, &table);
	result.set_name(method_name);
	r.write_no_lang(result);
}

// constructor

MHash::MHash(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), HASH_CLASS_NAME));

	// ^hash.default[]
	// ^hash.default[hash]
	add_native_method("default", Method::CT_DYNAMIC, _default, 0, 1);

	// ^hash:sql[query][(count[;offset])]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 3);

	// ^hash.keys[]
	add_native_method("keys", Method::CT_DYNAMIC, _keys, 0, 0);	
}

// global variable

Methoded *hash_base_class;

// creator

Methoded *MHash_create(Pool& pool) {
	return hash_base_class=new(pool) MHash(pool);
}
