/** @file
	Parser: @b table parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vtable.h,v 1.20 2001/05/07 14:00:55 paf Exp $
*/

#ifndef PA_VTABLE_H
#define PA_VTABLE_H

#include "pa_vstateless_object.h"
#include "pa_table.h"
#include "pa_vunknown.h"

extern Methoded *table_class;

/// value of type 'table'. implemented with Table
class VTable : public VStateless_object {
public: // Value

	const char *type() const { return "table"; }
	/// extract VTable
	Table *get_table() { return ftable; }
	/// VTable: columns,methods
	Value *get_element(const String& name) {
		// columns
		if(ftable) {
			int index=ftable->column_name2index(name);
			if(index>=0) // column name|number valid
				if(const String *string=ftable->item(index)) // there is such column
					return NEW VString(*string);
		}

		// methods
		if(Value *result=VStateless_object::get_element(name))
			return result;

		THROW(0, 0,
			&name, 
			"column not found");
		return 0; //unreached
	}

public:

	bool last_locate_was_successful;

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VTable(Pool& apool, Table* atable=0) : VStateless_object(apool, *table_class), 
		ftable(atable), locked(false),
		last_locate_was_successful(false) {
	}
	void lock() { 
		check_lock();
		locked=true; 
	}
	void unlock() { locked=false; }
	void set_table(Table& avalue) { 
		check_lock();
		ftable=&avalue; 
	}
	Table& table() { 
		if(!ftable)
			bark("getting unset vtable value");

		return *ftable; 
	}

private:

	void check_lock() {
		if(locked)
			bark("is locked");
	}

private:

	Table *ftable;
	bool locked;

};

#endif
