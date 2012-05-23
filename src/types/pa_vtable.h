/** @file
	Parser: @b table parser class decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VTABLE_H
#define PA_VTABLE_H

#define IDENT_PA_VTABLE_H "$Id: pa_vtable.h,v 1.59 2012/05/23 16:26:41 moko Exp $"

#include "pa_vstateless_object.h"
#include "pa_table.h"
//#include "pa_vvoid.h"
#include "pa_vint.h"

// defines

#define VTABLE_TYPE "table"
#define TABLE_FIELDS_ELEMENT_NAME "fields"

// externs

extern Methoded* table_class;

/// value of type 'table'. implemented with Table
class VTable: public VStateless_object {
public: // Value

	override const char* type() const { return VTABLE_TYPE; }
	override VStateless_class *get_class() { return table_class; }
	/// VTable: count
	override int as_int() const { return table().count(); }
	/// VTable: count
	override double as_double() const { return as_int(); }
	/// VTable: count!=0
	override bool is_defined() const { return as_bool(); }
	/// VTable: 0 or !0
	override bool as_bool() const { return as_int()!=0; }
	/// VTable: count
	override Value& as_expr_result() { return *new VInt(as_int()); }
	/// VTable: json-string
	override const String* get_json_string(Json_options* options);
	/// extract VTable
	override Table* get_table() { return ftable; }
	/// VTable: columns,methods
	override Value* get_element(const String& aname);

public: // usage

	VTable(Table* atable=0): ftable(atable) {}

	void set_table(Table& avalue) { 
		ftable=&avalue; 
	}
	Table& table() const { 
		if(!ftable)
			bark("getting unset vtable value", 0);

		return *ftable; 
	}

private:

	Value* fields_element();

	String& get_json_string_array(String&, const char *);
	String& get_json_string_object(String&, const char *);
	String& get_json_string_compact(String&, const char *);

private:

	Table* ftable;

};

#endif
