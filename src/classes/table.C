/** @file
	Parser: @b table parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: table.C,v 1.146 2002/03/27 15:30:34 paf Exp $
*/

#include "classes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_vtable.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"
#include "pa_vbool.h"

// defines

#define TABLE_CLASS_NAME "table"

// class

class MTable : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VTable(pool); }

public:
	MTable(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _create(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	// clone?
	if(params->size()==1) 
		if(const Table *source=params->get(0).get_table()) {
			static_cast<VTable *>(r.self)->set_table(*new(pool) Table(*source));
			return;
		}

	// data is last parameter
	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	const String& data=
		r.process(params->as_junction(params->size()-1, "body must be code")).as_string();

	size_t pos_after=0;
	// parse columns
	Array *columns;
	if(params->size()==2) {
		columns=0;
	} else {
		columns=new(pool) Array(pool);

		Array head(pool);
		data.split(head, &pos_after, "\n", 1, String::UL_AS_IS, 1);
		if(head.size())
			head.get_string(0)->split(*columns, 0, "\t", 1, String::UL_AS_IS);
	}

	Table& table=*new(pool) Table(pool, &method_name, columns);
	// parse cells
	Array rows(pool);
	data.split(rows, &pos_after, "\n", 1, String::UL_AS_IS);
	Array_iter i(rows);
	while(i.has_next()) {
		Array& row=*new(pool) Array(pool);
		const String& string=*i.next_string();
		// remove comment lines
		if(!string.size())
			continue;

		string.split(row, 0, "\t", 1, String::UL_AS_IS);
		table+=&row;
	}

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(table);
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	// filename is last parameter
	Value& vfile_name=params->as_no_junction(params->size()-1, 
		"file name must not be code");

	// loading text
	char *data=file_read_text(pool, r.absolute(vfile_name.as_string()));

	// parse columns
	Array *columns;
#ifndef NO_STRING_ORIGIN
	const Origin& origin=method_name.origin();
	const char *file=origin.file;
	uint line=origin.line;
#endif
	if(params->size()==2) {
		columns=0; // nameless
	} else {
		columns=new(pool) Array(pool);

		while(char *row_chars=getrow(&data)) {
			// remove empty&comment lines
			if(!*row_chars || *row_chars == '#')
				continue;
			do {
				String *name=new(pool) String(pool);
				name->APPEND_TAINTED(lsplit(&row_chars, '\t'), 0, file, line++);
				*columns+=name;
			} while(row_chars);

			break;
		}
	}

	// parse cells
	Table& table=*new(pool) Table(pool, &method_name, columns);
	char *row_chars;
	while(row_chars=getrow(&data)) {
		// remove empty&comment lines
		if(!*row_chars || *row_chars == '#')
			continue;
		Array *row=new(pool) Array(pool);
		while(char *cell_chars=lsplit(&row_chars, '\t')) {
			String *cell=new(pool) String(pool);
			cell->APPEND_TAINTED(cell_chars, 0, file, line);
			*row+=cell;
		}
#ifndef NO_STRING_ORIGIN
		line++;
#endif
		table+=row;
	};

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(table);
}

/// @todo "x\nx" "xxx""xx"
static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vfile_name=params->as_no_junction(params->size()-1, 
		"file name must not be code");

	Table& table=static_cast<VTable *>(r.self)->table();

	bool do_append=false;
	String sdata(pool);
	if(params->size()==1) { // named output
		// write out names line
		if(table.columns()) { // named table
			Array_iter i(*table.columns());
			while(i.has_next()) {
				sdata.append(*i.next_string(), //*static_cast<String *>(table.columns()->quick_get(column)), 
					String::UL_TABLE);
				if(i.has_next())
					sdata.APPEND_CONST("\t");
			}
		} else { // nameless table
			if(int lsize=table.size()?static_cast<Array *>(table.get(0))->size():0)
				for(int column=0; column<lsize; column++) {
					char *cindex_tab=(char *)pool.malloc(MAX_NUMBER);
					snprintf(cindex_tab, MAX_NUMBER, "%d\t", column);
					sdata.APPEND_CONST(cindex_tab);
				}
			else
				sdata.APPEND_CONST("empty nameless table");
		}
		sdata.APPEND_CONST("\n");
	} else { // mode specified
		const String& mode=params->as_string(0, "mode must be string");
		if(mode=="append")
			do_append=true;
		else if(mode=="nameless")
			/*ok, already skipped names output*/;
		else
			throw Exception("parser.runtime",
				&mode,
				"unknown mode, must be 'append'");

	}
	// data lines
	Array_iter i(table);
	while(i.has_next()) {
		Array_iter c(*static_cast<Array *>(i.next()));
		while(c.has_next()) {
			sdata.append(*c.next_string(), //*static_cast<String *>(row->quick_get(column)), 
				String::UL_TABLE);
			if(c.has_next())
				sdata.APPEND_CONST("\t");
		}
		sdata.APPEND_CONST("\n");
	}

	// write
	file_write(r.absolute(vfile_name.as_string()), 
		sdata.cstr(), sdata.size(), true, do_append);
}

