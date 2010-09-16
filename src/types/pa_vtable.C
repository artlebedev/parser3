/** @file
	Parser: @b table class.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VTABLE_C="$Date: 2010/09/16 23:33:52 $";

#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"

#ifndef DOXYGEN
struct Record_info {
	Table* table;
	HashStringValue* hash;
};
#endif

static void store_column_item_to_hash(const String* column_name, Record_info *info) {
	const String* column_item=info->table->item(*column_name);
	info->hash->put(*column_name, 
		(column_item && !column_item->is_empty())
			?new VString(*column_item)
			:new VString()
	);
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
	if(aname==TABLE_FIELDS_ELEMENT_NAME)
		return fields_element();

	// methods
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

	throw Exception(PARSER_RUNTIME,
		&aname, 
		"column not found");
}

const String* VTable::get_json_string(Json_options* options) {
	String& result = *new String("[");
	Table& ltable=table();
	if(options && options->table == Json_options::T_ARRAY){
		// [
		//		["c1",  "c2",  "c3"  ...] || null (for nameless),
		//		["v11", "v12", "v13" ...],
		//		["v21", "v22", "v23" ...],
		//		...
		// ]

		// columns
		if(ltable.columns()){
			// named
			result << "[\"";
			bool need_delim=false;
			for(Array_iterator<const String*> c(*ltable.columns()); c.has_next(); ) {
				if(need_delim)
					result << "\",\"";
				result.append(*c.next(), String::L_JSON, true/*forced lang*/);
				need_delim=true;
			}
			result << "\"]";
		} else // nameless
			result << "null";

		// data
		if(ltable.count()){
			result << ",";
			for(Array_iterator<ArrayString*> r(ltable); r.has_next(); ) {
				result << "[\"";
				bool need_delim=false;
				for(Array_iterator<const String*> c(*r.next()); c.has_next(); ) {
					if(need_delim)
						result << "\",\"";
					result.append(*c.next(), String::L_JSON, true/*forced lang*/);
					need_delim=true;
				}
				result << (r.has_next() ? "\"]," : "\"]");
			}
		}
	} else {
		// [
		//		{"c1":"v11", "c2":"v12", "c3":"v13"},
		//		{"c1":"v21", "c2":"v22", "c3":"v23"},
		//		...
		// ]
		ArrayString* columns=ltable.columns();
		size_t columns_count = (columns) ? columns->count() : 0;

		for(Array_iterator<ArrayString*> r(ltable); r.has_next(); ) {
			ArrayString* row=r.next();

			result << "{\"";
			for(size_t index=0; index<row->count(); index++){
				if(index)
					result << "\",\"";
				//result << ( index < columns_count ? String(*columns->get(index), String::L_JSON) : String(format(index, 0)) );
				result.append(index < columns_count ? *columns->get(index) : String(format(index, 0)), String::L_JSON, true/*forced lang*/);
				result << "\":\"";
				result.append(*row->get(index), String::L_JSON, true/*forced lang*/);
			}
			result << "\"}";

			if(r.has_next())
				result << ",";
		}
	}

	result << "]";
	return &result;
}