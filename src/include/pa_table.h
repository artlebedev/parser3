/** @file
	Parser: table class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.h,v 1.25 2001/03/28 09:01:21 paf Exp $
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
	int current() { return fcurrent; }
	void shift(int offset);

	/// @return item from @a column_name
	const String *item(const String& column) const { 
		return item(column_name2index(column)); 
	}

	/// saves to text file
	void save(bool nameless_save, const String& file_spec);

	bool locate(const String& column, const String& value);

private:
	
	// where this table came from, may be NULL
	const String *forigin_string;

	// column name->number lookup table
	Hash name2number;

	// current row
	int fcurrent;

	// columns
	Array *fcolumns;

	bool valid(int index) const { return index>=0 && index<size(); }

	const Array& at(int index) const {
		// force @c const result
		return *const_cast<const Array *>(static_cast<Array *>(get(index)));
	}

	/// @return column index from @a column_name
	int column_name2index(const String& column) const;

	const String *item(int column) const;

};

#endif
