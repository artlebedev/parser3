/** @file
	Parser: table class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_table.C,v 1.34 2001/07/02 13:24:32 parser Exp $"; 

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

int Table::column_name2index(const String& column_name, bool bark) const {
	if(fcolumns) {// named
		int result=name2number.get_int(column_name)-1; // -1 = column not found
		if(bark && result<0)
			THROW(0, 0,
				&column_name,
				"column not found");
		return result;
	} else { // nameless
		char *error_pos=0;
		int result=(int)strtol(column_name.cstr(), &error_pos, 0);
		if(error_pos && *error_pos) {
			result=-1; // calm, compiler
			if(bark)
				THROW(0, 0,
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

void Table::shift(int offset) {
	if(size())
		fcurrent=(fcurrent+offset+size())%size();
}
