/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.C,v 1.12 2001/03/12 12:00:06 paf Exp $
*/

#include <stdlib.h>

#include "pa_table.h"
#include "pa_pool.h"

Table::Table(Pool& apool, 
			 char *afile, uint aline, 
			 Array *acolumns, 
			 int initial_rows) :
	Array(apool, initial_rows),
	fcurrent(0),
	fcolumns(acolumns), 
	name2number(pool(), false) {
#ifndef NO_STRING_ORIGIN
	forigin.file=afile;
	forigin.line=aline;
#endif

	if(fcolumns)
		for(int i=0; i<fcolumns->size(); i++) {
			const String& name=*fcolumns->get_string(i);
			name2number.put(name, i+1);
		}
}

const Array &Table::at(int index) {
	if(index<0 || index>=size())
		THROW(0, 0, 
			0,
			"table row index %d is out of range [0..%d]", 
			index, size()-1);
	
	return *static_cast<const Array *>(get(index));
}

const String *Table::item(const String& column_name) {
	int column_index;
	if(fcolumns) { // named
		int found_index=name2number.get_int(column_name);
		if(found_index)
			column_index=found_index-1;
		else
			THROW(0, 0,
				&column_name, 
				"column not found");
	} else { // nameless
		column_index=atoi(column_name.cstr());
		const Array& row=at(fcurrent);
		if(column_index<0 || column_index>=row.size()) // read past proper index?
			return 0; // it's OK, just return nothing
	}

	return item(column_index);
}
