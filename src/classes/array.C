/** @file
	Parser: @b array parser class.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_charsets.h"
#include "pa_varray.h"
#include "pa_vvoid.h"
#include "pa_sql_connection.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_vmethod_frame.h"

volatile const char * IDENT_ARRAY_C="$Id: array.C,v 1.16 2024/10/02 17:58:15 moko Exp $";

// class

class MArray: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VArray; }

public:
	MArray();
};

// global variable

DECLARE_CLASS_VAR(array, new MArray);

const char* const PARAM_ARRAY_OR_HASH = "param must be array or hash";
const char* const PARAM_INDEX = "index must be integer";

// methods

static void _create_or_add(Request& r, MethodParams& params) {
	if(params.count()) {
		Value& vsrc=params.as_no_junction(0, PARAM_ARRAY_OR_HASH);
		VArray& self=GET_SELF(r, VArray);
		ArrayValue& self_array=self.array();

		if(VArray* src=dynamic_cast<VArray*>(&vsrc)) {
			if(src==&self)
				return;
			if(self_array.count()){
				for(ArrayValue::Iterator i(src->array()); i; i.next()){
					if(i.value())
						self_array.put(i.index(), i.value());
				}
			} else {
				self_array.append(src->array());
			}
		} else {
			HashStringValue* src_hash=vsrc.as_hash("param must be array or");
			if(!src_hash)
				return;
			for(HashStringValue::Iterator i(*src_hash); i; i.next()){
				self_array.put(VArray::index(i.key()), i.value());
			}
		}
		self.invalidate();
	}
}

static ArrayValue::Action_options get_action_options(Request& r, MethodParams& params, size_t options_index) {
	ArrayValue::Action_options result;
	if(params.count() <= options_index)
		return result;

	HashStringValue* options=params.as_hash(options_index);
	if(!options)
		return result;

	result.defined=true;
	int valid_options=0;

	if(Value* voffset=options->get(sql_offset_name)) {
		valid_options++;
		int offset=r.process(*voffset).as_int();
		result.offset=offset < 0 ? 0 : offset;
	}
	if(Value* vlimit=options->get(sql_limit_name)) {
		valid_options++;
		int limit=r.process(*vlimit).as_int();
		result.limit=limit < 0 ? 0: limit;
	}

	if(valid_options!=options->count())
		throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);

	return result;
}

static void _join(Request& r, MethodParams& params) {
	Value& vsrc=params.as_no_junction(0, PARAM_ARRAY_OR_HASH);
	ArrayValue::Action_options o=get_action_options(r, params, 1);

	VArray& self=GET_SELF(r, VArray);
	ArrayValue& self_array=self.array();

	if(VArray* src=dynamic_cast<VArray*>(&vsrc)) {
		if(src==&self)
			throw Exception(PARSER_RUNTIME, 0, "source and destination are the same array");

		if(o.defined){
			for(ArrayValue::Iterator i(src->array()); i; i.next()){
				if(i.value()){
					if(o.offset > 0){
						o.offset--;
						continue;
					}
					if(o.limit-- == 0)
						break;
					self_array+=i.value();
				}
			}
		} else {
			for(ArrayValue::Iterator i(src->array()); i; i.next()){
				if(i.value())
					self_array+=i.value();
			}
		}
	} else {
		HashStringValue* src_hash=vsrc.as_hash("param must be array or");
		if(!src_hash)
			return;
		if(o.defined){
			for(HashStringValue::Iterator i(*src_hash); i; i.next()){
				if(o.offset > 0){
					o.offset--;
					continue;
				}
				if(o.limit-- == 0)
					break;
				self_array+=i.value();
			}
		} else {
			for(HashStringValue::Iterator i(*src_hash); i; i.next()){
				self_array+=i.value();
			}
		}
	}
	self.invalidate();
}

#ifndef DOXYGEN

#define STRING(str) ((str) ? *new String(str, String::L_TAINTED /* no length as 0x00 can be inside */) : String::Empty)

class SparseArray_sql_event_handlers: public SQL_Driver_query_event_handlers {
	bool distinct;
	ArrayValue& result;
	Value* row_value;
	int column_index;
	ArrayString* columns;
	bool one_bool_column;
	Table2hash_value_type value_type;
	int columns_count;
public:
	Table* empty;
public:
	SparseArray_sql_event_handlers(bool adistinct, ArrayValue& aresult, Table2hash_value_type avalue_type):
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
			*columns+=&STRING(str);
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
						error=SQL_Error("only 2 columns allowed for $.type[string] and $.sparse(true)");
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
			if(column_index==columns_count){
				// should never happen, buggy driver case
				error=SQL_Error("columns index exceed the columns count");
				return true;
			}

