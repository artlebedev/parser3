/** @file
	Parser: @b hash parser class.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_charsets.h"
#include "pa_vhash.h"
#include "pa_varray.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_vmethod_frame.h"

volatile const char * IDENT_HASH_C="$Id: hash.C,v 1.172 2025/05/26 01:56:54 moko Exp $";

// class

class MHash: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VHash(); }

public:
	MHash();
};

// global variable

DECLARE_CLASS_VAR(hash, new MHash);

// methods

#ifndef DOXYGEN
class Hash_sql_event_handlers: public SQL_Driver_query_event_handlers {
	bool distinct;
	HashStringValue& result;
	Value* row_value;
	int column_index;
	ArrayString* columns;
	bool one_bool_column;
	Table2hash_value_type value_type;
	int columns_count;
public:
	Table* empty;
public:
	Hash_sql_event_handlers(bool adistinct, HashStringValue& aresult, Table2hash_value_type avalue_type):
		distinct(adistinct),
		result(aresult),
		row_value(0),
		column_index(0),
		columns(new ArrayString),
		one_bool_column(false),
		value_type(avalue_type),
		empty(0) {
	}

	bool add_column(SQL_Error& error, const char* str, size_t ) {
		try {
			if(columns_count){
				// another query in multi_statements mode
				columns=new ArrayString;
				columns_count=0;
			}
			*columns+=new String(str, String::L_TAINTED /* no length as 0x00 can be inside */);
			return false;
		} catch(...) {
			error=SQL_Error("exception occurred in Hash_sql_event_handlers::add_column");
			return true;
		}
	}

	bool before_rows(SQL_Error& error) { 
		columns_count=columns->count();
		if(columns_count<1) {
			error=SQL_Error("no columns");
			return true;
		}
		if(columns_count==1) {
			one_bool_column=true;
		} else {
			switch(value_type){
				case C_STRING: {
					if(columns_count>2){
						error=SQL_Error("only 2 columns allowed for $.type[string]");
						return true;
					}
					break;
				}
				case C_TABLE: {
					// create empty table which we'll copy later
					empty=new Table(columns);
					break;
				}
			}
		}
		return false;
	}

	bool add_row(SQL_Error& /*error*/) {
		column_index=0;
		return false;
	}

	bool add_row_cell(SQL_Error& error, const char *str, size_t ) {
		try {
			const String& cell=str ? *new String(str, String::L_TAINTED /* no length as 0x00 can be inside */) : String::Empty;

			if(column_index==columns_count){
				// should never happen, buggy driver case
				error=SQL_Error("columns index exceed the columns count");
				return true;
			}

			bool duplicate=false;
			if(one_bool_column) {
				duplicate=result.put_dont_replace(cell, &VBool::get(true));  // put. existed?
			} else if(column_index==0) {
				switch(value_type){
					case C_HASH: {
						VHash* row_vhash=new VHash;
						row_value=row_vhash;
						duplicate=result.put_dont_replace(cell, row_vhash); // put. existed?
						break;
					}
					case C_STRING: {
						VString* row_vstring=new VString();
						row_value=row_vstring;
						duplicate=result.put_dont_replace(cell, row_vstring);  // put. existed?
						break;
					}
					case C_TABLE: {
						VTable* vtable=(VTable*)result.get(cell);

						if(vtable) { // table with this key exist?
							if(!distinct) {
								duplicate=true;
								break;
							}
						} else {
							// no? creating table of same structure as source
							Table::Action_options table_options(0, 0);
							vtable=new VTable(new Table(*empty, table_options/*no rows, just structure*/));
							result.put(cell, vtable); // put
						}
						ArrayString* row=new ArrayString(columns_count);
						row_value=(Value*)row;
						*row+=&cell;
						*vtable->get_table()+=row;
						break;
					}
				}
			} else {
				switch(value_type) {
					case C_HASH: {
						row_value->get_hash()->put(*columns->get(column_index), new VString(cell));
						break;
					}
					case C_STRING: {
						VString* row_string=(VString*)row_value;
						row_string->set_string(cell);
						break;
					}
					case C_TABLE: {
						ArrayString* row=(ArrayString*)row_value;
						*row+=&cell;
						break;
					}
				}
			}

			if(duplicate & !distinct) {
				error=SQL_Error("duplicate key");
				return true;
			}

			column_index++;
			return false;
		} catch(...) {
			error=SQL_Error("exception occurred in Hash_sql_event_handlers::add_row_cell");
			return true;
		}
	}

};

