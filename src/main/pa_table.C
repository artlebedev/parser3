/*
  $Id: pa_table.C,v 1.3 2001/01/29 20:46:22 paf Exp $
*/

#include <stdlib.h>

#include "pa_table.h"
#include "pa_pool.h"

Table::Table(Request& arequest, 
			 char *afile, uint aline, 
			 Array *acolumns, 
			 int initial_rows) :
	Array(arequest.pool, initial_rows),
	request(arequest),
	current(0),
	columns(acolumns), 
	name2number(arequest.pool, false) {
#ifndef NO_STRING_ORIGIN
	origin.file=afile;
	origin.line=aline;
#endif

	if(columns)
		for(int i=0; i<columns->size(); i++) {
			String name(pool);
			name.APPEND(columns->get_cstr(i), 0, 0);
			name2number.put(name, i+1);
		}
}

const Array *Table::at(int index) {
	if(index<0 || index>=size())
		request.error.raise(0, 
			"table column index %d is out of range [0..%d]", 
			index, size()-1);
	
	return static_cast<const Array *>(get(index));
}

const char *Table::item(int index) {
	const Array *row=at(current);
	return row->get_cstr(index);
}

const char *Table::item(String column_name) {
	int column_index;
	if(columns) {
		int found_index=name2number.get_int(column_name);
		if(found_index)
			column_index=found_index-1;
		else
			request.error.raise(&column_name, "column not found");
	} else {
		column_index=atoi(column_name.cstr());
		const Array *row=at(current);
		if(column_index<0 || column_index>=row->size())
			request.error.raise(&column_name, 
				"table column index %d is out of range [0..%d]", 
				column_index, row->size()-1);
	}

	return item(column_index);
}
