/** @file
	Parser: table class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_TABLE_C="$Date: 2002/08/06 14:23:23 $";

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

Table::Table(Pool& apool, const Table& source, int offset, int limit) :
	Array(apool, limit/*may be more than needed, no harm done*/),

	forigin_string(source.forigin_string),
	fcurrent(source.fcurrent),
	fcolumns(source.fcolumns),
	name2number(source.name2number) {

	append_array(source, offset, limit);
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

bool Table::locate(int column, const String& value) {
	int scurrent=fcurrent;
	for(fcurrent=0; fcurrent<size(); fcurrent++) {
		const String *item_value=item(column);
		if(item_value && *item_value==value)
			return true;
	}

	fcurrent=scurrent;
	return false;
}

bool Table::locate(const String& column, const String& value) {
	return locate(column_name2index(column, true), value);
}

void Table::offset(bool absolute, int offset) {
	if(size())
		fcurrent=((absolute?0:fcurrent)+offset+size())%size();
}
