/** @file
	Parser: @b hash parser class.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_HASH_C="$Date: 2003/11/20 15:35:29 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_vmethod_frame.h"

// class

class MHash: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VHash(); }

public:
	MHash();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(hash, new MHash, 0);

// methods

#ifndef DOXYGEN
class Hash_sql_event_handlers: public SQL_Driver_query_event_handlers {
	const String& statement_string; const char* statement_cstr;
	bool distinct;
	HashStringValue& rows_hash;
	HashStringValue* row_hash;
	int column_index;
	ArrayString columns;
public:
	Hash_sql_event_handlers(
		const String& astatement_string, const char* astatement_cstr,
		bool adistinct,
		HashStringValue& arows_hash): 
		statement_string(astatement_string), statement_cstr(astatement_cstr),
		distinct(adistinct),
		rows_hash(arows_hash),
		row_hash(0),
		column_index(0) {
	}
	bool add_column(SQL_Error& error, const char* str, size_t length) {
		try {
			columns+=new String(str, length, true);
			return false;
		} catch(...) {
			error=SQL_Error("exception occured in Hash_sql_event_handlers::add_column");
			return true;
		}
	}
	bool before_rows(SQL_Error& error) { 
		if(columns.count()<=1) {
			error=SQL_Error("parser.runtime",
				/*method_name,*/
				"column count must be more than 1 to create a hash");
			return true;
		}

		return false;
	}
	bool add_row(SQL_Error& /*error*/) {
		column_index=0;
		return false;
	}
	bool add_row_cell(SQL_Error& error, const char *ptr, size_t length) {
		try {
			String& cell=*new String;
			if(length)
				cell.append_know_length(ptr, length, String::L_TAINTED);
			if(column_index==0) {
				VHash* row_vhash=new VHash;
				row_hash=&row_vhash->hash();
				if(rows_hash.put_dont_replace(cell, row_vhash)) // put. existed?
					if(!distinct) {
						error=SQL_Error("parser.runtime",
							/*cell,*/
							"duplicate key");
						return true;
					}
			} else
				row_hash->put(*columns[column_index], new VString(cell));
			column_index++;

			return false;
		} catch(...) {
			error=SQL_Error("exception occured in Hash_sql_event_handlers::add_row_cell");
			return true;
		}
	}

};
#endif

static void copy_all_overwrite_to(
								  HashStringValue::key_type key, 
								  HashStringValue::value_type value, 
								  HashStringValue* dest) {
	dest->put(key, value);
}
static void _create_or_add(Request& r, MethodParams& params) {
	if(params.count()) {
		Value& vb=params.as_no_junction(0, "param must be hash");
		if(HashStringValue* b=vb.get_hash())
			b->for_each(copy_all_overwrite_to, &(GET_SELF(r, VHash).hash()));
	}
}

static void remove_key_from(
							HashStringValue::key_type key, 
							HashStringValue::value_type /*value*/, 
							HashStringValue* dest) {
	dest->remove(key);
}
static void _sub(Request& r, MethodParams& params) {
	Value& vb=params.as_no_junction(0, "param must be hash");
	if(HashStringValue* b=vb.get_hash())
		b->for_each(remove_key_from, &GET_SELF(r, VHash).hash());
}

static void copy_all_dontoverwrite_to(
								  HashStringValue::key_type key, 
								  HashStringValue::value_type value, 
								  HashStringValue* dest) {
	dest->put_dont_replace(key, value);
}
static void _union(Request& r, MethodParams& params) {
	// dest = copy of self
	Value& result=*new VHash(GET_SELF(r, VHash).hash());
	// dest += b
	Value& vb=params.as_no_junction(0, "param must be hash");
	if(HashStringValue* b=vb.get_hash())
		b->for_each(copy_all_dontoverwrite_to, result.get_hash());

	// return result
	r.write_no_lang(result);
}

#ifndef DOXYGEN
struct Copy_intersection_to_info {
	HashStringValue* b;
	HashStringValue* dest;
};
#endif
static void copy_intersection_to(
								 HashStringValue::key_type key, 
								 HashStringValue::value_type value, 
								 Copy_intersection_to_info *info) {
	if(info->b->get(key))
		info->dest->put_dont_replace(key, value);
}
static void _intersection(Request& r, MethodParams& params) {
	Value& result=*new VHash;
	// dest += b
	Value& vb=params.as_no_junction(0, "param must be hash");
	if(HashStringValue* b=vb.get_hash()) {
		Copy_intersection_to_info info={b, result.get_hash()};
		GET_SELF(r, VHash).hash().for_each(copy_intersection_to, &info);
	}

	// return result
	r.write_no_lang(result);
}

static bool intersects(
					   HashStringValue::key_type key, 
					   HashStringValue::value_type /*value*/, 
					   HashStringValue* b) {
	return b->get(key)!=0;
}

