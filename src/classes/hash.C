/** @file
	Parser: @b hash parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: hash.C,v 1.40 2002/04/18 15:33:32 paf Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_vmethod_frame.h"

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
			throw Exception("parser.runtime",
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
			row_hash=row_vhash->get_hash(0);
			rows_hash.put(*cell, row_vhash);
		} else
			row_hash->put(*columns.get_string(column_index), new(pool) VString(*cell));
		column_index++;
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

static void copy_all_overwrite_to(const Hash::Key& key, Hash::Val *value, void *info) {
	Hash& dest=*static_cast<Hash *>(info);
	dest.put(key, value);
}
static void _create_or_add(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	
	if(params->size()) {
		Value& vb=params->as_no_junction(0, "param must be hash");
		if(Hash *b=vb.get_hash(&method_name))
			b->for_each(copy_all_overwrite_to, &static_cast<VHash *>(r.self)->hash(&method_name));
	}
}

static void remove_key_from(const Hash::Key& key, Hash::Val *value, void *info) {
	Hash& dest=*static_cast<Hash *>(info);
	dest.remove(key);
}
static void _sub(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	
	Value& vb=params->as_no_junction(0, "param must be hash");
	if(Hash *b=vb.get_hash(&method_name))
		b->for_each(remove_key_from, &static_cast<VHash *>(r.self)->hash(&method_name));
}

static void copy_all_dontoverwrite_to(const Hash::Key& key, Hash::Val *value, void *info) {
	Hash& dest=*static_cast<Hash *>(info);
	dest.put_dont_replace(key, value);
}
static void _union(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	// dest = copy of self
	Hash& dest=*new(pool) Hash(static_cast<VHash *>(r.self)->hash(&method_name));
	// dest += b
	Value& vb=params->as_no_junction(0, "param must be hash");
	if(Hash *b=vb.get_hash(&method_name))
		b->for_each(copy_all_dontoverwrite_to, &dest);

	// return result
	Value& result=*new(pool) VHash(pool, dest);
	r.write_no_lang(result);
}

#ifndef DOXYGEN
struct Copy_intersection_to_info {
	Hash *b;
	Hash *dest;
};
#endif

static void copy_intersection_to(const Hash::Key& key, Hash::Val *value, void *info) {
	Copy_intersection_to_info& i=*static_cast<Copy_intersection_to_info *>(info);

	if(i.b->get(key))
		i.dest->put_dont_replace(key, value);
}
static void _intersection(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	// dest = copy of self
	Hash& dest=*new(pool) Hash(pool);
	// dest += b
	Value& vb=params->as_no_junction(0, "param must be hash");
	if(Hash *b=vb.get_hash(&method_name)) {
		Copy_intersection_to_info info={
			b,
			&dest
		};
		static_cast<VHash *>(r.self)->hash(&method_name).for_each(copy_intersection_to, &info);
	}

	// return result
	r.write_no_lang(*new(pool) VHash(pool, dest));
}

static void *intersects(const Hash::Key& key, Hash::Val *value, void *info) {
	return static_cast<Hash *>(info)->get(key);
}

static void _intersects(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	bool yes=false;

	// dest = copy of self
	Hash& dest=*new(pool) Hash(pool);
	// dest += b
	Value& vb=params->as_no_junction(0, "param must be hash");
	if(Hash *b=vb.get_hash(&method_name))
		yes=static_cast<VHash *>(r.self)->hash(&method_name).first_that(intersects, b)!=0;

	// return result
	r.write_no_lang(*new(pool) VBool(pool, yes));
}


static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& statement=params->as_junction(0, "statement must be code");

	ulong limit=0;
	ulong offset=0;
	if(params->size()>1) {
		Value& voptions=params->as_no_junction(1, "options must be hash, not code");
		if(voptions.is_defined())
			if(Hash *options=voptions.get_hash(&method_name)) {
				if(Value *vlimit=(Value *)options->get(*sql_limit_name))
					limit=(ulong)r.process_to_value(*vlimit).as_double();
				if(Value *voffset=(Value *)options->get(*sql_offset_name))
					offset=(ulong)r.process_to_value(*voffset).as_double();
			} else
				throw Exception("parser.runtime",
					&method_name,
					"options must be hash");
	}

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process_to_string(statement);
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection(&method_name));
	Hash& hash=static_cast<VHash *>(r.self)->hash(&method_name);
	hash.clear();	
	Hash_sql_event_handlers handlers(pool, method_name,
		statement_string, statement_cstr, hash);

	r.connection(&method_name)->query(
		statement_cstr, offset, limit,
		handlers);
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

	static_cast<VHash *>(r.self)->hash(&method_name).for_each(keys_collector, &table);

	r.write_no_lang(*new(pool) VTable(pool, &table));
}

static void _count(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();

	r.write_no_lang(
		*new(pool) VInt(pool, static_cast<VHash *>(r.self)->hash(&method_name).size()));
}

static void _delete(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	static_cast<VHash *>(r.self)->hash(&method_name).remove(params->as_string(0, "key must be string"));
}

#ifndef DOXYGEN
struct Foreach_info {
	Request *r;
	const String* key_var_name;
	const String* value_var_name;
	Value *body_code;
	Value *delim_maybe_code;

	VString *vkey;
	bool need_delim;
};
#endif

static void one_foreach_cycle(const Hash::Key& akey, Hash::Val *avalue, 
										  void *info) {
	Foreach_info& i=*static_cast<Foreach_info *>(info);

	i.vkey->set_string(akey);
	i.r->root->put_element(*i.key_var_name, i.vkey);
	i.r->root->put_element(*i.value_var_name, static_cast<Value *>(avalue));

	StringOrValue processed_body=i.r->process(*i.body_code);
	if(i.delim_maybe_code) { // delimiter set?
		const String *string=processed_body.get_string();
		if(i.need_delim && string && string->size()) // need delim & iteration produced string?
			i.r->write_pass_lang(i.r->process(*i.delim_maybe_code));
		i.need_delim=true;
	}
	i.r->write_pass_lang(processed_body);
}
static void _foreach(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& key_var_name=params->as_string(0, "key-var name must be string");
	const String& value_var_name=params->as_string(1, "value-var name must be string");
	Value& body_code=params->as_junction(2, "body must be code");
	Value *delim_maybe_code=params->size()>3?&params->get(3):0;

	Foreach_info info={
		&r,
		&key_var_name, &value_var_name,
		&body_code,
		delim_maybe_code,

		new(pool) VString(pool),
		false
	};
	VHash& self=*static_cast<VHash *>(r.self);
	Hash& hash=self.hash(&method_name);
	VHash_lock lock(self);
	hash.for_each(one_foreach_cycle, &info);
}

// constructor

MHash::MHash(Pool& apool) : Methoded(apool, "hash") 
{
	// ^hash::create[[copy_from]]
	add_native_method("create", Method::CT_DYNAMIC, _create_or_add, 0, 1);
	// ^hash.add[add_from]
	add_native_method("add", Method::CT_DYNAMIC, _create_or_add, 1, 1);
	// ^hash.sub[sub_from]
	add_native_method("sub", Method::CT_DYNAMIC, _sub, 1, 1);
	// ^a.union[b] = hash
	add_native_method("union", Method::CT_DYNAMIC, _union, 1, 1);
	// ^a.intersection[b] = hash
	add_native_method("intersection", Method::CT_DYNAMIC, _intersection, 1, 1);
	// ^a.intersects[b] = bool
	add_native_method("intersects", Method::CT_DYNAMIC, _intersects, 1, 1);

	// ^a.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 1, 1);

	// ^hash:sql[query][$.limit(1) $.offset(2)]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// ^hash._keys[]
	add_native_method("_keys", Method::CT_DYNAMIC, _keys, 0, 0);	

	// ^hash._count[]
	add_native_method("_count", Method::CT_DYNAMIC, _count, 0, 0);	

	// ^hash.foreach[key;value]{code}[delim]
	add_native_method("foreach", Method::CT_DYNAMIC, _foreach, 2+1, 2+1+1);
}

// global variable

Methoded *hash_class;

// creator

Methoded *MHash_create(Pool& pool) {
	return hash_class=new(pool) MHash(pool);
}
