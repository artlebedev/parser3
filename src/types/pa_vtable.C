/** @file
	Parser: @b table class.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VTABLE_C="$Date: 2009/04/15 04:49:50 $";

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
static void store_column_item_to_hash(const String* column_name, 
				      Record_info *info) {
	Value* value;
	if(const String* column_item=info->table->item(*column_name))
		value=new VString(*column_item);
	else
		value=VVoid::get();
	info->hash->put(*column_name, value);
}
Value* VTable::fields_element() {
	Table& ltable=table();
	if(Table::columns_type columns=ltable.columns()) {
		Value& result=*new VHash;
		Record_info record_info={&ltable, result.get_hash()};
		columns->for_each(store_column_item_to_hash, &record_info);
		return &result;
	}
	return 0;
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
