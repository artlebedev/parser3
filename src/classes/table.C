/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: table.C,v 1.12 2001/03/13 11:19:29 paf Exp $
*/

#include "pa_request.h"
#include "_table.h"
#include "pa_vtable.h"
#include "pa_common.h"
#include "pa_vint.h"

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

static void _count(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, r.self->as_vtable().table().size());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _line(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, 1+r.self->as_vtable().table().get_current());
	r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
}

static void _offset(Request& r, const String&, Array *params) {
	Pool& pool=r.pool();
	Table& table=r.self->as_vtable().table();
	if(params->size()) {
		if(int size=table.size()) {
			int offset=
				(int)r.process(*static_cast<Value *>(params->get(0))).get_double();
			table.set_current((table.get_current()+offset+size)%size);
		}
	} else {
		Value& value=*new(pool) VInt(pool, table.get_current());
		r.wcontext->write(value, String::Untaint_lang::NO /*always object, not string*/);
	}
}

static void _menu(Request& r, const String& method_name, Array *params) {
	Value& body_code=*static_cast<Value *>(params->get(0));
	// forcing ^menu{this param type}
	r.fail_if_junction_(false, body_code, 
		method_name, "body must be junction");
	
	Value *delim_code=params->size()==2?static_cast<Value *>(params->get(1)):0;

	Table& table=r.self->as_vtable().table();
	bool need_delim=false;
	for(int i=0; i<table.size(); i++) {
		table.set_current(i);

		Value& processed_body=r.process(body_code);
		if(delim_code) { // delimiter set?
			const String *string=processed_body.get_string();
			if(need_delim && string && string->size()) // need delim & iteration produced string?
				r.write_pass_lang(r.process(*delim_code));
			need_delim=true;
		}
		r.write_pass_lang(processed_body);
	}
}

static void _empty(Request& r, const String&, Array *params) {
	Table& table=r.self->as_vtable().table();
	if(table.size()==0) {
		Value& value=r.process(*static_cast<Value *>(params->get(0)));
		r.write_pass_lang(value);
	} else if(params->size()==2) {
		Value& value=r.process(*static_cast<Value *>(params->get(1)));
		r.write_pass_lang(value);
	}
}

void initialize_table_class(Pool& pool, VClass& vclass) {
	// ^table.set[data]  
	// ^table.set[nameless;data]
	vclass.add_native_method("set", _set, 1, 2);

	// ^table.load[file]  
	// ^table.load[nameless;file]
	vclass.add_native_method("load", _load, 1, 2);

	// ^table.count[]
	vclass.add_native_method("count", _count, 0, 0);

	// ^table.line[]
	vclass.add_native_method("line", _line, 0, 0);

	// ^table.offset[]  
	// ^table.offset[offset]
	vclass.add_native_method("offset", _offset, 0, 1);

	// ^table.menu{code}  
	// ^table.menu{code}[delim]
	vclass.add_native_method("menu", _menu, 1, 2);

	// ^table.empty{code-when-empty}  
	// ^table.empty{code-when-empty}{code-when-not}
	vclass.add_native_method("empty", _empty, 1, 2);

}	
