/** @file
	Parser: @b table parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vtable.h,v 1.31 2001/09/24 14:34:25 parser Exp $
*/

#ifndef PA_VTABLE_H
#define PA_VTABLE_H

#include "pa_vstateless_object.h"
#include "pa_table.h"
#include "pa_vvoid.h"
#include "pa_vint.h"

#define TABLE_FIELDS_ELEMENT_NAME "fields"

extern Methoded *table_class;

/// value of type 'table'. implemented with Table
class VTable : public VStateless_object {
public: // Value

	const char *type() const { return "table"; }
	/// VTable: finteger
	int as_int() const { return table().size(); }
	/// VTable: finteger
	double as_double() const { return as_int(); }
	/// VTable: count!=0
	bool is_defined() const { return as_int()!=0; }
	/// VTable: 0 or !0
	bool as_bool() const { return as_int()!=0; }
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
	Table& table() const { 
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