static void _intersects(Request& r, MethodParams& params) {
	bool result=false;

	Value& vb=params.as_no_junction(0, "param must be hash");
	if(HashStringValue* b=vb.get_hash())
		result=GET_SELF(r, VHash).hash().first_that(intersects, b)!=0;

	// return result
	r.write_no_lang(*new VBool(result));
}


extern String sql_limit_name;
extern String sql_offset_name;
extern String sql_default_name;
extern String sql_distinct_name;
static void _sql(Request& r, MethodParams& params) {
	Value& statement=params.as_junction(0, "statement must be code");

	ulong limit=0;
	ulong offset=0;
	bool distinct=false;
	if(params.count()>1) {
		Value& voptions=params.as_no_junction(1, "options must be hash, not code");
		if(!voptions.is_string())
			if(HashStringValue* options=voptions.get_hash()) {
				int valid_options=0;
				if(Value* vlimit=options->get(sql_limit_name)) {
					valid_options++;
					limit=(ulong)r.process_to_value(*vlimit).as_double();
				}
				if(Value* voffset=options->get(sql_offset_name)) {
					valid_options++;
					offset=(ulong)r.process_to_value(*voffset).as_double();
				}
				if(Value* vdistinct=options->get(sql_distinct_name)) {
					valid_options++;
					distinct=r.process_to_value(*vdistinct).as_bool();
				}
				if(valid_options!=options->count())
					throw Exception("parser.runtime",
						0,
						"called with invalid option");
			} else
				throw Exception("parser.runtime",
					0,
					"options must be hash");
	}

	Temp_lang temp_lang(r, String::L_SQL);
	const String& statement_string=r.process_to_string(statement);
	const char* statement_cstr=
		statement_string.cstr(String::L_UNSPECIFIED, r.connection());
	HashStringValue& hash=GET_SELF(r, VHash).hash();
	hash.clear();	
	Hash_sql_event_handlers handlers(
		statement_string, statement_cstr, 
		distinct,
		hash);
	r.connection()->query(
		statement_cstr, offset, limit,
		handlers,
		statement_string);
}

static void keys_collector(
			   HashStringValue::key_type key, 
			   HashStringValue::value_type, 
			   Table *table) {
	Table::element_type row(new ArrayString);
	*row+=new String(key, String::L_TAINTED);
	*table+=row;
}
static void _keys(Request& r, MethodParams&) {
	Table::columns_type columns(new ArrayString);
	*columns+=new String("key");
	Table* table=new Table(columns);

	GET_SELF(r, VHash).hash().for_each(keys_collector, table);

	r.write_no_lang(*new VTable(table));
}

static void _count(Request& r, MethodParams&) {
	r.write_no_lang(*new VInt(GET_SELF(r, VHash).hash().count()));
}

static void _delete(Request& r, MethodParams& params) {

	GET_SELF(r, VHash).hash().remove(params.as_string(0, "key must be string"));
}

#ifndef DOXYGEN
struct Foreach_info {
	Request *r;
	const String* key_var_name;
	const String* value_var_name;
	Value* body_code;
	Value* delim_maybe_code;

	VString* vkey;
	bool need_delim;
};
#endif
static void one_foreach_cycle(						   
			      HashStringValue::key_type akey, 
			      HashStringValue::value_type avalue, 
			      Foreach_info *info) {
	info->vkey->set_string(*new String(akey, String::L_TAINTED));
	Value& ncontext=*info->r->get_method_frame()->caller();
	ncontext.put_element(*info->key_var_name, info->vkey, false);
	ncontext.put_element(*info->value_var_name, avalue, false);

	StringOrValue sv_processed=info->r->process(*info->body_code);
	const String* s_processed=sv_processed.get_string();
	if(info->delim_maybe_code && s_processed && s_processed->length()) { // delimiter set and we have body
		if(info->need_delim) // need delim & iteration produced string?
			info->r->write_pass_lang(info->r->process(*info->delim_maybe_code));
		info->need_delim=true;
	}
	info->r->write_pass_lang(sv_processed);
}
static void _foreach(Request& r, MethodParams& params) {
	Foreach_info info={0};
	info.r=&r;
	info.key_var_name=&params.as_string(0, "key-var name must be string");
	info.value_var_name=&params.as_string(1, "value-var name must be string");
	info.body_code=&params.as_junction(2, "body must be code");
	info.delim_maybe_code=params.count()>3?params.get(3):0;
	info.vkey=new VString;

	VHash& self=GET_SELF(r, VHash);
	HashStringValue& hash=self.hash();
	VHash_lock lock(self);
	hash.for_each(one_foreach_cycle, &info);
}

// constructor

MHash::MHash(): Methoded("hash") 
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