			bool duplicate=false;
			if(one_bool_column) {
				size_t index=str ? pa_atoui(str) : 0;
				duplicate=result.put_dont_replace(index, &VBool::get(true));  // put. existed?
			} else if(column_index==0) {
				size_t index=str ? pa_atoui(str) : 0;
				switch(value_type){
					case C_HASH: {
						VHash* row_vhash=new VHash;
						row_value=row_vhash;
						duplicate=result.put_dont_replace(index, row_vhash); // put. existed?
						break;
					}
					case C_STRING: {
						VString* row_vstring=new VString();
						row_value=row_vstring;
						duplicate=result.put_dont_replace(index, row_vstring);  // put. existed?
						break;
					}
					case C_TABLE: {
						VTable* vtable=(VTable*)result.get(index);

						if(vtable) { // table with this key exist?
							if(!distinct) {
								duplicate=true;
								break;
							}
						} else {
			 				// no? creating table of same structure as source
							Table::Action_options table_options(0, 0);
							vtable=new VTable(new Table(*empty, table_options/*no rows, just structure*/));
							result.put(index, vtable); // put
						}
						ArrayString* row=new ArrayString(columns_count);
						*row+=&STRING(str);
						*vtable->get_table()+=row;
						row_value=(Value*)row;
						break;
					}
				}
			} else {
				const String& cell=STRING(str);
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
		} catch(const Exception& e) {
			error=SQL_Error(e.type(), e.comment());
			return true;
		} catch(...) {
			error=SQL_Error("exception occurred in Hash_sql_event_handlers::add_row_cell");
			return true;
		}
	}
};

class Array_sql_event_handlers: public SQL_Driver_query_event_handlers {
	ArrayValue& result;
	Value* row_value;
	int column_index;
	ArrayString* columns;
	Table2hash_value_type value_type;
	int columns_count;
public:
	Table* empty;
public:
	Array_sql_event_handlers(ArrayValue& aresult, Table2hash_value_type avalue_type):
		result(aresult),
		row_value(0),
		column_index(0),
		columns(new ArrayString),
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
			*columns+=&STRING(str);
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
		switch(value_type){
			case C_STRING: {
				if(columns_count>1){
					error=SQL_Error("only one column allowed for $.type[string]");
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
		return false;
	}

	bool add_row(SQL_Error& /*error*/) {
		column_index=0;
		return false;
	}

	bool add_row_cell(SQL_Error& error, const char *str, size_t ) {
		try {
			if(column_index==columns_count){
				// should never happen, buggy driver case
				error=SQL_Error("columns index exceed the columns count");
				return true;
			}

			if(column_index==0) {
				switch(value_type){
					case C_HASH: {
						VHash* row_vhash=new VHash;
						row_value=row_vhash;
						result+=row_vhash;
						break;
					}
					case C_STRING: {
						VString* row_vstring=new VString();
						row_value=row_vstring;
						result+=row_vstring;
						break;
					}
					case C_TABLE: {
		 				// creating table of same structure as source
						Table::Action_options table_options(0, 0);
						VTable* vtable=new VTable(new Table(*empty, table_options/*no rows, just structure*/));
						ArrayString* row=new ArrayString(columns_count);
						*vtable->get_table()+=row;
						row_value=(Value*)row;
						result+=vtable;
						break;
					}
				}
			}

			const String& cell=STRING(str);
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

			column_index++;
			return false;
		} catch(const Exception& e) {
			error=SQL_Error(e.type(), e.comment());
			return true;
		} catch(...) {
			error=SQL_Error("exception occurred in Hash_sql_event_handlers::add_row_cell");
			return true;
		}
	}
};

#endif

extern Table2hash_value_type get_value_type(Value& vvalue_type);
extern int marshal_binds(HashStringValue& hash, SQL_Driver::Placeholder*& placeholders);
extern void unmarshal_bind_updates(HashStringValue& hash, int placeholder_count, SQL_Driver::Placeholder* placeholders);

static void _sql(Request& r, MethodParams& params) {
	Value& statement=params.as_junction(0, "statement must be code");

	HashStringValue* bind=0;
	ulong limit=SQL_NO_LIMIT;
	ulong offset=0;
	bool distinct=false;
	bool sparse=false;
	Table2hash_value_type value_type=C_HASH;
	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1, "sql options")) {
			int valid_options=0;
			bool distinct_specified=false;
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
					distinct_specified=true;
					valid_options++;
				} else if (key == sql_value_type_name) {
					value_type=get_value_type(r.process(*value));
					valid_options++;
				} else if (key == "sparse") {
					sparse=r.process(*value).as_bool();
					valid_options++;
				}
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			if(distinct_specified && !sparse)
				throw Exception(PARSER_RUNTIME, 0, "'distinct' option can only be used when $.sparse(true) is specified");
		}

	SQL_Driver::Placeholder* placeholders=0;
	uint placeholders_count=0;
	if(bind)
		placeholders_count=marshal_binds(*bind, placeholders);

	const String& statement_string=r.process_to_string(statement);
	const char* statement_cstr=statement_string.untaint_cstr(String::L_SQL, r.connection());

	VArray& self=GET_SELF(r, VArray);

	self.array().clear(); self.invalidate(); // just in case if called as method

	if(sparse){
		SparseArray_sql_event_handlers handlers(distinct, self.array(), value_type);
		r.connection()->query(statement_cstr, placeholders_count, placeholders, offset, limit, handlers, statement_string);
	} else {
		Array_sql_event_handlers handlers(self.array(), value_type);
		r.connection()->query(statement_cstr, placeholders_count, placeholders, offset, limit, handlers, statement_string);
	}

	if(bind)
		unmarshal_bind_updates(*bind, placeholders_count, placeholders);
}



