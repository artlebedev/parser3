/** @file
	Parser: table class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.h,v 1.21 2001/03/25 09:10:29 paf Exp $
*/

#ifndef PA_TABLE_H
#define PA_TABLE_H

#include "pa_types.h"
#include "pa_array.h"
#include "pa_hash.h"
#include "pa_string.h"
#include "pa_request.h"

/** 
	VTable backend.

	holds:
	- column names[if any]
	- data rows
	- current row pointer

	uses String for column names and data items

	hence most of tables are "named", no need to uptimize nameless onces
*/
class Table : public Array {
public:

	Table(Pool& apool,
		const String *aorigin,
		Array *acolumns,
		int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	/// where this table came from, may be NULL
	const String *origin_string() { return forigin_string; }

	/// column names
	const Array *columns() { return fcolumns; }

	/// moves @a current pointer
	void set_current(int acurrent) { fcurrent=acurrent; }
	/// @return current pointer
	int get_current() { return fcurrent; }

	/// @return item from @a column_name
	const String *item(const String& column_name);

private:
	
	// where this table came from, may be NULL
	const String *forigin_string;

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
