/** @file
	Parser: @b table parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VTABLE_H
#define PA_VTABLE_H

static const char* IDENT_VTABLE_H="$Date: 2002/10/31 15:01:57 $";

#include "pa_vstateless_object.h"
#include "pa_table.h"
#include "pa_vvoid.h"
#include "pa_vint.h"

// defines

#define VTABLE_TYPE "table"
#define TABLE_FIELDS_ELEMENT_NAME "fields"

// externs

extern Methoded *table_class;

/// value of type 'table'. implemented with Table
class VTable : public VStateless_object {
public: // Value

	const char *type() const { return VTABLE_TYPE; }
	VStateless_class *get_class() { return table_class; }
	/// VTable: count
	int as_int() const { return table(0).size(); }
	/// VTable: count
	double as_double() const { return as_int(); }
	/// VTable: count!=0
	bool is_defined() const { return as_int()!=0; }
	/// VTable: 0 or !0
	bool as_bool() const { return is_defined(); }
	/// VTable: count
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VInt(pool(), as_int()); }
	/// extract VTable
	Table *get_table() { return ftable; }
	/// VTable: columns,methods
	Value *get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: // usage

	VTable(Pool& apool, Table* atable=0) : VStateless_object(apool), 
		ftable(atable) {
	}
	void set_table(Table& avalue) { 
		ftable=&avalue; 
	}
	Table& table(const String *source) const { 
		if(!ftable)
			bark("getting unset vtable value", 0, source);

		return *ftable; 
	}

private:

	Value *fields_element();

private:

	Table *ftable;

};

#endif
