/** @file
	Parser: table parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: table.C,v 1.34 2001/03/27 16:35:52 paf Exp $
*/

#include "pa_config_includes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "_table.h"
#include "pa_vtable.h"
#include "pa_vint.h"

// global var

VStateless_class *table_class;

// methods

static void set_or_load(
						Request& r, 
						const String& method_name, Array *params, 
						bool is_load) {
	Pool& pool=r.pool();
	// data is last parameter
	Value *vdata_or_filename=static_cast<Value *>(params->get(params->size()-1));
	// forcing
	// ^load[this file name type]
	// ^set{this body type}
	r.fail_if_junction_(is_load, *vdata_or_filename, 
		method_name, is_load?"file name must not be junction":"body must be junction");

	// data or file_name
	char *data;
	if(is_load) {
		// forcing untaint language
		String lfile_name(pool);
		lfile_name.append(vdata_or_filename->as_string(), String::UL_FILE_NAME, true);
		// loading text
		data=file_read_text(pool, r.absolute(lfile_name));
	} else {
		// suggesting untaint language
		Temp_lang temp_lang(r, String::UL_TABLE);
		data=r.process(*vdata_or_filename).as_string().cstr();
	}

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
	Table& table=*new(pool) Table(pool, &method_name, columns);
	char *row_chars;
	while(row_chars=getrow(&data)) {
		if(!*row_chars) // remove empty lines
			continue;
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
	static_cast<VTable *>(r.self)->set_table(table);
}


static void _set(Request& r, const String& method_name, Array *params) {
	set_or_load(r, method_name, params, false);
}

static void _load(Request& r, const String& method_name, Array *params) {
	set_or_load(r, method_name, params, true);
}

static void _save(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value *vfile_name=static_cast<Value *>(params->get(params->size()-1));
	// forcing
	// ^save[this body type]
	r.fail_if_junction_(true, *vfile_name, 
		method_name, "file name must not be junction");

	// forcing untaint language
	String lfile_name(pool);
	lfile_name.append(vfile_name->as_string(),
		String::UL_FILE_NAME, true);

	Table& table=static_cast<VTable *>(r.self)->table();

	String sdata(pool);
	if(params->size()==1) { // not nameless=named output
		// write out names line
		if(table.columns()) { // named table
			for(int column=0; column<table.columns()->size(); column++) {
				if(column)
					sdata.APPEND_CONST("\t");
				sdata.append(*static_cast<String *>(table.columns()->quick_get(column)), 
					String::UL_TABLE);
			}
		} else { // nameless table
			int lsize=table.size()?static_cast<Array *>(table.get(0))->size():0;
			if(lsize)
				for(int column=0; column<lsize; column++) {
					char *cindex_tab=(char *)malloc(MAX_NUMBER);
					snprintf(cindex_tab, MAX_NUMBER, "%d\t", column);
					sdata.APPEND_CONST(cindex_tab);
				}
			else
				sdata.APPEND_CONST("empty nameless table");
		}
		sdata.APPEND_CONST("\n");
	}
	// data lines
	for(int index=0; index<table.size(); index++) {
		Array *row=static_cast<Array *>(table.quick_get(index));
		for(int column=0; column<row->size(); column++) {
			if(column)
				sdata.APPEND_CONST("\t");
			sdata.append(*static_cast<String *>(row->quick_get(column)), 
				String::UL_TABLE);
		}
		sdata.APPEND_CONST("\n");
	}

	// write
	file_write(pool, r.absolute(lfile_name), sdata.cstr(), sdata.size(), true);
}

static void _count(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, static_cast<VTable *>(r.self)->table().size());
	r.write_no_lang(value);
}

static void _line(Request& r, const String&, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, 1+static_cast<VTable *>(r.self)->table().get_current());
	r.write_no_lang(value);
}

static void _offset(Request& r, const String&, Array *params) {
	Pool& pool=r.pool();
	Table& table=static_cast<VTable *>(r.self)->table();
	if(params->size()) {
		if(int size=table.size()) {
			int offset=
				(int)r.process(*static_cast<Value *>(params->get(0))).as_double();
			table.set_current((table.get_current()+offset+size)%size);
		}
	} else {
		Value& value=*new(pool) VInt(pool, table.get_current());
		r.write_no_lang(value);
	}
}

