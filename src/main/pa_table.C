/** @file
	Parser: table class.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_TABLE_C="$Date: 2003/04/11 15:00:05 $";

//#include <stdlib.h>

#include "pa_table.h"
#include "pa_pool.h"
#include "pa_exception.h"

Table::Table(Pool& apool, 
			 const String *aorigin_string,
			 const Array *acolumns, 
			 int initial_rows) :
	Array(apool, initial_rows),

	forigin_string(aorigin_string),
	fcurrent(0),
	fcolumns(acolumns), 
	name2number(*NEW Hash(pool())) {

	if(fcolumns)
		for(int i=0; i<fcolumns->size(); i++) {
			const String& name=*fcolumns->get_string(i);
			name2number.put(name, i+1);
		}
}

Table::Table(Pool& apool, const Table& source, Action_options& options) :
	Array(apool, options.limit/*may be more than needed, no harm done*/),

	forigin_string(source.forigin_string),
	fcurrent(0),
	fcolumns(source.fcolumns),
	name2number(source.name2number) {

	append_array(source, options.offset, options.limit, options.reverse);
}

int Table::column_name2index(const String& column_name, bool bark) const {
	if(fcolumns) {// named
		int result=name2number.get_int(column_name)-1; // -1 = column not found
		if(bark && result<0)
			throw Exception("parser.runtime",
				&column_name,
				"column not found");
		return result;
	} else { // nameless
		char *error_pos;
		const char *cstr=column_name.cstr();
		int result=(int)strtol(cstr, &error_pos, 0);
		if(*error_pos/*not EOS*/) {
			result=-1;
			if(bark)
				throw Exception("parser.runtime",
					&column_name,
					"invalid column number");
		}
		return result;
	}
}

const String *Table::item(int column) const {
	if(valid(fcurrent)) {
		const Array& row=at(fcurrent);
		if(column>=0 && column<row.size()) // proper index?
			return row.get_string(column);
	}
	return 0; // it's OK we don't have row|column, just return nothing
}

void Table::set_current(int acurrent) { 
	if(acurrent<0 || acurrent>size())
		throw Exception(0,
			0,
			"table row (%d) out of range [0..%d]", acurrent, size()-1);
	fcurrent=acurrent; 
}
bool Table::locate(Table::locate_func func, void *info, Table::Action_options& o) {
	int size=this->size();
	if(!size || !o.limit)
		return false;
	if(o.limit<0)
		o.limit=size;
	int row=o.offset;

	int saved_current=current();
	if(o.reverse) { // reverse
		int to=max(0, row-o.limit);
		for(; row>=to; --row) {
			set_current(row);

			if(func(*this, info))
				return true;
		}
	} else { // forward
		int to=min(row+o.limit, size);
		for(; row<to; row++) {
			set_current(row);

			if(func(*this, info))
				return true;
		}
	}
	set_current(saved_current);

	return false;
}

#ifndef DOXYGEN
struct Locate_int_string_info {
	int column;
	const String* value;
};
#endif
bool locate_int_string(Table& self, void* ainfo) {
	Locate_int_string_info& info=*static_cast<Locate_int_string_info*>(ainfo);

	const String *item_value=self.item(info.column);
	return item_value && *item_value==*info.value;
}

bool Table::locate(int column, const String& value,
		   Table::Action_options& options) {
	Locate_int_string_info info={column, &value};
	return locate(locate_int_string, &info, options);
}

bool Table::locate(const String& column, const String& value, 
		   Table::Action_options& options) {
	return locate(column_name2index(column, true), value, options);
}

void Table::offset(bool absolute, int offset) {
	if(size())
		fcurrent=((absolute?0:fcurrent)+offset+size())%size();
}
