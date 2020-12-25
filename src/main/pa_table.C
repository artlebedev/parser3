/** @file
	Parser: table class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_table.h"

volatile const char * IDENT_PA_TABLE_C="$Id: pa_table.C,v 1.72 2020/12/25 22:05:31 moko Exp $" IDENT_PA_TABLE_H;

#include "pa_exception.h"

Table::Table(columns_type acolumns, size_t initial_rows): Array<element_type>(initial_rows), fcurrent(0), fcolumns(acolumns), name2number(NULL){
	column_names_init();
}

void Table::column_names_init(){
	if(fcolumns){
		name2number = new name2number_hash_class;
		size_t number=1;
		for(Array_iterator<const String*> i(*fcolumns); i.has_next(); ) {
			const String& name=*i.next();
			name2number->put(name, number++);
		}
	}
}

static void append_row(Table& src, Table* dest) {
	Table::element_type src_row(src[src.current()]);
	Table::element_type row(new ArrayString(src_row->count()));
	row->append(*src_row);
	*dest+=row;
}

Table::Table(const Table& src, Action_options& options) :
	Array<element_type>( (options.limit==ARRAY_OPTION_LIMIT_ALL || options.limit>src.count()) ?  src.count() : options.limit),

	fcurrent(0),
	fcolumns(src.fcolumns),
	name2number(src.name2number) {

	((Table &)src).table_for_each(append_row, this, options);
}

size_t Table::max_cells() const {
	size_t result=0;
	for(size_t i=0; i<count(); i++){
		element_type row=get(i);
		if(row->count()>result)
			result=row->count();
	}
	return result;
}

int Table::column_name2index(const String& column_name, bool bark) const {
	if(fcolumns) {// named
		int result=name2number->get(column_name)-1; // -1 = column not found
		if(bark && result<0)
			throw Exception(PARSER_RUNTIME, &column_name, "column not found");
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

void Table::put_item(size_t column, const String* value) {
	if(!valid(fcurrent)) {
		throw Exception(PARSER_RUNTIME, 0, "invalid current row");
	}
	element_type row=get(fcurrent);
	while(row->count()<=column){
	    *row+=&String::Empty;
	}
	row->put(column, value);
}

void Table::remove_current(){
	if(!valid(fcurrent)) {
		throw Exception(PARSER_RUNTIME, 0, "invalid current row");
	}
	remove(fcurrent);
	if(fcurrent==count() && count()>0){
	    fcurrent--;
	}
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

bool Table::locate(int column, const String& value, Table::Action_options& options) {
	Locate_int_string_info info={column, &value};
	return table_first_that(locate_int_string, &info, options);
}

bool Table::locate(const String& column, const String& value, Table::Action_options& options) {
	return locate(column_name2index(column, true), value, options);
}

void Table::offset(bool absolute, int offset) {
	// not +lcount, but "if either operand is unsigned, the other shall be converted to unsigned" C++ rule works here
	if(size_t lcount=count())
		fcurrent=((absolute?0:fcurrent)+offset+lcount)%lcount;
}