#endif

static void _create_or_add(Request& r, MethodParams& params) {
	if(params.count()) {
		Value& vsrc=params.as_no_junction(0, PARAM_MUST_BE_HASH);
		VHashBase& self=GET_SELF(r, VHashBase);
		HashStringValue* self_hash=&(self.hash());
		HashStringValue* src_hash;

		if(VArray* src=dynamic_cast<VArray*>(&vsrc)) {
			for(ArrayValue::Iterator i(src->array()); i; i.next()){
				if(i.value())
					self_hash->put(String::Body::uitoa(i.index()), i.value());
			}
			return;
		} else if(VHashBase* src=dynamic_cast<VHashBase*>(&vsrc)) {
			src_hash=&(src->hash());

			if(src_hash==self_hash) // same: doing nothing
				return;

			if(Value* vdefault=src->get_default())
				self.set_default(vdefault);
		} else {
			// allows $h[^hash::create[non-blank string]], thus as_hash("param") is more correct, but is not backward compatible
			src_hash=vsrc.get_hash();
		}

		if(src_hash)
			for(HashStringValue::Iterator i(*src_hash); i; i.next())
				self_hash->put(i.key(), i.value());
	}
}

static void _sub(Request& r, MethodParams& params) {
	if(HashStringValue* src=params.as_hash(0, "param")) {
		HashStringValue* self=&(GET_SELF(r, VHashBase).hash());
		if(src==self) { // same: clearing
			self->clear();
			return;
		}
		for(HashStringValue::Iterator i(*src); i; i.next())
			self->remove(i.key());
	}
}

static void copy_all_dontoverwrite_to(HashStringValue::key_type key, HashStringValue::value_type value, HashStringValue* dest) {
	dest->put_dont_replace(key, value);
}
static void _union(Request& r, MethodParams& params) {
	// dest = copy of self
	Value& result=*new VHash(GET_SELF(r, VHashBase).hash());
	// dest += b
	if(HashStringValue* src=params.as_hash(0, "param"))
		src->for_each<HashStringValue*>(copy_all_dontoverwrite_to, result.get_hash());

	// return result
	r.write(result);
}

#ifndef DOXYGEN
struct Copy_intersection_to_info {
	HashStringValue* b;
	HashStringValue* dest;
};
#endif

static void copy_intersection_by_arg(HashStringValue::key_type key, HashStringValue::value_type, Copy_intersection_to_info *info) {
	if(HashStringValue::value_type value=info->b->get(key))
		info->dest->put_dont_replace(key, value);
}

static void copy_intersection_by_self(HashStringValue::key_type key, HashStringValue::value_type value, Copy_intersection_to_info *info) {
	if(info->b->get(key))
		info->dest->put_dont_replace(key, value);
}

