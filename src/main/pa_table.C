/*
  $Id: pa_table.C,v 1.7 2001/01/30 13:43:43 paf Exp $
*/

#include <stdlib.h>

#include "pa_table.h"
#include "pa_pool.h"

Table::Table(Request& arequest, 
			 char *afile, uint aline, 
			 Array *acolumns, 
			 int initial_rows) :
	Array(arequest.pool(), initial_rows),
	request(arequest),
	fcurrent(0),
	fcolumns(acolumns), 
	name2number(arequest.pool(), false) {
#ifndef NO_STRING_ORIGIN
	forigin.file=afile;
	forigin.line=aline;
#endif

	if(fcolumns)
		for(int i=0; i<fcolumns->size(); i++) {
			String name(pool);
			name.APPEND(fcolumns->get_cstr(i), 0, 0);
			name2number.put(name, i+1);
		}
}

const Array &Table::at(int index) {
	if(index<0 || index>=size())
		request.exception().raise(0, 0, 
			0,
			"table row index %d is out of range [0..%d]", 
			index, size()-1);
	
	return *static_cast<const Array *>(get(index));
}

const char *Table::item(int index) {
	const Array& row=at(fcurrent);
	return row.get_cstr(index);
}

const char *Table::item(const String& column_name) {
	int column_index;
	if(fcolumns) {
		int found_index=name2number.get_int(column_name);
		if(found_index)
			column_index=found_index-1;
		else
			request.exception().raise(0, 0,
				&column_name, 
				"column not found");
	} else {
		column_index=atoi(column_name.cstr());
		const Array& row=at(fcurrent);
		if(column_index<0 || column_index>=row.size())
			return 0;
		/*
			request.exception().raise(0, 0,
				&column_name, 
				"table column index %d is out of range [0..%d]", 
				column_index, row->size()-1);
				*/
	}

	return item(column_index);
}
