/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.h,v 1.11 2001/03/12 12:00:05 paf Exp $
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
		char *afile, uint aline,
		Array *acolumns,
		int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	// the base origin of table's data
	//const Origin& origin() { return forigin; }

	const Array *columns() { return fcolumns; }

	void set_current(int acurrent) { fcurrent=acurrent; }
	int get_current() { return fcurrent; }
	void inc_current() { fcurrent++; }

	const String *item(const String& column_name);

protected:

	// the base origin of table's data
	Origin forigin;

	// column name->number lookup table
	Hash name2number;

	// current row
	int fcurrent;

	// columns
	Array *fcolumns;

	const Array& at(int index);

	const String *item(int column_index) {
		return at(fcurrent).get_string(column_index);
	}
};

#endif