static void _count(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, static_cast<VTable *>(r.self)->table().size());
	value.set_name(method_name);
	r.write_no_lang(value);
}

static void _line(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, 1+static_cast<VTable *>(r.self)->table().current());
	value.set_name(method_name);
	r.write_no_lang(value);
}

static void _offset(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Table& table=static_cast<VTable *>(r.self)->table();
	if(params->size()) {
		bool absolute=false;
		if(params->size()>1) {
		    const String& whence=params->as_string(0, "whence must be string");
		    if(whence=="cur")
				absolute=false;
		    else if(whence=="set")
				absolute=true;
		    else
				throw Exception("parser.runtime",
					&whence,
					"is invalid whence, valid are 'cur' or 'set'");
		}		    
		
		Value& offset_expr=params->as_junction(params->size()-1, "offset must be expression");
		table.offset(absolute, r.process(offset_expr).as_int());
	} else {
		Value& value=*new(pool) VInt(pool, table.current());
		value.set_name(method_name);
		r.write_no_lang(value);
	}
}

static void _menu(Request& r, const String& method_name, MethodParams *params) {
	Value& body_code=params->as_junction(0, "body must be code");
	
	Value *delim_maybe_code=params->size()>1?&params->get(1):0;

	VTable& vtable=*static_cast<VTable *>(r.self);
	Table& table=vtable.table();
	bool need_delim=false;
	int saved_current=table.current();
	int size=table.size();
	for(int row=0; row<size; row++) {
		table.set_current(row);

		Value& processed_body=r.process(body_code);
		if(delim_maybe_code) { // delimiter set?
			const String *string=processed_body.get_string();
			if(need_delim && string && string->size()) // need delim & iteration produced string?
				r.write_pass_lang(r.process(*delim_maybe_code));
			need_delim=true;
		}
		r.write_pass_lang(processed_body);
	}
	table.set_current(saved_current);
}

#ifndef DOXYGEN
struct Row_info {
	Table *table;
	int key_field;
	Array *value_fields;
	Hash *hash;
};
#endif
static void table_row_to_hash(Array::Item *value, void *info) {
	Array& row=*static_cast<Array *>(value);
	Row_info& ri=*static_cast<Row_info *>(info);
	Pool& pool=ri.table->pool();

	if(ri.key_field<row.size()) {
		VHash& result=*new(pool) VHash(pool);
		Hash& hash=*result.get_hash(0);
		for(int i=0; i<ri.value_fields->size(); i++) {
			int value_field=ri.value_fields->get_int(i);
			if(value_field<row.size())
				hash.put(
					*ri.table->columns()->get_string(value_field), 
					new(pool) VString(*row.get_string(value_field)));
		}
		
		ri.hash->put(*row.get_string(ri.key_field), &result);
	}
}
static void _hash(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Table& self_table=static_cast<VTable *>(r.self)->table();
	Value& result=*new(pool) VHash(pool);
	if(const Array *columns=self_table.columns()) 
		if(columns->size()>1) {
			const String& key_field_name=params->as_no_junction(0, 
				"key field name must not be code").as_string();
			int key_field=self_table.column_name2index(key_field_name, true);

			Array value_fields(pool);
			if(params->size()>1) {
				Value& value_fields_param=params->as_no_junction(1, "value field(s) must not be code");
				if(value_fields_param.is_string()) {
					value_fields+=self_table.column_name2index(value_fields_param.as_string(), true);
				} else if(Table *value_fields_table=value_fields_param.get_table()) {
					for(int i=0; i<value_fields_table->size(); i++) {
						const String& value_field_name=
							*static_cast<Array *>(value_fields_table->get(i))->get_string(0);
						value_fields+=self_table.column_name2index(value_field_name, true);
					}
				} else
					throw Exception("parser.runtime",
						&method_name,
						"value field(s) must be string or self_table"
					);
			} else { // by all columns, including key
				for(int i=0; i<columns->size(); i++)
					value_fields+=i;
			}

			// integers: key_field & value_fields
			Row_info row_info={&self_table, key_field, &value_fields, result.get_hash(0)};
			self_table.for_each(table_row_to_hash, &row_info);
		}
	result.set_name(method_name);
	r.write_no_lang(result);
}

