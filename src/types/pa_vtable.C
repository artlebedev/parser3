/** @file
	Parser: @b table class.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VTABLE_C="$Date: 2009/07/14 11:17:53 $";

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
	Value* value;
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

Value* VTable::get_element(const String& aname, Value& aself, bool looking_up) {
	// fields
	if(aname==TABLE_FIELDS_ELEMENT_NAME)
		return fields_element();

	// methods
	if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
		return result;

	// columns
	if(ftable) {
		int index=ftable->column_name2index(aname, false);
		if(index>=0) // column aname|number valid
			if(const String* string=ftable->item(index)) // there is such column
				return new VString(*string);
			else
				return VVoid::get();
	}

	throw Exception(PARSER_RUNTIME,
		&aname, 
		"column not found");
}
