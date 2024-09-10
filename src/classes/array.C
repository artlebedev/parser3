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

volatile const char * IDENT_ARRAY_C="$Id: array.C,v 1.1 2024/09/10 19:15:48 moko Exp $";

// class

class MArray: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VArray; }

public:
	MArray();
};

// global variable

DECLARE_CLASS_VAR(array, new MArray);

// methods

static void _create_or_add(Request& r, MethodParams& params) {
	if(params.count()) {
		Value& vsrc=params.as_no_junction(0, PARAM_MUST_BE_HASH);
		VArray& self=GET_SELF(r, VArray);
		ArrayValue& self_array=self.array();

		if(VArray* src=static_cast<VArray*>(vsrc.as(VARRAY_TYPE))) {
			ArrayValue& src_array =src->array();
			if(&src_array==&self_array) // same: doing nothing
				return;
		} else {
			HashStringValue* src_hash=vsrc.get_hash();
			if(src_hash)
				for(HashStringValue::Iterator i(*src_hash); i; i.next())
					self_array+=i.value();
		}
		self.clear_hash();
	}
}

static void _sql(Request& r, MethodParams& params) {}

static void _sub(Request& r, MethodParams& params) {}

static void _union(Request& r, MethodParams& params) {}

static void _intersection(Request& r, MethodParams& params) {}

static void _intersects(Request& r, MethodParams& params) {}

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

static void _count(Request& r, MethodParams&) {
	r.write(*new VInt(GET_SELF(r, VArray).count()));
}

static void _delete(Request& r, MethodParams& params) {
	if(params.count()>0)
		GET_SELF(r, VArray).clear(VArray::index(params.as_int(0, "index must be integer", r)));
	else
		GET_SELF(r, VArray).clear();
}

static void _contains(Request& r, MethodParams& params) {
	VArray& self=GET_SELF(r, VArray);
	bool result=self.contains(VArray::index(params.as_int(0, "index must be integer", r)));
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


enum AtResultType {
	AtResultTypeValue = 0,
	AtResultTypeKey = 1,
	AtResultTypeHash = 2
};

inline Value& SingleElementHash(String::Body akey, Value* avalue) {
	Value& result=*new VHash;
	result.put_element(*new String(akey, String::L_TAINTED), avalue);
	return result;
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
	int count=self.count(); // not array.count()

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

static void _at(Request& r, MethodParams& params) {
	VArray& self=GET_SELF(r, VArray);
	ArrayValue& array=self.array();
	size_t count=self.count();

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
				Value *value=i.prev(); // here for correct i.key()
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
		} else {
			for(ArrayValue::Iterator i(source_array); i; i.next() ){
				Value *value=i.value();
				if(value){
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
	VArray& self=GET_SELF(r, VArray);
	ArrayValue& source_array=self.array();
	size_t count=source_array.count();

	VArray& result=*new VArray(count);
	ArrayValue& result_array=result.array();

	for(ArrayValue::Iterator i(source_array); i; ){
		Value *v=i.next();
		if(v){
			result_array.fit(count-i.index() /* no -1 as .next() is called */, v);
		}
	}

	r.write(result);
}


static void _rename(Request& r, MethodParams& params) {}

// constructor

MArray::MArray(): Methoded(VARRAY_TYPE) {
//	set_base(hash_class);

	// ^array::create[[copy_from]]
	add_native_method("create", Method::CT_DYNAMIC, _create_or_add, 0, 1);
	// ^array.add[add_from]
	add_native_method("add", Method::CT_DYNAMIC, _create_or_add, 1, 1);

	// ^array.sub[sub_from]
	add_native_method("sub", Method::CT_DYNAMIC, _sub, 1, 1);
	// ^a.union[b] = array
	add_native_method("union", Method::CT_DYNAMIC, _union, 1, 1);
	// ^a.intersection[b][options array] = array
	add_native_method("intersection", Method::CT_DYNAMIC, _intersection, 1, 2);
	// ^a.intersects[b] = bool
	add_native_method("intersects", Method::CT_DYNAMIC, _intersects, 1, 1);

	// ^a.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 0, 1);

	// ^a.contains[key]
	add_native_method("contains", Method::CT_DYNAMIC, _contains, 1, 1);
	// backward
	add_native_method("contain", Method::CT_DYNAMIC, _contains, 1, 1);

	// ^array::sql[query][options array]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// ^array._keys[[column name]]
	add_native_method("_keys", Method::CT_DYNAMIC, _keys, 0, 1);

	// ^array._count[]
	add_native_method("_count", Method::CT_DYNAMIC, _count, 0, 0);

	// ^array.foreach[key;value]{code}[delim]
	add_native_method("foreach", Method::CT_DYNAMIC, _foreach, 2+1, 2+1+1);

	// ^array.sort[key;value]{string-key-maker}[[asc|desc]]
	// ^array.sort[key;value](numeric-key-maker)[[asc|desc]]
	add_native_method("sort", Method::CT_DYNAMIC, _sort, 3, 4);

	// ^array.select[key;value](bool-condition)[options array]
	add_native_method("select", Method::CT_DYNAMIC, _select, 3, 4);

	// ^array.reverse[]
	add_native_method("reverse", Method::CT_DYNAMIC, _reverse, 0, 0);

	// ^array._at[first|last[;'key'|'value'|'array']]
	// ^array._at([-+]offset)[['key'|'value'|'array']]
	add_native_method("_at", Method::CT_DYNAMIC, _at, 1, 2);

	// ^array.rename[from;to]
	// ^array.rename[ $.from[to] ... ]
	add_native_method("rename", Method::CT_DYNAMIC, _rename, 1, 2);

#ifdef FEATURE_GET_ELEMENT4CALL
	// aliases without "_"
	add_native_method("keys", Method::CT_DYNAMIC, _keys, 0, 1);
	add_native_method("count", Method::CT_DYNAMIC, _count, 0, 0);
	add_native_method("at", Method::CT_DYNAMIC, _at, 1, 2);
#endif

}