#ifndef DOXYGEN
struct Table_seq_item {
	Array *row;
	union {
		char *c_str;
		double d;
	} value;
};
#endif
static int sort_cmp_string(const void *a, const void *b) {
	return strcmp(
		static_cast<const Table_seq_item *>(a)->value.c_str, 
		static_cast<const Table_seq_item *>(b)->value.c_str
	);
}
static int sort_cmp_double(const void *a, const void *b) {
	double va=static_cast<const Table_seq_item *>(a)->value.d;
	double vb=static_cast<const Table_seq_item *>(b)->value.d;
	if(va<vb)
		return -1;
	else if(va>vb)
		return +1;
	else 
		return 0;
}
static void _sort(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& key_maker=params->as_junction(0, "key-maker must be code");

	bool reverse=params->size()>1/*..[desc|asc|]*/?
		reverse=params->as_no_junction(1, "order must not be code").as_string()=="desc":
		false; // default=asc

	Table& old_table=static_cast<VTable *>(r.self)->table();
	Table& new_table=*new(pool) Table(pool, &method_name, old_table.columns());

	Table_seq_item *seq=(Table_seq_item *)pool.malloc(sizeof(Table_seq_item)*old_table.size());
	int i;

	// calculate key values
	bool key_values_are_strings=true;
	// save 'current'
	int saved_current=old_table.current();
	for(i=0; i<old_table.size(); i++) {
		old_table.set_current(i);
		// calculate key value
		seq[i].row=(MethodParams *)old_table.get(i);
		Value& value=*r.process(key_maker).as_expr_result(true/*return string as-is*/);
		if(i==0) // determining key values type by first one
			key_values_are_strings=value.is_string();

		if(key_values_are_strings)
			seq[i].value.c_str=value.as_string().cstr();
		else
			seq[i].value.d=value.as_double();
	}
	// restore 'current'
	old_table.set_current(saved_current);
	// sort keys
	_qsort(seq, old_table.size(), sizeof(Table_seq_item), 
		key_values_are_strings?sort_cmp_string:sort_cmp_double);

	// reorder table as they require in 'seq'
	for(i=0; i<old_table.size(); i++)
		new_table+=seq[reverse?old_table.size()-1-i:i].row;

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(new_table);
}

static bool _locate_expression(Request& r, const String& method_name, MethodParams *params) {
	if(params->size()>1)
		throw Exception("parser.runtime", 
			&method_name,
			"locate by expression has only one parameter - expression");

	Value& expression_code=params->as_junction(0, "must be expression");

	VTable& vtable=*static_cast<VTable *>(r.self);
	Table& table=vtable.table();
	int saved_current=table.current();
	int size=table.size();
	for(int row=0; row<size; row++) {
		table.set_current(row);

		if(r.process(expression_code).as_bool())
			return true;
	}
	table.set_current(saved_current);
	return false;
}
static bool _locate_name_value(Request& r, const String& method_name, MethodParams *params) {
	if(params->size()>2)
		throw Exception("parser.runtime", 
			&method_name,
			"locate by name and value has only two parameters - name and value");

	VTable& vtable=*static_cast<VTable *>(r.self);
	Table& table=vtable.table();
	return table.locate(
		params->as_string(0, "column name must be string"),
		params->as_string(1, "value must be string")
	);
}
static void _locate(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& result=*new(pool) VBool(pool, 
		params->get(0).get_junction()?
		_locate_expression(r, method_name, params) :
		_locate_name_value(r, method_name, params));
	result.set_name(method_name);
	r.write_no_lang(result);
}