static void mid(Request& r, size_t offset=0, size_t limit=ARRAY_OPTION_LIMIT_ALL) {
	ArrayValue& array=GET_SELF(r, VArray).array();
	if(limit>0){
		VArray *result=new VArray;
		ArrayValue& result_array=result->array();
		for(ArrayValue::Iterator i(array); i; i.next()){
			if(i.value()){
				if(offset > 0){
					offset--;
					continue;
				}
				if(limit-- == 0)
					break;
				result_array+=i.value();
			}
		}
		r.write(*result);
	} else {
		r.write(*new VArray);
	}
}

static void _left(Request& r, MethodParams& params) {
	int sn=params.as_int(0, "n must be int", r);
	mid(r, 0, sn < 0 ? 0 : sn);
}

static void _right(Request& r, MethodParams& params) {
	int sn=params.as_int(0, "n must be int", r);

	if(sn>0){
		size_t used=GET_SELF(r, VArray).array().used();
		if(sn<used){
			mid(r, used-sn, sn);
		} else {
			mid(r);
		}
	} else {
		mid(r, 0, 0);
	}
}

static void _mid(Request& r, MethodParams& params) {
	const String& string=GET_SELF(r, VString).string();

	int begin=params.as_int(0, "p must be int", r);
	if(begin<0)
		throw Exception(PARSER_RUNTIME, 0,  "p(%d) must be >=0", begin);

	size_t end;
	size_t length=0;

	if(params.count()>1) {
		int n=params.as_int(1, "n must be int", r);
		if(n<0)
			throw Exception(PARSER_RUNTIME, 0, "n(%d) must be >=0", n);
		mid(r, begin, n);
	} else {
		mid(r, begin);
	}
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

	ArrayValue& array=GET_SELF(r, VArray).array();
	for(ArrayValue::Iterator i(array); i; i.next()){
		if(i.value()){
			Table::element_type row(new ArrayString(1));
			*row+=new String(i.key(), String::L_TAINTED);
			*table+=row;
		}
	}

	r.write(*new VTable(table));
}

static void _count(Request& r, MethodParams& params) {
	ArrayValue& array=GET_SELF(r, VArray).array();
	if(params.count()>0){
		const String& what=params.as_string(0, PARAMETER_MUST_BE_STRING);
		if(!what.is_empty()){
			if(what != "all")
				throw Exception(PARSER_RUNTIME, &what, "param must be empty or 'all'");
			return r.write(*new VInt(array.count()));
		}
	}
	r.write(*new VInt(array.used()));
}

static void _append(Request& r, MethodParams& params) {
	VArray& self=GET_SELF(r, VArray);
	ArrayValue& array=self.array();

	int count=params.count();

	for(int i=0; i<count; i++){
		array+=&r.process(params[i]);
	}
	self.invalidate();
}

