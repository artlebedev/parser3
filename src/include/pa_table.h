/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.h,v 1.13 2001/03/12 20:55:15 paf Exp $
*/

/*
	hence most of tables are "named", no need to uptimize nameless onces
*/


#ifndef PA_TABLE_H
#define PA_TABLE_H

#include "pa_types.h"
#include "pa_array.h"
#include "pa_hash.h"
#include "pa_string.h"
#include "pa_request.h"

class Table : public Array {
public:

	Table(Pool& apool,
		const String& asource,
		Array *acolumns,
		int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	// the source of table's data
	const String& source() { return fsource; }

	const Array *columns() { return fcolumns; }

	void set_current(int acurrent) { fcurrent=acurrent; }
	int get_current() { return fcurrent; }

	const String *item(const String& column_name);

private:

	// the base origin of table's data
	const String& fsource;

	// column name->number lookup table
	Hash name2number;

	// current row
	int fcurrent;

	// columns
	Array *fcolumns;

	bool valid(int index) { return index>=0 && index<size();	}

	const Array& at(int index);

	const String *item(int column_index) {
		return valid(fcurrent)?at(fcurrent).get_string(column_index):0;
	}
};

#endif