static void _flip(Request& r, const String& method_name, MethodParams *params) {
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
					new_row+=column<old_row.size()?old_row.get(column):new(pool) String(pool);
				}
				new_table+=&new_row;
			}

	r.write_no_lang(*new(pool) VTable(pool, &new_table));
}

static void _append(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	// data
	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	const String& string=
		r.process(params->as_junction(0, "body must be code")).as_string();

	// parse cells
	Array& row=*new(pool) Array(pool);
	string.split(row, 0, "\t", 1, String::UL_AS_IS);

	VTable& vtable=*static_cast<VTable *>(r.self);
	vtable.table()+=&row;
}

static void _join(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Table *maybe_src=params->as_no_junction(0, "table ref must not be code").get_table();
	if(!maybe_src)
		throw Exception("parser.runtime", 
			&method_name, 
			"source is not a table");

	Table& src=*maybe_src;
	Table& dest=static_cast<VTable *>(r.self)->table();
	if(&src == &dest)
		throw Exception("parser.runtime", 
			&method_name, 
			"source and destination are same table");

	if(const Array *dest_columns=dest.columns()) { // dest is named
		int saved_src_current=src.current();
		for(int src_row=0; src_row<src.size(); src_row++) {
			src.set_current(src_row);
			Array& dest_row=*new(pool) Array(pool);
			for(int dest_column=0; dest_column<dest_columns->size(); dest_column++) {
				const String *src_item=src.item(*dest_columns->get_string(dest_column));
				dest_row+=src_item?src_item:new(pool) String(pool);
			}
			dest+=&dest_row;
		}
		src.set_current(saved_src_current);
	} else { // dest is nameless
		for(int src_row=0; src_row<src.size(); src_row++)
			dest+=&src.at(src_row);
	}
}

#ifndef DOXYGEN
class Table_sql_event_handlers : public SQL_Driver_query_event_handlers {
public:
	Table_sql_event_handlers(Pool& apool, const String& amethod_name,
		const String& astatement_string, const char *astatement_cstr) :
		pool(apool), 
		method_name(amethod_name),
		statement_string(astatement_string),
		statement_cstr(astatement_cstr),
		columns(*new(pool) Array(pool)),
		row(0), 
		table(0)
	{
	}

	void add_column(void *ptr, size_t size) {
		String *column=new(pool) String(pool);
		column->APPEND_TAINTED(
			(const char *)ptr, size, 
			statement_cstr, 0);
		columns+=column;
	}
	void before_rows() { 
		table=new(pool) Table(pool, &method_name, &columns);
	}
	void add_row() {
		(*table)+=(row=new(pool) Array(pool));
	}
	void add_row_cell(void *ptr, size_t size) {
		String *cell=new(pool) String(pool);
		if(size)
			cell->APPEND_TAINTED(
				(const char *)ptr, size, 
				statement_cstr, table->size()-1);
		(*row)+=cell;
	}

private:
	Pool& pool;
	const String& method_name;
	const String& statement_string; const char *statement_cstr;
	Array& columns;
	Array *row;
public:
	Table *table;
};
#endif
static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& statement=params->as_junction(0, "statement must be code");

	ulong limit=0;
	ulong offset=0;
	if(params->size()>1) {
		Value& voptions=params->as_no_junction(1, "options must be hash, not code");
		if(voptions.is_defined())
			if(Hash *options=voptions.get_hash(&method_name)) {
				if(Value *vlimit=(Value *)options->get(*sql_limit_name))
					limit=(ulong)r.process(*vlimit).as_double();
				if(Value *voffset=(Value *)options->get(*sql_offset_name))
					offset=(ulong)r.process(*voffset).as_double();
			} else
				throw Exception("parser.runtime",
					&method_name,
					"options must be hash");
	}

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process(statement).as_string();
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection(&method_name));
	Table_sql_event_handlers handlers(pool, method_name,
		statement_string, statement_cstr);
	try {
#ifdef RESOURCES_DEBUG
	struct timeval mt[2];
	//measure:before
	gettimeofday(&mt[0],NULL);
#endif	
		r.connection(&method_name)->query(
			statement_cstr, offset, limit, 
			handlers);
	
#ifdef RESOURCES_DEBUG
		//measure:after connect
	gettimeofday(&mt[1],NULL);
	
	double t[2];
	for(int i=0;i<2;i++)
	    t[i]=mt[i].tv_sec+mt[i].tv_usec/1000000.0;
	    
	r.sql_request_time+=t[1]-t[0];
#endif	    			
	} catch(const Exception& e) { // query problem
		// more specific source [were url]
		throw Exception("sql.execute", 
			&statement_string, 
			"%s", e.comment());
	}

	Table *result=
		handlers.table?handlers.table: // query resulted in table? return it
		new(pool) Table(pool, &method_name, 0); // query returned no table, fake it

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(*result);
}

