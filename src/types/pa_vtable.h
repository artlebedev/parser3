/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vtable.h,v 1.3 2001/03/12 21:54:21 paf Exp $
*/

#ifndef PA_VTABLE_H
#define PA_VTABLE_H

#include "pa_vstateless_object.h"
#include "pa_table.h"
#include "_table.h"
#include "pa_vunknown.h"

class VTable : public VStateless_object {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "table"; }
	// table: fvalue
//	const Table *get_table() { return &fvalue; };
	// table: empty or not
	bool get_bool() { return table().size()!=0; }
	// table: itself
	VTable *get_vtable() { return this; }
	// table: column
	Value *get_element(const String& name) {
		// methods
		if(Value *result=VStateless_object::get_element(name))
			return result;

		if(fvalue)
			if(const String *string=fvalue->item(name))
				return NEW VString(*string);

		return NEW VUnknown(pool());
	}

public: // usage

	VTable(Pool& apool) : VStateless_object(apool, *table_class), 
		fvalue(0) {
	}
/*
	VTable(const Table& avalue) : VStateless_object(avalue.pool(), *table_class),
		fvalue(avalue) {
	}
*/

	void set_table(Table& avalue) { fvalue=&avalue; }
	Table& table() { 
		if(!fvalue)
			bark("getting unset vtable value");

		return *fvalue; 
	}

private:

	Table *fvalue;

};

#endif