static void _intersection(Request& r, MethodParams& params) {
	Value& result=*new VHash;

	bool order_by_arg=false;
	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1, "options")) {
			int valid_options=0;
			if(Value* vorder=options->get("order")) {
				const String &sorder=r.process(*vorder).as_string();
				if(sorder == "arg")
					order_by_arg=true;
				else if(sorder != "self")
					throw Exception(PARSER_RUNTIME, &sorder, "'order' must be 'self' or 'arg'");
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	if(HashStringValue* b=params.as_hash(0, "param")) {
		if(order_by_arg){
			Copy_intersection_to_info info={&GET_SELF(r, VHashBase).hash(), result.get_hash()};
			b->for_each<Copy_intersection_to_info*>(copy_intersection_by_arg, &info);
		} else {
			Copy_intersection_to_info info={b, result.get_hash()};
			GET_SELF(r, VHashBase).hash().for_each<Copy_intersection_to_info*>(copy_intersection_by_self, &info);
		}
	}

	r.write(result);
}

static bool intersects(	HashStringValue::key_type key, HashStringValue::value_type /*value*/, HashStringValue* b) {
	return b->get(key)!=0;
}

static void _intersects(Request& r, MethodParams& params) {
	bool result=false;

	if(HashStringValue* b=params.as_hash(0, "param")) {
		HashStringValue* self=&(GET_SELF(r, VHashBase).hash());
		if(b==self) {
			r.write(VBool::get(true));
			return;
		}
		result=self->first_that<HashStringValue*>(intersects, b)!=0;
	}

	// return result
	r.write(VBool::get(result));
}


extern Table2hash_value_type get_value_type(Value& vvalue_type);
extern int marshal_binds(HashStringValue& hash, SQL_Driver::Placeholder*& placeholders);
extern void unmarshal_bind_updates(HashStringValue& hash, int placeholder_count, SQL_Driver::Placeholder* placeholders);

static void _sql(Request& r, MethodParams& params) {
	Value& statement=params.as_junction(0, "statement must be code");

	HashStringValue* bind=0;
	ulong limit=SQL_NO_LIMIT;
	ulong offset=0;
	bool distinct=false;
	Table2hash_value_type value_type=C_HASH;
	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1, "sql options")) {
			int valid_options=0;
			for(HashStringValue::Iterator i(*options); i; i.next() ){
				String::Body key=i.key();
				Value* value=i.value();
				if(key == sql_bind_name) {
					bind=value->get_hash();
					valid_options++;
				} else if(key == sql_limit_name) {
					limit=(ulong)r.process(*value).as_double();
					valid_options++;
				} else if(key == sql_offset_name) {
					offset=(ulong)r.process(*value).as_double();
					valid_options++;
				} else if (key == sql_distinct_name) {
					distinct=r.process(*value).as_bool();
					valid_options++;
				} else if (key == sql_value_type_name) {
					value_type=get_value_type(r.process(*value));
					valid_options++;
				}
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

	HashStringValue& hash=GET_SELF(r, VHashBase).hash();
	hash.clear();
	Hash_sql_event_handlers handlers(distinct, hash, value_type);

	r.connection()->query(statement_cstr, placeholders_count, placeholders, offset, limit, handlers, statement_string);

	if(bind)
		unmarshal_bind_updates(*bind, placeholders_count, placeholders);
}

static void keys_collector(HashStringValue::key_type key, HashStringValue::value_type, Table *table) {
	Table::element_type row(new ArrayString(1));
	*row+=new String(key, String::L_TAINTED);
	*table+=row;
}
static void _keys(Request& r, MethodParams& params) {
	const String* keys_column_name;
	if(params.count()>0)
		keys_column_name=&params.as_string(0, COLUMN_NAME_MUST_BE_STRING);
	else 
		keys_column_name=new String("key");

	Table::columns_type columns(new ArrayString(1));
	*columns+=keys_column_name;
	Table* table=new Table(columns);

	GET_SELF(r, VHashBase).hash().for_each<Table*>(keys_collector, table);

	r.write(*new VTable(table));
}

static void _count(Request& r, MethodParams&) {
	r.write(*new VInt(GET_SELF(r, VHashBase).hash().count()));
}

static void _delete(Request& r, MethodParams& params) {
	if(params.count()>0)
		GET_SELF(r, VHashBase).hash().remove(params.as_string(0, "key must be string"));
	else
		GET_SELF(r, VHashBase).hash().clear();
}

static void _contains(Request& r, MethodParams& params) {
	VHashBase& self=GET_SELF(r, VHashBase);
	const String& key_name=params.as_string(0, "key must be string");
	bool result=SYMBOLS_EQ(key_name,_DEFAULT_SYMBOL) ? (self.get_default() != 0) : self.hash().contains(key_name);
	r.write(VBool::get(result));
}

static void _foreach(Request& r, MethodParams& params) {
	InCycle temp(r);

	const String* key_var_name=&params.as_string(0, "key-var name must be string");
	const String* value_var_name=&params.as_string(1, "value-var name must be string");
	Value* body_code=&params.as_junction(2, "body must be code");
	Value* delim_maybe_code=params.count()>3?&params[3]:0;
	Value& caller=*r.get_method_frame()->caller();

	if(key_var_name->is_empty()) key_var_name=0;
	if(value_var_name->is_empty()) value_var_name=0;

	HashStringValue& hash=GET_SELF(r, VHashBase).hash();

	if(delim_maybe_code){ // delimiter set
		bool need_delim=false;;
		for(HashStringValue::Iterator i(hash); i; i.next()){
			if(key_var_name){
				VString* vkey=new VString(i.key());
				r.put_element(caller, *key_var_name, vkey);
			}

			if(value_var_name)
				r.put_element(caller, *value_var_name, i.value());

			Value& sv_processed=r.process(*body_code);
			TempSkip4Delimiter skip(r);

			const String* s_processed=sv_processed.get_string();
			if(s_processed && !s_processed->is_empty()) { // we have body
				if(need_delim) // need delim & iteration produced string?
					r.write(r.process(*delim_maybe_code));
				else
					need_delim=true;
			}

			r.write(sv_processed);

			if(skip.check_break())
				break;
		}
	} else {
		for(HashStringValue::Iterator i(hash); i; i.next()){
			if(key_var_name){
				VString* vkey=new VString(i.key());
				r.put_element(caller, *key_var_name, vkey);
			}

			if(value_var_name)
				r.put_element(caller, *value_var_name, i.value());

			r.process_write(*body_code);

			if(r.check_skip_break())
				break;
		}
	}
}

enum AtResultType {
	AtResultTypeValue = 0,
	AtResultTypeKey = 1,
	AtResultTypeHash = 2
};

static Value& SingleElementHash(String::Body akey, Value* avalue) {
	Value& result=*new VHash;
	result.put_element(*new String(akey, String::L_TAINTED), avalue);
	return result;
}

#ifndef DOXYGEN
struct Hash_seq_item : public PA_Allocated {
	HashStringValue::Pair *hash_pair;
	union {
		const char *c_str;
		double d;
	} value;
};
#endif
static int sort_cmp_string(const void *a, const void *b) {
	return strcmp(
		static_cast<const Hash_seq_item *>(a)->value.c_str,
		static_cast<const Hash_seq_item *>(b)->value.c_str
	);
}
static int sort_cmp_double(const void *a, const void *b) {
	double va=static_cast<const Hash_seq_item *>(a)->value.d;
	double vb=static_cast<const Hash_seq_item *>(b)->value.d;
	if(va<vb)
		return -1;
	else if(va>vb)
		return +1;
	else 
		return 0;
}
static void _sort(Request& r, MethodParams& params){
#ifdef HASH_ORDER
	const String& key_var_name=params.as_string(0, "key-var name must be string");
	const String& value_var_name=params.as_string(1, "value-var name must be string");
	Value& key_maker=params.as_junction(2, "key-maker must be code");
	bool reverse=params.count()>3 /*..[desc|asc|]*/ && params.as_no_junction(3, "order must not be code").as_string()=="desc"; // default=asc

	const String* key_var=key_var_name.is_empty()? 0 : &key_var_name;
	const String* value_var=value_var_name.is_empty()? 0 : &value_var_name;
	VMethodFrame* context=r.get_method_frame()->caller();

	HashStringValue& hash=GET_SELF(r, VHashBase).hash();
	int count=hash.count();

	Hash_seq_item* seq=new Hash_seq_item[count];
	int pos=0;
	bool key_values_are_strings=true;

	for(HashStringValue::Iterator i(hash); i; i.next(), pos++ ){
		if(key_var)
			r.put_element(*context, *key_var, new VString(i.key()));
		if(value_var)
			r.put_element(*context, *value_var, i.value());
	
		Value& value=r.process(key_maker);
		if(pos==0) // determining key values type by first one
			key_values_are_strings=value.is_string();

		seq[pos].hash_pair=i.pair();
		if(key_values_are_strings)
			seq[pos].value.c_str=value.as_string().cstr();
		else
			seq[pos].value.d=value.as_expr_result().as_double();
	}

	// @todo: handle this elsewhere
	if(r.charsets.source().NAME()=="KOI8-R" && key_values_are_strings)
		for(pos=0; pos<count; pos++)
			if(*seq[pos].value.c_str)
				seq[pos].value.c_str=Charset::transcode(seq[pos].value.c_str, r.charsets.source(), pa_UTF8_charset).cstr();

	// sort keys
	qsort(seq, count, sizeof(Hash_seq_item), key_values_are_strings?sort_cmp_string:sort_cmp_double);

	// reorder hash as required in 'seq'
	hash.order_clear();
	if(reverse)
		for(pos=count-1; pos>=0; pos--)
			hash.order_next(seq[pos].hash_pair);
	else
		for(pos=0; pos<count; pos++)
			hash.order_next(seq[pos].hash_pair);

	delete[] seq;
#endif
}

static void _at(Request& r, MethodParams& params) {
	HashStringValue& hash=GET_SELF(r, VHashBase).hash();
	size_t count=hash.count();

	// misha@
	// I do not like that type is checked before whence.
	// But I do not like the idea to move it after whence (where process can be called) even more.
	AtResultType result_type=AtResultTypeValue;
	if(params.count() > 1) {
		const String& stype=params.as_string(1, "type must be string");
		if(stype == "key")
			result_type=AtResultTypeKey;
		else if(stype == "hash")
			result_type=AtResultTypeHash;
		else if(stype != "value")
			throw Exception(PARSER_RUNTIME, &stype, "type must be 'key', 'value' or 'hash'");
	}

	int pos=params.as_index(0, count, r);

	if(count && pos >= 0 && (size_t)pos < count){
		switch(result_type) {
			case AtResultTypeKey:
				{
#ifdef HASH_ORDER
					if(pos == 0) {
						r.write(*new VString(hash.first_key()));
					} else if((size_t)pos == count-1) {
						r.write(*new VString(hash.last_key()));
					} else
#endif
					{
						for(HashStringValue::Iterator i(hash); i; i.next(), pos-- )
							if(!pos){
								r.write(*new VString(i.key()));
								break;
							}
					}
					break;
				}
			case AtResultTypeValue:
				{
#ifdef HASH_ORDER
					if(pos == 0) {
						r.write(*hash.first_value());
					} else if((size_t)pos == count-1) {
						r.write(*hash.last_value());
					} else
#endif
					{
						for(HashStringValue::Iterator i(hash); i; i.next(), pos-- )
							if(!pos){
								r.write(*i.value());
								break;
							}
					}
					break;
				}
			case AtResultTypeHash:
				{
#ifdef HASH_ORDER
					if(pos == 0) {
						r.write(SingleElementHash(hash.first_key(), hash.first_value()));
					} else if((size_t)pos == count-1) {
						r.write(SingleElementHash(hash.last_key(), hash.last_value()));
					} else
#endif
					{
						for(HashStringValue::Iterator i(hash); i; i.next(), pos-- )
							if(!pos){
								r.write(SingleElementHash(i.key(), i.value()));
								break;
							}
					}
					break;
				}
		}
	}
}

static void _set(Request& r, MethodParams& params) {
	HashStringValue& hash=GET_SELF(r, VHashBase).hash();
	size_t count=hash.count();

	int pos=params.as_index(0, count, r);

	if(count && pos >= 0 && (size_t)pos < count){
#ifdef HASH_ORDER
		if((size_t)pos == count-1) {
			hash.last_pair()->value=&r.process(params[1]);
			return;
		} else
#endif
		{
			for(HashStringValue::Iterator i(hash); i; i.next(), pos-- )
				if(!pos){
					i.pair()->value=&r.process(params[1]);
					return;
				}
		}
	}

	if(count)
		throw Exception(PARSER_RUNTIME, 0, "index '%d' is out of range 0..%d", pos, count-1);
	throw Exception(PARSER_RUNTIME, 0, "index '%d' is out of range: hash is empty", pos);
}

extern String table_reverse_name;

static void _select(Request& r, MethodParams& params) {
	InCycle temp(r);
	const String* key_var_name=&params.as_string(0, "key-var name must be string");
	const String* value_var_name=&params.as_string(1, "value-var name must be string");
	Value& vcondition=params.as_expression(2, "condition must be number, bool or expression");

	if(key_var_name->is_empty()) key_var_name=0;
	if(value_var_name->is_empty()) value_var_name=0;

	HashStringValue& source_hash=GET_SELF(r, VHashBase).hash();
	Value& caller=*r.get_method_frame()->caller();

	int limit=source_hash.count();
	bool reverse=false;
	bool copy_default=false;

	if(params.count()>3)
		if(HashStringValue* options=params.as_hash(3)) {
			int valid_options=0;
			if(Value* vlimit=options->get(sql_limit_name)) {
				valid_options++;
				limit=r.process(*vlimit).as_int();
			}
			if(Value* vreverse=options->get(table_reverse_name)) {
				valid_options++;
				reverse=r.process(*vreverse).as_bool();
			}
			if(Value* vcopy_default=options->get(sql_default_name)) {
				valid_options++;
				copy_default=r.process(*vcopy_default).as_bool();
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	HashStringValue& result_hash=*new HashStringValue();

	if(limit>0){

#ifdef HASH_ORDER
		if(reverse){
			for(HashStringValue::ReverseIterator i(source_hash); i; i.prev()){
				if(key_var_name)
					r.put_element(caller, *key_var_name, new VString(i.key()));
				if(value_var_name)
					r.put_element(caller, *value_var_name, i.value());

				bool condition=r.process(vcondition).as_bool();

				if(r.check_skip_break())
					break;

				if(condition){
					result_hash.put(i.key(), i.value());
					if(!--limit)
						break;
				}
			}
		} else {
#else
		{
#endif
			for(HashStringValue::Iterator i(source_hash); i; i.next() ){
				if(key_var_name)
					r.put_element(caller, *key_var_name, new VString(i.key()));
				if(value_var_name)
					r.put_element(caller, *value_var_name, i.value());

				bool condition=r.process(vcondition).as_bool();

				if(r.check_skip_break())
					break;

				if(condition){
					result_hash.put(i.key(), i.value());
					if(!--limit)
						break;
				}
			}
		}
	}

	VHash *result=new VHash(result_hash);
	if(copy_default){
		result->set_default(GET_SELF(r, VHashBase).get_default());
	}

	r.write(*result);
}

static void _reverse(Request& r, MethodParams&) {
	VHashBase& self=GET_SELF(r, VHashBase);
	HashStringValue& source_hash=self.hash();
	HashStringValue& result_hash=*new HashStringValue();

#ifdef HASH_ORDER
	for(HashStringValue::ReverseIterator i(source_hash); i; i.prev())
		result_hash.put(i.key(), i.value());
#else
	for(HashStringValue::Iterator i(source_hash); i; i.next() )
		result_hash.put(i.key(), i.value());
#endif

	VHashBase& result=*new VHash(result_hash);
	if(Value* vdefault=self.get_default())
		result.set_default(vdefault);

	r.write(result);
}


static void _rename(Request& r, MethodParams& params) {
	HashStringValue& hash=GET_SELF(r, VHashBase).hash();

	if(params.count()>1){
		const String& key_from=params.as_string(0, "from key must be string");
		const String& key_to=params.as_string(1, "to key must be string");

		hash.rename(key_from, key_to);
	} else {
		HashStringValue* names=params.as_hash(0,"single parameter");

		for(HashStringValue::Iterator i(*names); i; i.next())
			hash.rename(i.key(), i.value()->as_string());
	}
}


static void _array(Request& r, MethodParams& params) {
	HashStringValue& hash=GET_SELF(r, VHashBase).hash();

	VArray& result=*new VArray;
	ArrayValue& array=result.array();

	if(params.count() > 0) {
		const String& smode=params.as_string(0, "mode must be string");
		if(smode == "keys"){
			for(HashStringValue::Iterator i(hash); i; i.next()){
				array+=new VString(i.key());
			}
		} else if(smode == "values"){
			for(HashStringValue::Iterator i(hash); i; i.next()){
				array+=i.value();
			}
		} else {
			throw Exception(PARSER_RUNTIME, &smode, "mode must be 'keys' or 'values'");
		}
	} else {
		for(HashStringValue::Iterator i(hash); i; i.next()){
			array.put(VArray::index(i.key()), i.value());
		}
	}

	r.write(result);
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
	// ^a.intersection[b][options hash] = hash
	add_native_method("intersection", Method::CT_DYNAMIC, _intersection, 1, 2);
	// ^a.intersects[b] = bool
	add_native_method("intersects", Method::CT_DYNAMIC, _intersects, 1, 1);

	// ^a.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 0, 1);

	// ^a.contains[key]
	add_native_method("contains", Method::CT_DYNAMIC, _contains, 1, 1);
	// backward
	add_native_method("contain", Method::CT_DYNAMIC, _contains, 1, 1);

	// ^hash::sql[query][options hash]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// ^hash._keys[[column name]]
	add_native_method("_keys", Method::CT_DYNAMIC, _keys, 0, 1);

	// ^hash._count[]
	add_native_method("_count", Method::CT_DYNAMIC, _count, 0, 0);

	// ^hash.foreach[key;value]{code}[delim]
	add_native_method("foreach", Method::CT_DYNAMIC, _foreach, 2+1, 2+1+1);

	// ^hash.sort[key;value]{string-key-maker}[[asc|desc]]
	// ^hash.sort[key;value](numeric-key-maker)[[asc|desc]]
	add_native_method("sort", Method::CT_DYNAMIC, _sort, 3, 4);

	// ^hash.select[key;value](bool-condition)[options hash]
	add_native_method("select", Method::CT_DYNAMIC, _select, 3, 4);

	// ^hash.reverse[]
	add_native_method("reverse", Method::CT_DYNAMIC, _reverse, 0, 0);

	// ^hash._at[first|last[;'key'|'value'|'hash']]
	// ^hash._at([-+]index)[['key'|'value'|'hash']]
	add_native_method("_at", Method::CT_DYNAMIC, _at, 1, 2);

	// ^hash.set[first|last;value]
	// ^hash.set([-+]index)[value]
	add_native_method("set", Method::CT_DYNAMIC, _set, 2, 2);

	// ^hash.rename[from;to]
	// ^hash.rename[ $.from[to] ... ]
	add_native_method("rename", Method::CT_DYNAMIC, _rename, 1, 2);

	// ^hash.array[[keys|values]]
	add_native_method("array", Method::CT_DYNAMIC, _array, 0, 1);

#ifdef FEATURE_GET_ELEMENT4CALL
	// aliases without "_"
	add_native_method("keys", Method::CT_DYNAMIC, _keys, 0, 1);
	add_native_method("count", Method::CT_DYNAMIC, _count, 0, 0);
	add_native_method("at", Method::CT_DYNAMIC, _at, 1, 2);
#endif

}
