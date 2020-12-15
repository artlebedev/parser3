/** @file
	Parser: @b table class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VTABLE_C="$Id: pa_vtable.C,v 1.50 2020/12/15 17:10:44 moko Exp $" IDENT_PA_VTABLE_H;

#ifndef DOXYGEN
struct Record_info {
	Table* table;
	HashStringValue* hash;
};
#endif

static void store_column_item_to_hash(const String* column_name, Record_info *info) {
	const String* column_item=info->table->item(*column_name);
	info->hash->put(*column_name, (column_item && !column_item->is_empty()) ? new VString(*column_item) : new VString() );
}

Value* VTable::fields_element() {
	Value& result=*new VHash;
	Table& ltable=table();
	if(!ltable.count())
		return &result;

	HashStringValue* hash=result.get_hash();

	if(Table::columns_type columns=ltable.columns()) { // named
		Record_info record_info={&ltable, hash};
		columns->for_each(store_column_item_to_hash, &record_info);
	} else { // nameless
		size_t row_size=ltable[ltable.current()]->count(); // number of columns in current row
		for(size_t index=0; index<row_size; index++){
			const String* column_item=ltable.item(index);
			hash->put(String::Body::Format(index), 
				(column_item && !column_item->is_empty())
					?new VString(*column_item)
					:new VString()
			);
		}
	}

	return &result;
}

Value* VTable::get_element(const String& aname) {
	// fields
	if(SYMBOLS_EQ(aname,FIELDS_SYMBOL))
		return fields_element();

#ifdef FEATURE_GET_ELEMENT4CALL
	// columns first
	if(ftable) {
		int index=ftable->column_name2index(aname, false);
		if(index>=0) // column aname|number valid
		{
			const String* string=ftable->item(index); // there is such column
			return new VString(string ? *string : String::Empty);
		}
	}

#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	// CLASS, CLASS_NAME
	if(Value* result=VStateless_object::get_element(aname))
		return result;
#endif

	throw Exception(PARSER_RUNTIME, &aname, "column not found");
}

Value* VTable::get_element4call(const String& aname) {
	// methods
	return VStateless_object::get_element(aname);
}

#else
	// methods first
	if(Value* result=VStateless_object::get_element(aname))
		return result;

	// columns
	if(ftable) {
		int index=ftable->column_name2index(aname, false);
		if(index>=0) // column aname|number valid
		{
			const String* string=ftable->item(index); // there is such column
			return new VString(string ? *string : String::Empty);
		}
	}

	throw Exception(PARSER_RUNTIME, &aname, "column not found");
}
#endif

const VJunction* VTable::put_element(const String& aname, Value* avalue) {
	if(ftable) {
		int index=ftable->column_name2index(aname, false);
		if(index>=0) // column aname|number valid
		{
			if(index > pa_loop_limit)
				throw Exception(PARSER_RUNTIME, &aname, "too big column number");
			const String *svalue=avalue->get_string();
			if(!svalue)
				throw Exception(PARSER_RUNTIME, 0, "column value must be string compatible");
			ftable->put_item(index, svalue);
			return 0;
		}
	}

	throw Exception(PARSER_RUNTIME, &aname, "column not found");
}

String& VTable::get_json_string_array(String& result, const char *indent) {
	// [
	//		["c1",  "c2",  "c3"  ...] || null (for nameless),
	//		["v11", "v12", "v13" ...],
	//		["v21", "v22", "v23" ...],
	//		...
	// ]
	Table& ltable=table();

	// columns
	if(ltable.columns()){
		// named
		indent ? result << "\n\t" << indent << "[\"" : result << "\n[\"";

		bool need_delim=false;
		for(Array_iterator<const String*> c(*ltable.columns()); c.has_next(); ) {
			if(need_delim)
				result << "\",\"";
			result.append(*c.next(), String::L_JSON, true/*forced lang*/);
			need_delim=true;
		}
		result << "\"]";
	} else {
		// nameless
		indent ? result << "\n\t" << indent << "null" : result << "\nnull";
	}

	// data
	if(ltable.count()){
		result << ",";
		for(Array_iterator<ArrayString*> r(ltable); r.has_next(); ) {
			indent ? result << "\n\t" << indent << "[\"" : result << "\n[\"";
			bool need_delim=false;
			for(Array_iterator<const String*> c(*r.next()); c.has_next(); ) {
				if(need_delim)
					result << "\",\"";
				result.append(*c.next(), String::L_JSON, true/*forced lang*/);
				need_delim=true;
			}
			r.has_next() ? result << "\"]," : result << "\"]";
		}
	}

	result << "\n" << indent; 
	return result;
}

String& VTable::get_json_string_object(String& result, const char *indent) {
	// [
	//		{"c1":"v11", "c2":"v12", "c3":"v13"},
	//		{"c1":"v21", "c2":"v22", "c3":"v23"},
	//		...
	// ]
	Table& ltable=table();
	ArrayString* columns=ltable.columns();
	size_t columns_count = (columns) ? columns->count() : 0;

	for(Array_iterator<ArrayString*> r(ltable); r.has_next(); ) {
		indent ? result << "\n\t" << indent << "{\"" : result << "\n{\"";

		ArrayString* row=r.next();
		for(size_t index=0; index<row->count(); index++){
			if(index)
				result << "\",\"";
			result.append(index < columns_count ? *columns->get(index) : String(format(index, 0)), String::L_JSON, true/*forced lang*/);
			result << "\":\"";
			result.append(*row->get(index), String::L_JSON, true/*forced lang*/);
		}
		r.has_next() ? result << "\"}," : result << "\"}\n" << indent;
	}
	return result;
}

String& VTable::get_json_string_compact(String& result, const char *indent) {
	// [
	//		"v11",
	//		["v21", "v22", "v23" ...],
	//		...
	// ]
	Table& ltable=table();

	for(Array_iterator<ArrayString*> r(ltable); r.has_next(); ) {
		ArrayString& line=*r.next();
		if (line.count()==1){
			indent ? result << "\n\t" << indent << "\"" : result << "\n\"";

			result.append(*line[0], String::L_JSON, true/*forced lang*/);
			r.has_next() ? result << "\"," : result << "\"\n" << indent;
		} else {
			indent ? result << "\n\t" << indent << "[\"" : 	result << "\n[\"";

			bool need_delim=false;
			for(Array_iterator<const String*> c(line); c.has_next(); ) {
				if(need_delim)
					result << "\",\"";
				result.append(*c.next(), String::L_JSON, true/*forced lang*/);
				need_delim=true;
			}
			r.has_next() ? result << "\"]," : result  << "\"]\n" << indent;
		}
	}
	return result;
}

const String* VTable::get_json_string(Json_options& options) {
	String* result = new String("[", String::L_AS_IS);

	switch(options.table){
	case Json_options::T_ARRAY:
		result=&get_json_string_array(*result, options.indent);
		break;
	case Json_options::T_OBJECT:
		result=&get_json_string_object(*result, options.indent);
		break;
	case Json_options::T_COMPACT:
		result=&get_json_string_compact(*result, options.indent);
		break;
	}

	*result << "]";
	return result;
}
