/** @file
	Parser: @b table parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: table.C,v 1.57 2001/04/06 08:55:18 paf Exp $
*/

#include "pa_config_includes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "_table.h"
#include "pa_vtable.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"

// global var

VStateless_class *table_class;

// methods
static void _set(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	// data is last parameter
	Value *vdata=static_cast<Value *>(params->get(params->size()-1));
	// forcing {this body type}
	r.fail_if_junction_(false, *vdata, method_name, "body must be code");

	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	const String& data=r.process(*vdata).as_string();

	size_t pos_after=0;
	// parse columns
	Array *columns;
	if(params->size()==2) {
		columns=0;
	} else {
		columns=new(pool) Array(pool);

		Array head(pool);
		data.split(head, &pos_after, "\n", 1, String::UL_CLEAN, 1);
		if(head.size())
			head.get_string(0)->split(*columns, 0, "\t", 1, String::UL_CLEAN);
	}

	Table& table=*new(pool) Table(pool, &method_name, columns);
	// parse cells
	Array rows(pool);
	data.split(rows, &pos_after, "\n", 1, String::UL_CLEAN);
	int size=rows.quick_size();
	for(int i=0; i<size; i++) {
		Array& row=*new(pool) Array(pool);
		const String& string=*rows.quick_get_string(i);
		// remove empty lines
		if(!string.size())
			continue;

		string.split(row, 0, "\t", 1, String::UL_CLEAN);
		table+=&row;
	}

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(table);
}

static void _load(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	// filename is last parameter
	Value *vfilename=static_cast<Value *>(params->get(params->size()-1));
	// forcing [this file name type]
	r.fail_if_junction_(true, *vfilename, 
		method_name, "file name must not be code");

	// loading text
	char *data=file_read_text(pool, r.absolute(vfilename->as_string()));

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
				name->APPEND_TAINTED(lsplit(&row_chars, '\t'), 0, file, line++);
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
			cell->APPEND_TAINTED(cell_chars, 0, file, line);
			*row+=cell;
		}
		line++;
		table+=row;
	};

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(table);
}

static void _save(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Value *vtable_name=static_cast<Value *>(params->get(params->size()-1));
	// forcing this body type]
	r.fail_if_junction_(true, *vtable_name, 
		method_name, "file name must not be code");

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
	file_write(pool, r.absolute(vtable_name->as_string()), 
		sdata.cstr(), sdata.size(), true);
}

static void _count(Request& r, const String&method_name, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, static_cast<VTable *>(r.self)->table().size());
	r.write_no_lang(value);
}

static void _line(Request& r, const String& method_name, Array *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, 1+static_cast<VTable *>(r.self)->table().current());
	r.write_no_lang(value);
}

static void _offset(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	Table& table=static_cast<VTable *>(r.self)->table();
	if(params->size())
		table.shift((int)r.process(*static_cast<Value *>(params->get(0))).as_double());
	else {
		Value& value=*new(pool) VInt(pool, table.current());
		r.write_no_lang(value);
	}
}

/// @test $a.menu{ $a[123] }
static void _menu(Request& r, const String& method_name, Array *params) {
	Value& body_code=*static_cast<Value *>(params->get(0));
	// forcing ^menu{this param type}
	r.fail_if_junction_(false, body_code, 
		method_name, "body must be code");
	
	Value *delim_code=params->size()==2?static_cast<Value *>(params->get(1)):0;

	Table& table=static_cast<VTable *>(r.self)->table();
	bool need_delim=false;
	int saved_current=table.current();
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
	table.set_current(saved_current);
}

