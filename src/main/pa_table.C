/** @file
	Parser: table class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.C,v 1.19 2001/03/26 10:36:56 paf Exp $
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

const Array &Table::at(int index) {
	return *const_cast<const Array *>(static_cast<Array *>(get(index)));
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
		if(!valid(fcurrent))
			return 0; // it's OK we don't have row, just return nothing
		const Array& row=at(fcurrent);
		if(column_index<0 || column_index>=row.size()) // read past proper index?
			return 0; // it's OK we don't have column, just return nothing
	}

	return item(column_index);
}

void Table::save(bool nameless_save, const String& file_spec) {
	String sdata(pool());
	if(!nameless_save) { // not nameless=named output
		// write out names line
		if(fcolumns) { // named table
			for(int column=0; column<fcolumns->size(); column++) {
				if(column)
					sdata.APPEND_CONST("\t");
				sdata.append(*static_cast<String *>(fcolumns->quick_get(column)), 
					String::UL_TABLE);
			}
		} else { // nameless table
			int lsize=size()?static_cast<Array *>(get(0))->size():0;
			if(lsize)
				for(int column=0; column<lsize; column++) {
					char *cindex_tab=(char *)malloc(MAX_NUMBER);
					snprintf(cindex_tab, MAX_NUMBER, "%d\t", column);
					sdata.APPEND_CONST(cindex_tab);
				}
			else
				sdata.APPEND_CONST("empty nameless table");
		}
		sdata.APPEND_CONST("\n");
	}
	// data lines
	for(int index=0; index<size(); index++) {
		Array *row=static_cast<Array *>(quick_get(index));
		for(int column=0; column<row->size(); column++) {
			if(column)
				sdata.APPEND_CONST("\t");
			sdata.append(*static_cast<String *>(row->quick_get(column)), 
				String::UL_TABLE);
		}
		sdata.APPEND_CONST("\n");
	}

	// write
	char *cdata=sdata.cstr();
	file_write(pool(), file_spec, cdata, strlen(cdata), true);
}