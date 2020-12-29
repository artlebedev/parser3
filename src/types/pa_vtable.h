/** @file
	Parser: @b table parser class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VTABLE_H
#define PA_VTABLE_H

#define IDENT_PA_VTABLE_H "$Id: pa_vtable.h,v 1.69 2020/12/29 09:21:16 moko Exp $"

#include "pa_vstateless_object.h"
#include "pa_table.h"
//#include "pa_vvoid.h"
#include "pa_vint.h"

// defines

#define VTABLE_TYPE "table"

// externs

extern Methoded* table_class;

/// value of type 'table'. implemented with Table
class VTable: public VStateless_object {
public: // Value

	override const char* type() const { return VTABLE_TYPE; }
	override VStateless_class *get_class() { return table_class; }
	/// VTable: count
	override int as_int() const { return (int)table().count(); }
	/// VTable: count
	override double as_double() const { return (double)table().count(); }
	/// VTable: count!=0
	override bool is_defined() const { return table().count()!=0; }
	/// VTable: 0 or !0
	override bool as_bool() const { return table().count()!=0; }
	/// VTable: count
	override Value& as_expr_result() { return *new VInt(as_int()); }
	/// VTable: json-string
	override const String* get_json_string(Json_options& options);
	/// extract VTable
	override Table* get_table() { return ftable; }
	/// VTable: columns,methods
	override Value* get_element(const String& aname);
#ifdef FEATURE_GET_ELEMENT4CALL
	/// VTable: methods,columns
	override Value* get_element4call(const String& aname);
#endif
	/// VTable: columns
	virtual const VJunction* put_element(const String&, Value*);

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