static void _insert(Request& r, MethodParams& params) {
	VArray& self=GET_SELF(r, VArray);
	ArrayValue& array=self.array();

	int count=params.count();
	size_t index=VArray::index(params.as_int(0, PARAM_INDEX, r));

	for(int i=1; i<count; i++){
		array.insert(index++, &r.process(params[i]));
	}
	self.invalidate();
}

static void _delete(Request& r, MethodParams& params) {
	VArray& self=GET_SELF(r, VArray);
	if(params.count()>0)
		self.array().clear(VArray::index(params.as_int(0, PARAM_INDEX, r)));
	else
		self.array().clear();
	self.invalidate();
}

static void _remove(Request& r, MethodParams& params) {
	VArray& self=GET_SELF(r, VArray);
	self.array().remove(VArray::index(params.as_int(0, PARAM_INDEX, r)));
	self.invalidate();
}

static void _contains(Request& r, MethodParams& params) {
	VArray& self=GET_SELF(r, VArray);
	bool result=self.contains(VArray::index(params.as_int(0, PARAM_INDEX, r)));
	r.write(VBool::get(result));
}

static void _for(Request& r, MethodParams& params) {
	InCycle temp(r);

	const String* key_var_name=&params.as_string(0, "key-var name must be string");
	const String* value_var_name=&params.as_string(1, "value-var name must be string");
	Value* body_code=&params.as_junction(2, "body must be code");
	Value* delim_maybe_code=params.count()>3 ? &params[3] : 0;
	Value& caller=*r.get_method_frame()->caller();

	if(key_var_name->is_empty()) key_var_name=0;
	if(value_var_name->is_empty()) value_var_name=0;

	ArrayValue& array=GET_SELF(r, VArray).array();

	if(delim_maybe_code){ // delimiter set
		bool need_delim=false;
		for(ArrayValue::Iterator i(array); i; i.next()){
			if(key_var_name){
				VString* vkey=new VString(*new String(i.key(), String::L_TAINTED));
				r.put_element(caller, *key_var_name, vkey);
			}

			if(value_var_name)
				r.put_element(caller, *value_var_name, i.value() ? i.value() : VVoid::get());

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
		for(ArrayValue::Iterator i(array); i; i.next()){
			if(key_var_name){
				VString* vkey=new VString(*new String(i.key(), String::L_TAINTED));
				r.put_element(caller, *key_var_name, vkey);
			}

			if(value_var_name)
				r.put_element(caller, *value_var_name, i.value() ? i.value() : VVoid::get());

			r.process_write(*body_code);

			if(r.check_skip_break())
				break;
		}
	}
}

static void _foreach(Request& r, MethodParams& params) {
	InCycle temp(r);

	const String* key_var_name=&params.as_string(0, "key-var name must be string");
	const String* value_var_name=&params.as_string(1, "value-var name must be string");
	Value* body_code=&params.as_junction(2, "body must be code");
	Value* delim_maybe_code=params.count()>3 ? &params[3] : 0;
	Value& caller=*r.get_method_frame()->caller();

	if(key_var_name->is_empty()) key_var_name=0;
	if(value_var_name->is_empty()) value_var_name=0;

	ArrayValue& array=GET_SELF(r, VArray).array();

	if(delim_maybe_code){ // delimiter set
		bool need_delim=false;
		for(ArrayValue::Iterator i(array); i; i.next()){
			if(i.value()){
				if(key_var_name){
					VString* vkey=new VString(*new String(i.key(), String::L_TAINTED));
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
		}
	} else {
		for(ArrayValue::Iterator i(array); i; i.next()){
			if(i.value()){
				if(key_var_name){
					VString* vkey=new VString(*new String(i.key(), String::L_TAINTED));
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
}

#ifndef DOXYGEN
struct Array_seq_item : public PA_Allocated {
	Value *array_data;
	union {
		const char *c_str;
		double d;
	} value;
};
#endif

static int sort_cmp_string(const void *a, const void *b) {
	return strcmp(
		static_cast<const Array_seq_item *>(a)->value.c_str,
		static_cast<const Array_seq_item *>(b)->value.c_str
	);
}
static int sort_cmp_double(const void *a, const void *b) {
	double va=static_cast<const Array_seq_item *>(a)->value.d;
	double vb=static_cast<const Array_seq_item *>(b)->value.d;
	if(va<vb)
		return -1;
	else if(va>vb)
		return +1;
	else 
		return 0;
}

static void _sort(Request& r, MethodParams& params){
	const String& key_var_name=params.as_string(0, "key-var name must be string");
	const String& value_var_name=params.as_string(1, "value-var name must be string");
	Value& key_maker=params.as_junction(2, "key-maker must be code");
	bool reverse=params.count()>3 && params.as_no_junction(3, "order must not be code").as_string()=="desc"; // default=asc

	const String* key_var=key_var_name.is_empty()? 0 : &key_var_name;
	const String* value_var=value_var_name.is_empty()? 0 : &value_var_name;
	VMethodFrame* context=r.get_method_frame()->caller();

	VArray& self=GET_SELF(r, VArray);
	ArrayValue& array=self.array();
	int count=array.used(); // not array.count()

	Array_seq_item* seq=new Array_seq_item[count];
	int pos=0;
	bool key_values_are_strings=true;

	for(ArrayValue::Iterator i(array); i; i.next() ){
		if(i.value()){
			if(key_var)
				r.put_element(*context, *key_var, new VString(*new String(i.key(), String::L_TAINTED)));
			if(value_var)
				r.put_element(*context, *value_var, i.value());

			Value& value=r.process(key_maker);
			if(pos==0) // determining key values type by first one
				key_values_are_strings=value.is_string();

			seq[pos].array_data=i.value();
			if(key_values_are_strings)
				seq[pos++].value.c_str=value.as_string().cstr();
			else
				seq[pos++].value.d=value.as_expr_result().as_double();
		}
	}

	// @todo: handle this elsewhere
	if(r.charsets.source().NAME()=="KOI8-R" && key_values_are_strings)
		for(pos=0; pos<count; pos++)
			if(*seq[pos].value.c_str)
				seq[pos].value.c_str=Charset::transcode(seq[pos].value.c_str, r.charsets.source(), pa_UTF8_charset).cstr();

	// sort keys
	qsort(seq, count, sizeof(Array_seq_item), key_values_are_strings ? sort_cmp_string : sort_cmp_double);

	// reorder array as required in 'seq'
	array.clear();
	if(reverse)
		for(pos=count-1; pos>=0; pos--)
			array+=seq[pos].array_data;
	else
		for(pos=0; pos<count; pos++)
			array+=seq[pos].array_data;

	delete[] seq;
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

static void _at(Request& r, MethodParams& params) {
	VArray& self=GET_SELF(r, VArray);
	ArrayValue& array=self.array();
	size_t count=array.used(); // not array.count()

	int pos=0;

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

	Value& vwhence=params[0];
	if(vwhence.is_string()) {
		const String& swhence=*vwhence.get_string();
		if(swhence == "last")
			pos=count-1;
		else if(swhence != "first")
			throw Exception(PARSER_RUNTIME, &swhence, "whence must be 'first', 'last' or expression");
	} else {
		pos=r.process(vwhence).as_int();
		if(pos < 0)
			pos+=count;
	}

	if(count && pos >= 0 && (size_t)pos < count){
		switch(result_type) {
			case AtResultTypeKey:
				{
					for(ArrayValue::Iterator i(array); i; i.next() ){
						if(i.value() && !(pos--)){
							r.write(*new VString(*new String(i.key(), String::L_TAINTED)));
							break;
						}
					}
					break;
				}
			case AtResultTypeValue:
				{
					for(ArrayValue::Iterator i(array); i; i.next() )
						if(i.value() &&!(pos--)){
							r.write(*i.value());
							break;
						}
					break;
				}
			case AtResultTypeHash:
				{
					for(ArrayValue::Iterator i(array); i; i.next() )
						if(i.value() &&!(pos--)){
							r.write(SingleElementHash(i.key(), i.value()));
							break;
						}
					break;
				}
		}
	}
}


extern String table_reverse_name;

static void _select(Request& r, MethodParams& params) {
	InCycle temp(r);
	const String* key_var_name=&params.as_string(0, "key-var name must be string");
	const String* value_var_name=&params.as_string(1, "value-var name must be string");
	Value& vcondition=params.as_expression(2, "condition must be number, bool or expression");

	if(key_var_name->is_empty()) key_var_name=0;
	if(value_var_name->is_empty()) value_var_name=0;

	ArrayValue& source_array=GET_SELF(r, VArray).array();
	Value& caller=*r.get_method_frame()->caller();

	int limit=source_array.count();
	bool reverse=false;

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
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	VArray *result=new VArray;
	ArrayValue& result_array=result->array();

	if(limit>0){
		if(reverse){
			for(ArrayValue::ReverseIterator i(source_array); i; ){
				if(Value *value=i.prev()){ // here for correct i.key()
					if(key_var_name)
						r.put_element(caller, *key_var_name, new VString(*new String(i.key(), String::L_TAINTED)));
					if(value_var_name)
						r.put_element(caller, *value_var_name, value);

					bool condition=r.process(vcondition).as_bool();

					if(r.check_skip_break())
						break;

					if(condition){
						result_array+=value;
						if(!--limit)
							break;
					}
				}
			}
		} else {
			for(ArrayValue::Iterator i(source_array); i; i.next() ){
				if(Value *value=i.value()){
					if(key_var_name)
						r.put_element(caller, *key_var_name, new VString(*new String(i.key(), String::L_TAINTED)));
					if(value_var_name)
						r.put_element(caller, *value_var_name, value);

					bool condition=r.process(vcondition).as_bool();

					if(r.check_skip_break())
						break;

					if(condition){
						result_array+=value;
						if(!--limit)
							break;
					}
				}
			}
		}
	}

	r.write(*result);
}

static void _reverse(Request& r, MethodParams& params) {
	ArrayValue& source_array=GET_SELF(r, VArray).array();

	VArray& result=*new VArray(source_array.count());
	ArrayValue& result_array=result.array();

	for(ArrayValue::ReverseIterator i(source_array); i; ){
		result_array+=i.prev();
	}

	r.write(result);
}


// constructor

MArray::MArray(): Methoded(VARRAY_TYPE) {

	// ^array::create[[copy_from]]
	add_native_method("create", Method::CT_DYNAMIC, _create_or_add, 0, 1);
	// ^array.add[add_from]
	add_native_method("add", Method::CT_DYNAMIC, _create_or_add, 1, 1);
	// ^array.join[join_from[;options]]
	add_native_method("join", Method::CT_DYNAMIC, _join, 1, 2);

	// ^array.left(n)
	add_native_method("left", Method::CT_DYNAMIC, _left, 1, 1);
	// ^array.right(n)
	add_native_method("right", Method::CT_DYNAMIC, _right, 1, 1);
	// ^array.mid(p)
	// ^array.mid(p;n)
	add_native_method("mid", Method::CT_DYNAMIC, _mid, 1, 2);

	// ^array::new[value;value]
	add_native_method("new", Method::CT_DYNAMIC, _append, 0, 10000);
	// ^array.append[value;value]
	add_native_method("append", Method::CT_DYNAMIC, _append, 1, 10000);
	// ^array.insert[index;value...]
	add_native_method("insert", Method::CT_DYNAMIC, _insert, 2, 10000);

	// ^array.delete[index]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 0, 1);
	// ^array.remove[index]
	add_native_method("remove", Method::CT_DYNAMIC, _remove, 1, 1);

	// ^array.contains[index]
	add_native_method("contains", Method::CT_DYNAMIC, _contains, 1, 1);

	// ^array::sql[query][options array]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// ^array._keys[[column name]]
	add_native_method("_keys", Method::CT_DYNAMIC, _keys, 0, 1);

	// ^array._count[[all]]
	add_native_method("_count", Method::CT_DYNAMIC, _count, 0, 1);

	// ^array.for[index;value]{code}[delim]
	add_native_method("for", Method::CT_DYNAMIC, _for, 3, 3+1);
	// ^array.foreach[index;value]{code}[delim]
	add_native_method("foreach", Method::CT_DYNAMIC, _foreach, 3, 3+1);

	// ^array.sort[index;value]{string-key-maker}[[asc|desc]]
	// ^array.sort[index;value](numeric-key-maker)[[asc|desc]]
	add_native_method("sort", Method::CT_DYNAMIC, _sort, 3, 4);

	// ^array.select[index;value](bool-condition)[options hash]
	add_native_method("select", Method::CT_DYNAMIC, _select, 3, 4);

	// ^array.reverse[]
	add_native_method("reverse", Method::CT_DYNAMIC, _reverse, 0, 0);

	// ^array._at[first|last[;'key'|'value'|'hash']]
	// ^array._at([-+]offset)[['key'|'value'|'hash']]
	add_native_method("_at", Method::CT_DYNAMIC, _at, 1, 2);

#ifdef FEATURE_GET_ELEMENT4CALL
	// aliases without "_"
	add_native_method("keys", Method::CT_DYNAMIC, _keys, 0, 1);
	add_native_method("count", Method::CT_DYNAMIC, _count, 0, 1);
	add_native_method("at", Method::CT_DYNAMIC, _at, 1, 2);
#endif

}