static void _menu(Request& r, const String& method_name, Array *params) {
	Value& body_code=*static_cast<Value *>(params->get(0));
	// forcing ^menu{this param type}
	r.fail_if_junction_(false, body_code, 
		method_name, "body must be junction");
	
	Value *delim_code=params->size()==2?static_cast<Value *>(params->get(1)):0;

	Table& table=static_cast<VTable *>(r.self)->table();
	bool need_delim=false;
	for(int row=0; row<table.size(); row++) {
		table.set_current(row);

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
	Table& table=static_cast<VTable *>(r.self)->table();
	if(table.size()==0) {
		Value& value=r.process(*static_cast<Value *>(params->get(0)));
		r.write_pass_lang(value);
	} else if(params->size()==2) {
		Value& value=r.process(*static_cast<Value *>(params->get(1)));
		r.write_pass_lang(value);
	}
}

struct Record_info {
	Pool *pool;
	Table *table;
	Hash *hash;
};
static void store_column_item_to_hash(Array::Item *item, void *info) {
	Record_info& ri=*static_cast<Record_info *>(info);
	String& column_name=*static_cast<String *>(item);
	const String *column_item=ri.table->item(column_name);
	Value *value;
	if(column_item)
		value=new(*ri.pool) VString(*column_item);
	else
		value=new(*ri.pool) VUnknown(*ri.pool);
	ri.hash->put(column_name, value);
}
static void _record(Request& r, const String&, Array *params) {
	Table& table=static_cast<VTable *>(r.self)->table();
	if(const Array *columns=table.columns()) {
		Pool& pool=r.pool();
		Value& value=*new(pool) VHash(pool);
		Record_info record_info={&pool, &table, value.get_hash()};
		columns->for_each(store_column_item_to_hash, &record_info);
		
		r.write_no_lang(value);
	}
}

struct Seq_item {
	Array *row;
	union {
		char *c_str;
		double d;
	} value;
};
static int sort_cmp_string(const void *a, const void *b) {
	return strcmp(
		static_cast<const Seq_item *>(a)->value.c_str, 
		static_cast<const Seq_item *>(b)->value.c_str
	);
}
static int sort_cmp_double(const void *a, const void *b) {
	double va=static_cast<const Seq_item *>(a)->value.d;
	double vb=static_cast<const Seq_item *>(b)->value.d;
	if(va<vb)
		return -1;
	else if(va>vb)
		return +1;
	else 
		return 0;
}
static void _sort(Request& r, const String& method_name, Array *params) {
	Value& key_maker=*(Value *)params->get(0);
	// forcing ^sort{this} ^sort(or this) param type
	r.fail_if_junction_(false, key_maker, method_name, "key-maker must be junction");

	bool reverse;
	if(params->size()==2) { // ..[asc|desc]
		Value& sequence=*(Value *)params->get(1);
		// forcing ..[this param-type]
		r.fail_if_junction_(true, sequence, method_name, "sequence must not be junction");
		reverse=sequence.as_string()=="desc";
	} else
		reverse=false;

	Table& table=static_cast<VTable *>(r.self)->table();

	// anything to sort?
	if(!table.size())
		return;

	Seq_item *seq=(Seq_item *)malloc(sizeof(Seq_item)*table.size());
	int i;

	// calculate key values
	bool key_values_are_strings=true;
	for(i=0; i<table.size(); i++) {
		table.set_current(i);
		// calculate key value
		seq[i].row=(Array *)table.get(i);
		Value& value=*r.process(key_maker).as_expr_result(true/*return string as-is*/);
		if(i==0) // determining key values type by first one
			key_values_are_strings=value.is_string();

		if(key_values_are_strings)
			seq[i].value.c_str=value.as_string().cstr();
		else
			seq[i].value.d=value.as_double();
	}
	// sort keys
	_qsort(seq, table.size(), sizeof(Seq_item), 
		key_values_are_strings?sort_cmp_string:sort_cmp_double);

	// reorder table as they require in 'seq'
	for(i=0; i<table.size(); i++)
		table.put(i, seq[reverse?table.size()-1-i:i].row);

	// reset 'current'
	table.set_current(0);
}

// initialize

void initialize_table_class(Pool& pool, VStateless_class& vclass) {
	// ^table.set{data}
	// ^table.set[nameless]{data}
	vclass.add_native_method("set", _set, 1, 2);

	// ^table.load[file]  
	// ^table.load[nameless;file]
	vclass.add_native_method("load", _load, 1, 2);

	// ^table.save[file]  
	// ^table.save[nameless;file]
	vclass.add_native_method("save", _save, 1, 2);

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

	// ^table.record[]
	vclass.add_native_method("record", _record, 0, 0);

	// ^table.sort{string-key-maker} ^table.sort{string-key-maker}[asc|desc]
	// ^table.sort(numeric-key-maker) ^table.sort(numeric-key-maker)[asc|desc]
	vclass.add_native_method("sort", _sort, 1, 2);

}	
