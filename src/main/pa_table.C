/** @file
	Parser: table class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.C,v 1.21 2001/03/28 08:01:42 paf Exp $
*/

#include <stdlib.h>

#include "pa_table.h"
#include "pa_pool.h"

Table::Table(Pool& apool, 
			 const String *aorigin_string,
			 Array *acolumns, 
			 int initial_rows) :
	Array(apool, initial_rows),

	forigin_string(aorigin_string),
	fcurrent(0),
	fcolumns(acolumns), 
	name2number(pool(), false) {

	if(fcolumns)
		for(int i=0; i<fcolumns->size(); i++) {
			const String& name=*fcolumns->get_string(i);
			name2number.put(name, i+1);
		}
}

int Table::column_name2index(const String& column_name) const {
	int result;
	if(fcolumns) { // named
		int column_index=name2number.get_int(column_name);
		if(column_index)
			result=column_index-1;
		else
			THROW(0, 0,
				&column_name, 
				"column not found");
	} else { // nameless
		result=atoi(column_name.cstr());
	}
	return result;
}

const String *Table::item(int column_index) const {
	if(valid(fcurrent)) {
		const Array& row=at(fcurrent);
		if(column_index>=0 && column_index<row.size()) // proper index?
			return row.get_string(column_index);
	}
	return 0; // it's OK we don't have row|column, just return nothing
}

bool Table::locate(const String& column, const String& value) const {
	int key_index=column_name2index(column);
	for(int fcurrent=0; fcurrent<size(); fcurrent++) {
		const String *item_value=item(key_index);
		if(item_value && *item_value==value)
			return true;
	}

	fcurrent=0;
	return false;
}
