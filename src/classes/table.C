/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: table.C,v 1.5 2001/03/12 18:19:36 paf Exp $
*/

#include "pa_request.h"
#include "_table.h"
#include "pa_vtable.h"
#include "pa_common.h"

// global var

VClass *table_class;

// methods


// TODO: проверить ^set в ^menu & co

static void set_or_load(
						Request& r, 
						const String& method_name, Array *params, 
						bool is_load) {
	Pool& pool=r.pool();
	// data is last parameter
	Value *vdata=static_cast<Value *>(params->get(params->size()-1));
	// forcing
	//	[this param type] 
	//	[this param type] 
	r.fail_if_junction_(true, *vdata, 
		method_name, "body must not be a junction");

	// data or file_name
	char *data_or_filename=vdata->as_string().cstr();
	// data
	char *data=is_load?file_read(pool, r.absolute(data_or_filename)):data_or_filename;

	// parse columns
	Array *columns;
#ifndef NO_STRING_ORIGIN
	const Origin& origin=method_name.origin();
	const char *file=origin.file;
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


static void _set(Request& r, const String& method_name, Array *params) {
	set_or_load(r, method_name, params, false);
}

static void _load(Request& r, const String& method_name, Array *params) {
	set_or_load(r, method_name, params, true);
}

void initialize_table_class(Pool& pool, VClass& vclass) {
	// ^table.set[data]  ^table.set[nameless;data]
	vclass.add_native_method("set", _set, 1, 2);

	// ^table.load[file]  ^table.load[nameless;file]
	vclass.add_native_method("load", _load, 1, 2);
}	
