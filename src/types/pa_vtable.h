/** @file
	Parser: @b table parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vtable.h,v 1.28 2001/08/03 11:54:19 parser Exp $
*/

#ifndef PA_VTABLE_H
#define PA_VTABLE_H

#include "pa_vstateless_object.h"
#include "pa_table.h"
#include "pa_vvoid.h"

#define TABLE_FIELDS_ELEMENT_NAME "fields"

extern Methoded *table_class;

/// value of type 'table'. implemented with Table
class VTable : public VStateless_object {
public: // Value

	const char *type() const { return "table"; }
	/// extract VTable
	Table *get_table() { return ftable; }
	/// VTable: columns,methods
	Value *get_element(const String& name);

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VTable(Pool& apool, Table* atable=0) : VStateless_object(apool, *table_class), 
		ftable(atable) {
	}
	void set_table(Table& avalue) { 
		ftable=&avalue; 
	}
	Table& table() { 
		if(!ftable)
			bark("getting unset vtable value");

		return *ftable; 
	}

private:

	Value *fields_element();

private:

	Table *ftable;

};

#endif
