/** @file
	Parser: @b table class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VTABLE_C="$Id: pa_vtable.C,v 1.14 2002/08/01 11:26:57 paf Exp $";

#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhash.h"

#ifndef DOXYGEN
struct Record_info {
	Pool *pool;
	Table *table;
	Hash *hash;
};
#endif
static void store_column_item_to_hash(Array::Item *item, void *info) {
	Record_info& ri=*static_cast<Record_info *>(info);
	String& column_name=*static_cast<String *>(item);
	Value *value;
	if(const String *column_item=ri.table->item(column_name))
		value=new(*ri.pool) VString(*column_item);
	else
		value=new(*ri.pool) VVoid(*ri.pool);
	ri.hash->put(column_name, value);
}
Value *VTable::fields_element() {
	if(const Array *columns=table().columns()) {
		Value *result=NEW VHash(pool());
		Record_info record_info={&pool(), &table(), result->get_hash(0)};
		columns->for_each(store_column_item_to_hash, &record_info);
		return result;
	}
	return 0;
}


Value *VTable::get_element(const String& name) {
	// fields
	if(name==TABLE_FIELDS_ELEMENT_NAME)
		return fields_element();

	// methods
	if(Value *result=VStateless_object::get_element(name))
		return result;

	// columns
	if(ftable) {
		int index=ftable->column_name2index(name, false);
		if(index>=0) // column name|number valid
			if(const String *string=ftable->item(index)) // there is such column
				return NEW VString(*string);
			else
				return NEW VVoid(pool());
	}

	throw Exception("parser.runtime",
		&name, 
		"column not found");
	return 0; //unreached
}
