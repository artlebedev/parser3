/** @file
	Parser: table class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_TABLE_C="$Date: 2004/02/11 15:33:16 $";

#include "pa_table.h"

#include "pa_exception.h"

Table::Table(columns_type acolumns, size_t initial_rows):
	Array<element_type>(initial_rows),

	fcurrent(0),
	fcolumns(acolumns), 
	name2number(new name2number_hash_class) {

	if(fcolumns) {
		size_t number=1;
		for(Array_iterator<const String*> i(*fcolumns); i.has_next(); ) {
			const String& name=*i.next();
			name2number->put(name, number++);
		}
	}
}

Table::Table(const Table& src, Action_options& options) :
	Array<element_type>(options.limit==ARRAY_OPTION_LIMIT_ALL?0:options.limit/*may be more than needed, no harm done*/),

	fcurrent(0),
	fcolumns(src.fcolumns),
	name2number(src.name2number) {

	append(src, options.offset, options.limit, options.reverse);
}

int Table::column_name2index(const String& column_name, bool bark) const {
	if(fcolumns) {// named
		int result=name2number->get(column_name)-1; // -1 = column not found
		if(bark && result<0)
			throw Exception("parser.runtime",
				&column_name,
				"column not found");
		return result;
	} else // nameless
		return column_name.as_int();
}

const String* Table::item(size_t column) {
	if(valid(fcurrent)) {
		element_type row=get(fcurrent);
		if(column<row->count()) // proper index?
			return row->get(column);
	}
	return 0; // it's OK we don't have row|column, just return nothing
}

#ifndef DOXYGEN
struct Locate_int_string_info {
	int column;
	const String* value;
};
#endif
bool locate_int_string(Table& self, Locate_int_string_info* info) {
	const String *item_value=self.item(info->column);
	return item_value && *item_value==*info->value;
}

bool Table::locate(int column, const String& value,
		   Table::Action_options& options) {
	Locate_int_string_info info={column, &value};
	return table_first_that(locate_int_string, &info, options);
}

bool Table::locate(const String& column, const String& value, 
		   Table::Action_options& options) {
	return locate(column_name2index(column, true), value, options);
}

void Table::offset(bool absolute, int offset) {
	if(size_t lcount=count())
		fcurrent=((absolute?0:fcurrent)+offset+lcount)%lcount;
}
