/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: table.C,v 1.1 2001/03/12 13:40:01 paf Exp $
*/

#include "pa_request.h"
#include "_table.h"
#include "pa_vtable.h"

// global var

VClass *table_class;

// methods


// TODO: проверить ^set в ^menu & co

static void _set(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	// data is last parameter
	Value *vdata=static_cast<Value *>(params->get(params->size()-1));
	if(vdata->get_junction())
		R_THROW(0, 0,
			&method_name,
			"body must not be a junction");

	// data string
	char *data=vdata->as_string().cstr();

	// parse columns
	Array *columns;
#ifndef NO_STRING_ORIGIN
	const Origin& origin=method_name.origin();
	const char *file=0;//origin.file;
	uint line=origin.line;
#endif
	if(params->size()==2) {
		columns=0;
	} else {
		columns=new(pool) Array(pool);

		if(char *row_chars=getrow(&data)) 
			do {
				String *name=new(pool) String(pool);
				name->APPEND(lsplit(&row_chars, '\t'), 0, file, line++);
				*columns+=name;
			} while(row_chars);
	}

	// parse cells
	Table& table=*new(pool) Table(pool, method_name, columns);
	char *row_chars;
	while(row_chars=getrow(&data)) {
		Array *row=new(pool) Array(pool);
		while(char *cell_chars=lsplit(&row_chars, '\t')) {
			String *cell=new(pool) String(pool);
			cell->APPEND(cell_chars, 0, file, line);
			*row+=cell;
		}
		line++;
		table+=row;
	};

	// replace any previous table value
	r.self->as_vtable().set_table(table);
}

void initialize_table_class(Pool& pool, VClass& vclass) {
	// ^table.create[data]  ^table.create[nameless;data]
	vclass.add_native_method("set", _set, 1, 2);
}	
