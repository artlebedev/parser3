/** @file
	Parser: table class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.C,v 1.26 2001/04/05 16:30:42 paf Exp $
*/

#include <stdlib.h>

#include "pa_table.h"
#include "pa_pool.h"
#include "pa_exception.h"

Table::Table(Pool& apool, 
			 const String *aorigin_string,
			 Array *acolumns, 
			 int initial_rows) :
	Array(apool, initial_rows),

	forigin_string(aorigin_string),
	fcurrent(0),
	fcolumns(acolumns), 
	name2number(pool()) {

	if(fcolumns)
		for(int i=0; i<fcolumns->size(); i++) {
			const String& name=*fcolumns->get_string(i);
			name2number.put(name, i+1);
		}
}

int Table::column_name2index(const String& column_name) const {
	if(fcolumns) { // named
		int column_number=name2number.get_int(column_name);
		if(column_number)
			return column_number-1;
		else {
			THROW(0, 0,
				&column_name, 
				"column not found");
			return 0; // unreached
		}
	} else { // nameless
		char *error_pos=0;
		int result=(int)strtol(column_name.cstr(), &error_pos, 0);
		if(error_pos && *error_pos)
			THROW(0, 0,
				&column_name,
				"invalid column number");
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
	for(fcurrent=0; fcurrent<size(); fcurrent++) {
		const String *item_value=item(column);
		if(item_value && *item_value==value)
			return true;
	}

	fcurrent=0;
	return false;
}

void Table::shift(int offset) {
	if(size())
		fcurrent=(fcurrent+offset+size())%size();
}
