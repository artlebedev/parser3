/** @file
	Parser: table class decl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_TABLE_H
#define PA_TABLE_H

#define IDENT_PA_TABLE_H "$Id: pa_table.h,v 1.75 2024/11/16 02:57:05 moko Exp $"

#include "pa_types.h"
#include "pa_hash.h"
#include "pa_string.h"

class Temp_current;

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
class Table: public Array<ArrayString*> {
public:
	typedef ArrayString* columns_type;

	Table(columns_type acolumns, size_t initial_rows=3);
	Table(const Table& src, Action_options& options);

	/// gets column names
	columns_type columns() { return fcolumns; }

	/// moves @a current pointer, can be out of range when restoring current in modified table
	void set_current(size_t acurrent) {
		fcurrent=acurrent<count() ? acurrent : count()>0 ? count()-1 : 0;
	}

	/// @return current pointer
	size_t current() const { return fcurrent; }
	void offset(bool absolute, int offset);

	/// @return checks all rows to find maximum cells number
	size_t max_cells() const;

	/** @return column index from @a column_name. '<0' if no such column
		if no such - 'bark'
	*/
	int column_name2index(const String& column, bool bark) const;

	void column_names_init();

	/// @return item from @a column
	const String* item(size_t column);

	/// sets @a column value
	void put_item(size_t column, const String*);

	/// removes current row
	void remove_current();

	/// @return item from @a column. '0' if no such column
	const String* item(const String& column) {
		int index=column_name2index(column, false);
		return index>=0?item(index):0;
	}

	/// saves to text file
	void save(bool nameless_save, const String& file_spec);

	template<typename I>
	void table_for_each(void (*func)(Table& self, I* info), I* info, Action_options& o) {
		if(!o.adjust(count()))
			return;

		Temp_current tc(*this);
		size_t row=o.offset;
		if(o.reverse) { // reverse
			for(size_t i=0; i<o.limit; i++) {
				set_current(row-i);
				func(*this, info);
			}
		} else { // forward
			for(size_t to=row+o.limit; row<to; row++) {
				set_current(row);
				func(*this, info);
			}
		}
	}

	template<typename I>
	bool table_first_that(bool (*func)(Table& self, I info), I info, Action_options& o) {
		if(!o.adjust(count()))
			return false;

		Temp_current tc(*this);
		size_t row=o.offset;
		if(o.reverse) { // reverse
			for(size_t i=0; i<o.limit; i++) {
				set_current(row-i);

				if(func(*this, info))
					return true;
			}
		} else { // forward
			for(size_t to=row+o.limit; row<to; row++) {
				set_current(row);

				if(func(*this, info))
					return true;
			}
		}

		return false;
	}

	bool locate(int column, const String& value, Action_options& options);
	bool locate(const String& column, const String& value, Action_options& options);

private:
	
	/// current row
	size_t fcurrent;

	/// columns
	columns_type fcolumns;

	/// column name->number lookup table
	typedef HashString<int> name2number_hash_class;
	name2number_hash_class* name2number;

	/// is that @c index falid?
	bool valid(size_t index) const { return index<count(); }

};

/// Auto-object that temporarily saves and restores current
class Temp_current {
	Table& ftable;
	size_t fcurrent;
public:
	Temp_current(Table& atable) : ftable(atable), fcurrent(atable.current()){}
	~Temp_current(){
		ftable.set_current(fcurrent);
	}
};

#endif