static void _columns(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();

	Array& result_columns=*new(pool) Array(pool);
	result_columns+=new(pool) String(pool, "column");
	Table& result_table=*new(pool) Table(pool, &method_name, &result_columns);

	Table& source_table=static_cast<VTable *>(r.self)->table();
	if(const Array *source_columns=source_table.columns()) {
		Array_iter i(*source_columns);
		while(i.has_next()) {
			Array& result_row=*new(pool) Array(pool);
			result_row+=i.next();
			result_table+=&result_row;
		}
	}

	VTable& result=*new(pool) VTable(pool, &result_table);
	result.set_name(method_name);
	r.write_no_lang(result);
}

// constructor

MTable::MTable(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), TABLE_CLASS_NAME));

	// ^table::create{data}
	// ^table::create[nameless]{data}
	// ^table::create[table]
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 2);
	// old name for compatibility with <= v 1.141 2002/01/25 11:33:45 paf
	add_native_method("set", Method::CT_DYNAMIC, _create, 1, 2); 

	// ^table::load[file]  
	// ^table::load[nameless;file]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 2);

	// ^table.save[file]  
	// ^table.save[nameless;file]
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^table.count[]
	add_native_method("count", Method::CT_DYNAMIC, _count, 0, 0);

	// ^table.line[]
	add_native_method("line", Method::CT_DYNAMIC, _line, 0, 0);

	// ^table.offset[]  
	// ^table.offset(offset)
	// ^table.offset[cur|set](offset)
	add_native_method("offset", Method::CT_DYNAMIC, _offset, 0, 2);

	// ^table.menu{code}  
	// ^table.menu{code}[delim]
	add_native_method("menu", Method::CT_DYNAMIC, _menu, 1, 2);

	// ^table:hash[key field name]
	// ^table:hash[key field name][value field name(s) string/table]
	add_native_method("hash", Method::CT_DYNAMIC, _hash, 1, 2);

	// ^table.sort{string-key-maker} ^table.sort{string-key-maker}[desc|asc]
	// ^table.sort(numeric-key-maker) ^table.sort(numeric-key-maker)[desc|asc]
	add_native_method("sort", Method::CT_DYNAMIC, _sort, 1, 2);

	// ^table.locate[field;value]
	add_native_method("locate", Method::CT_DYNAMIC, _locate, 1, 2);

	// ^table.flip[]
	add_native_method("flip", Method::CT_DYNAMIC, _flip, 0, 0);

	// ^table.append{r{tab}e{tab}c{tab}o{tab}r{tab}d}
	add_native_method("append", Method::CT_DYNAMIC, _append, 1, 1);

	// ^table.join[table]
	add_native_method("join", Method::CT_DYNAMIC, _join, 1, 1);


	// ^table:sql[query]
	// ^table:sql[query][$.limit(1) $.offset(2)]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// ^table:columns[]
	add_native_method("columns", Method::CT_DYNAMIC, _columns, 0, 0);
}

// global variable

Methoded *table_class;

// creator

Methoded *MTable_create(Pool& pool) {
	return table_class=new(pool) MTable(pool);
}
