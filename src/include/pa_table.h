/** @file
	Parser: table class decl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_TABLE_H
#define PA_TABLE_H

static const char* IDENT_TABLE_H="$Date: 2003/04/11 15:00:05 $";

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
	struct Action_options {
	     int offset;
	     int limit; //< negative limit means 'all'. zero limit means 'nothing'
	     bool reverse;
	     bool defined;

	     Action_options(): offset(0), limit(-1), reverse(false), defined(false) {}
	};

	Table(Pool& apool,
		const String *aorigin,
		const Array *acolumns,
		int initial_rows=CR_INITIAL_ROWS_DEFAULT);
	Table(Pool& apool, const Table& source, Action_options& options);

	/// where this table came from, may be NULL
	const String *origin_string() { return forigin_string; }

	/// gets column names
	const Array *columns() { return fcolumns; }

	/// assignes column names
	void set_columns(const Array *acolumns) { fcolumns=acolumns; }

	/// moves @a current pointer
	void set_current(int acurrent);	/// @return current pointer
	int current() const { return fcurrent; }
	void offset(bool absolute, int offset);

	/** @return column index from @a column_name. '<0' if no such column
		if no such colum conditionally 'bark'
	*/
	int column_name2index(const String& column, bool bark) const;

	/// @return item from @a column
	const String *item(int column) const;

	/// @return item from @a column. '0' if no such column
	const String *item(const String& column) const {
		int index=column_name2index(column, false);
		return index>=0?item(index):0;
	}

	/// saves to text file
	void save(bool nameless_save, const String& file_spec);

	typedef bool (*locate_func)(Table& self, void* info);
	bool locate(locate_func func, void* info, Action_options& options);
	bool locate(int column, const String& value, Action_options& options);
	bool locate(const String& column, const String& value, Action_options& options);

	const Array& at(int index) const {
		// force @c const result
		return *const_cast<const Array *>(static_cast<Array *>(get(index)));
	}

private:
	
	// where this table came from, may be NULL
	const String *forigin_string;

	// column name->number lookup table
	Hash& name2number;

	// current row
	int fcurrent;

	// columns
	const Array *fcolumns;

	bool valid(int index) const { return index>=0 && index<size(); }

};

#endif
