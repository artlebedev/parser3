/** @file
	Parser: table parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vtable.h,v 1.7 2001/03/26 08:27:28 paf Exp $
*/

#ifndef PA_VTABLE_H
#define PA_VTABLE_H

#include "pa_vstateless_object.h"
#include "pa_table.h"
#include "_table.h"
#include "pa_vunknown.h"

/// value of type 'table'. implemented with Table
class VTable : public VStateless_object {
public: // Value

	/// all: for error reporting after fail(), etc
	const char *type() const { return "table"; }
	/// extract VTable
	VTable *get_vtable() { return this; }
	/// VTable: column
	Value *get_element(const String& name) {
		// methods
		if(Value *result=VStateless_object::get_element(name))
			return result;

		if(ftable)
			if(const String *string=ftable->item(name))
				return NEW VString(*string);

		return NEW VUnknown(pool());
	}

protected: // VAliased

	VStateless_class *get_class_alias() { return 0; }

public: // usage

	VTable(Pool& apool) : VStateless_object(apool, *table_class), 
		ftable(0) {
	}
	void set_table(Table& avalue) { ftable=&avalue; }
	Table& table() { 
		if(!ftable)
			bark("getting unset vtable value");

		return *ftable; 
	}

private:

	Table *ftable;

};

#endif