static void _empty(Request& r, const String& method_name, Array *params) {
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
static void _record(Request& r, const String& method_name, Array *params) {
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
	r.fail_if_junction_(false, key_maker, method_name, "key-maker must be code");

	bool reverse;
	if(params->size()==2) { // ..[asc|desc]
		Value& order=*(Value *)params->get(1);
		// forcing ..[this param-type]
		r.fail_if_junction_(true, order, method_name, "order must not be code");
		reverse=order.as_string()=="desc";
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

static void _locate(Request& r, const String& method_name, Array *params) {
	VTable& vtable=*static_cast<VTable *>(r.self);
	Table& table=vtable.table();
	vtable.last_locate_was_successful=table.locate(
		static_cast<Value *>(params->get(0))->as_string(),
		static_cast<Value *>(params->get(1))->as_string());
}

static void _found(Request& r, const String& method_name, Array *params) {
	if(static_cast<VTable *>(r.self)->last_locate_was_successful) {
		Value& then_code=*static_cast<Value *>(params->get(0));
		// forcing ^found{this param type}
		r.fail_if_junction_(false, then_code, 
			method_name, "found-parameter must be code");
		r.write_pass_lang(r.process(then_code));
	} else if(params->size()==2) {
		Value& else_code=*static_cast<Value *>(params->get(1));
		// forcing ^found{this param type}
		r.fail_if_junction_(false, else_code, 
			method_name, "not found-parameter must be code");
		r.write_pass_lang(r.process(else_code));
	}
}

static void _flip(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	VTable& vtable=*static_cast<VTable *>(r.self);

	Table& old_table=*vtable.get_table();
	Table& new_table=*new(pool) Table(pool, &method_name, 0/*nameless*/);
	if(old_table.size())
		if(int old_cols=old_table.at(0).size()) 
			for(int column=0; column<old_cols; column++) {
				Array& new_row=*new(pool) Array(pool, old_table.size());
				for(int i=0; i<old_table.size(); i++) {
					const Array& old_row=old_table.at(i);
					new_row+=column<old_row.size()?old_row.get(column):empty_string;
				}
				new_table+=&new_row;
			}

	vtable.set_table(new_table);
}

static void _append(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();
	// data is last parameter
	Value *value=static_cast<Value *>(params->get(0));
	// forcing {this body type}
	r.fail_if_junction_(false, *value, method_name, "body must be code");

	const String& string=r.process(*value).as_string();

	// parse cells
	Array& row=*new(pool) Array(pool);
	string.split(row, 0, "\t", 1, String::UL_CLEAN);

	static_cast<VTable *>(r.self)->table()+=&row;
}

static void _join(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value *value=static_cast<Value *>(params->get(0));
	// forcing [this table ref type]
	r.fail_if_junction_(true, *value, method_name, "table ref must not be code");

	Table *maybe_src=value->get_table();
	if(!maybe_src)
		PTHROW(0, 0,
			&method_name,
			"source is not a table");

	Table& src=*maybe_src;
	Table& dest=static_cast<VTable *>(r.self)->table();
	if(&src == &dest)
		PTHROW(0, 0,
			&method_name,
			"source and destination are same table");

	if(const Array *dest_columns=dest.columns()) { // dest is named
		int saved_src_current=src.current();
		for(int src_row=0; src_row<src.size(); src_row++) {
			src.set_current(src_row);
			Array& dest_row=*new(pool) Array(pool);
			for(int dest_column=0; dest_column<dest_columns->size(); dest_column++) 
				dest_row+=src.item(*dest_columns->get_string(dest_column));
			dest+=&dest_row;
		}
		src.set_current(saved_src_current);
	} else { // dest is nameless
		for(int src_row=0; src_row<src.size(); src_row++)
			dest+=&src.at(src_row);
	}
}

/// ^table:sql{query}[(count[;offset])]
static void _sql(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	if(!r.connection)
		PTHROW(0, 0,
			&method_name,
			"without connect");

	Value& statement=*static_cast<Value *>(params->get(0));
	// forcing {this query param type}
	r.fail_if_junction_(false, statement, method_name, "statement must be code");

	ulong limit=0;
	if(params->size()>1) {
		Value& limit_code=*static_cast<Value *>(params->get(1));
		// forcing (this limit param type)
		r.fail_if_junction_(false, limit_code, method_name, "limit must be expression");
		limit=(uint)r.process(limit_code).as_double();
	}

	ulong offset=0;
	if(params->size()>2) {
		Value& offset_code=*static_cast<Value *>(params->get(2));
		// forcing (this limit param type)
		r.fail_if_junction_(false, offset_code, method_name, "offset must be expression");
		offset=(ulong)r.process(offset_code).as_double();
	}

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process(statement).as_string();
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection);
	unsigned int sql_column_count; SQL_Driver::Cell *sql_columns;
	unsigned long sql_row_count; SQL_Driver::Cell **sql_rows;
	bool need_rethrow=false; Exception rethrow_me;
	PTRY {
		r.connection->query(
			statement_cstr, offset, limit,
			&sql_column_count, &sql_columns,
			&sql_row_count, &sql_rows);
	}
	PCATCH(e) { // connect/process problem
		rethrow_me=e;  need_rethrow=true;
	}
	PEND_CATCH
	if(need_rethrow)
		PTHROW(rethrow_me.type(), rethrow_me.code(),
			&statement_string, // setting more specific source [were url]
			rethrow_me.comment());
	
	Array& table_columns=*new(pool) Array(pool);
	for(unsigned int i=0; i<sql_column_count; i++) {
		String& table_column=*new(pool) String(pool);
		table_column.APPEND_TAINTED(
			(const char *)sql_columns[i].ptr, sql_columns[i].size,
			statement_cstr, 0);
		table_columns+=&table_column;
	}

	Table& table=*new(pool) Table(pool, &method_name, &table_columns);

	{
		for(unsigned long r=0; r<sql_row_count; r++) {
			SQL_Driver::Cell *sql_cells=sql_rows[r];
			Array& table_row=*new(pool) Array(pool);
			
			for(unsigned int i=0; i<sql_column_count; i++) {
				String& table_cell=*new(pool) String(pool);
				table_cell.APPEND_TAINTED(
					(const char *)sql_cells[i].ptr, sql_cells[i].size,
					statement_cstr, r);
				table_row+=&table_cell;
			}
			table+=&table_row;
		}
	}

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(table);
}

// initialize

void initialize_table_class(Pool& pool, VStateless_class& vclass) {
	// ^table:set{data}
	// ^table:set[nameless]{data}
	vclass.add_native_method("set", Method::CT_DYNAMIC, _set, 1, 2);

	// ^table:load[file]  
	// ^table:load[nameless;file]
	vclass.add_native_method("load", Method::CT_DYNAMIC, _load, 1, 2);

	// ^table.save[file]  
	// ^table.save[nameless;file]
	vclass.add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^table.count[]
	vclass.add_native_method("count", Method::CT_DYNAMIC, _count, 0, 0);

	// ^table.line[]
	vclass.add_native_method("line", Method::CT_DYNAMIC, _line, 0, 0);

	// ^table.offset[]  
	// ^table.offset[offset]
	vclass.add_native_method("offset", Method::CT_DYNAMIC, _offset, 0, 1);

	// ^table.menu{code}  
	// ^table.menu{code}[delim]
	vclass.add_native_method("menu", Method::CT_DYNAMIC, _menu, 1, 2);

	// ^table.empty{code-when-empty}  
	// ^table.empty{code-when-empty}{code-when-not}
	vclass.add_native_method("empty", Method::CT_DYNAMIC, _empty, 1, 2);

	// ^table.record[]
	vclass.add_native_method("record", Method::CT_DYNAMIC, _record, 0, 0);

	// ^table.sort{string-key-maker} ^table.sort{string-key-maker}[asc|desc]
	// ^table.sort(numeric-key-maker) ^table.sort(numeric-key-maker)[asc|desc]
	vclass.add_native_method("sort", Method::CT_DYNAMIC, _sort, 1, 2);

	// ^table.locate[field;value]
	vclass.add_native_method("locate", Method::CT_DYNAMIC, _locate, 2, 2);
	// ^table.found{when-found}
	// ^table.found{when-found}{when-not-found}
	vclass.add_native_method("found", Method::CT_DYNAMIC, _found, 1, 2);

	// ^table.flip[]
	vclass.add_native_method("flip", Method::CT_DYNAMIC, _flip, 0, 0);

	// ^table.append{r{tab}e{tab}c{tab}o{tab}r{tab}d}
	vclass.add_native_method("append", Method::CT_DYNAMIC, _append, 1, 1);

	// ^table.join[table]
	vclass.add_native_method("join", Method::CT_DYNAMIC, _join, 1, 1);


	// ^table:sql[query][(count[;offset])]
	vclass.add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 3);
}
