/** @file
	Parser: table class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_table.h,v 1.30 2001/05/07 08:29:42 paf Exp $
*/

#ifndef PA_TABLE_H
#define PA_TABLE_H

#include "pa_types.h"
#include "pa_array.h"
#include "pa_hash.h"
#include "pa_string.h"

/** 
	VTable backend.

	holds:
	- column names[if any]
	- data rows
	- current row pointer

	uses String for column names and data items

	hence most of tables are "named", no need to uptimize nameless onces.
	rows and strings stored are read-only. once stored they can be removed,
	but not altered. that's handy for quick copying & co. see table:join
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

	/// @return column index from @a column_name. '<0' if no such column
	int column_name2index(const String& column) const;

	/// @return item from @a column
	const String *item(int column) const;

	/// @return item from @a column. '0' if no such column
	const String *item(const String& column) const {
		int index=column_name2index(column);
		return index>=0?item(index):0;
	}

	/// saves to text file
	void save(bool nameless_save, const String& file_spec);

	bool locate(int column, const String& value);
	bool locate(const String& column, const String& value) {
		return locate(column_name2index(column), value);
	}

	const Array& at(int index) const {
		// force @c const result
		return *const_cast<const Array *>(static_cast<Array *>(get(index)));
	}

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

};

#endif
