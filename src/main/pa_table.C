/** @file
	Parser: table class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.C,v 1.28 2001/05/07 15:31:46 paf Exp $
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
	if(fcolumns) // named
		return name2number.get_int(column_name)-1; // -1 = column not found
	else { // nameless
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

bool Table::locate(const String& column, const String& value) {
	int index=column_name2index(column);
	if(index<0)
		THROW(0, 0,
			&column,
			"column not found (locate)");
	return locate(index, value);
}

void Table::shift(int offset) {
	if(size())
		fcurrent=(fcurrent+offset+size())%size();
}
