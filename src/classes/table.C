/** @file
	Parser: @b table parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: table.C,v 1.92 2001/07/09 16:13:17 parser Exp $"; 

#include "pa_config_includes.h"

#include "pcre.h"

#include "classes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_vtable.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"
#include "pa_dir.h"
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

static void _set(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	// data is last parameter
	Value& vdata=params->as_junction(params->size()-1, "body must be code");

	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	const String& data=r.process(vdata).as_string();

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

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	// filename is last parameter
	Value& vfilename=params->as_no_junction(params->size()-1, 
		"file name must not be code");

	// loading text
	char *data=file_read_text(pool, r.absolute(vfilename.as_string()));

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

static Table *fill_month_days(Request& r, 
							  const String& method_name, MethodParams *params, bool rus){
	Pool& pool=r.pool();
	Table *result=new(pool) Table(pool, &method_name, 0/*&columns*/);

    int year=params->as_int(1, r);
    int month=max(1, min(params->as_int(2, r), 12)) -1;

    tm tmIn={0, 0, 0, 1, month, year-1900};
    time_t t=mktime(&tmIn);
	if(t<0)
		PTHROW(0, 0, 
			&method_name, 
			"invalid date");
    tm *tmOut=localtime(&t);

    int weekDay1=tmOut->tm_wday;
	if(rus) 
		weekDay1=weekDay1?weekDay1-1:6; //sunday last
    int monthDays=getMonthDays(year, month);
    
    for(int _day=1-weekDay1; _day<=monthDays;) {
		Array& row=*new(pool) Array(pool, 7);
    	for(int wday=0; wday<7; wday++, _day++) {
        	String *cell;
			if(_day>=1 && _day<=monthDays) {
				char *buf=(char *)malloc(2+1); 
				sprintf(buf, "%02d", _day); 
				cell=new(pool) String(pool, buf);
            } else
				cell=new(pool) String(pool);
			row+=cell;            
        }
    	*result+=&row;
    }
    
    return result;
}

static Table *fill_week_days(Request& r, 
							 const String& method_name, MethodParams *params, bool rus){
	Pool& pool=r.pool();
	Array& columns=*new(pool) Array(pool, 4);
	columns+=new(pool) String(pool, "year");
	columns+=new(pool) String(pool, "month");
	columns+=new(pool) String(pool, "day");
	columns+=new(pool) String(pool, "weekday");
	Table *result=new(pool) Table(pool, &method_name, &columns);

    int year=params->as_int(1, r);
    int month=max(1, min(params->as_int(2, r), 12)) -1;
    int day=params->as_int(3, r);
    
    tm tmIn={0, 0, 18, day, month, year-1900};
    time_t t=mktime(&tmIn);
	if(t<0)
		PTHROW(0, 0, 
			&method_name, 
			"invalid date");
    tm *tmOut=localtime(&t);
    
    int baseWeekDay=tmOut->tm_wday;
	if(rus) 
		baseWeekDay=baseWeekDay?baseWeekDay-1:6; //sunday last

    t-=baseWeekDay*SECS_PER_DAY;

    for(int curWeekDay=0; curWeekDay<7; curWeekDay++, t+=SECS_PER_DAY) {
        tm *tmOut=localtime(&t);
		Array& row=*new(pool) Array(pool, 4);
		{char *buf=(char *)malloc(4+1); sprintf(buf, "%04d", 1900+tmOut->tm_year); row+=new(pool) String(pool, buf);}
		{char *buf=(char *)malloc(2+1); sprintf(buf, "%02d", 1+tmOut->tm_mon); row+=new(pool) String(pool, buf);}
		{char *buf=(char *)malloc(2+1); sprintf(buf, "%02d", tmOut->tm_mday); row+=new(pool) String(pool, buf);}
		{char *buf=(char *)malloc(2+1); sprintf(buf, "%02d", tmOut->tm_wday); row+=new(pool) String(pool, buf);}
        *result+=&row;
    }
    
    return result;
}

static void _calendar(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	const String& what=params->as_string(0, "format must be strig");
	bool rus=false;
	if(what=="rus")
		rus=true;
	else if(what=="eng")
		rus=false;
	else
		PTHROW(0, 0, 
			&what, 
			"must be rus|eng");

	Table *result=0;
	if(params->size()==1+2) 
		result=fill_month_days(r, method_name, params, rus);
	else // 1+3
		result=fill_week_days(r, method_name, params, rus);

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(*result);
}




static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& vtable_name=params->as_no_junction(params->size()-1, 
		"file name must not be code");

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
	file_write(pool, r.absolute(vtable_name.as_string()), 
		sdata.cstr(), sdata.size(), true);
}

static void _count(Request& r, const String&, MethodParams *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, static_cast<VTable *>(r.self)->table().size());
	r.write_no_lang(value);
}

static void _line(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	Value& value=*new(pool) VInt(pool, 1+static_cast<VTable *>(r.self)->table().current());
	r.write_no_lang(value);
}

static void _offset(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Table& table=static_cast<VTable *>(r.self)->table();
	if(params->size()) {
		Value& offset_expr=params->as_junction(0, "offset must be expression");
		table.shift(r.process(offset_expr).as_int());
	} else {
		Value& value=*new(pool) VInt(pool, table.current());
		r.write_no_lang(value);
	}
}

static void _menu(Request& r, const String& method_name, MethodParams *params) {
	Value& body_code=params->as_junction(0, "body must be code");
	
	Value *delim_code=params->size()==2?&params->get(1):0;

	VTable& vtable=*static_cast<VTable *>(r.self);
	Table& table=vtable.table();
	bool need_delim=false;
	vtable.lock(); int saved_current=table.current();
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
	table.set_current(saved_current); vtable.unlock();
}

static void _empty(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Table& table=static_cast<VTable *>(r.self)->table();
	
	r.write_no_lang(*new(pool) VBool(pool, table.size()==0));
}

/// used by table: _record / store_column_item_to_hash
struct Record_info {
	Pool *pool;
	Table *table;
	Hash *hash;
};
static void store_column_item_to_hash(Array::Item *item, void *info) {
	Record_info& ri=*static_cast<Record_info *>(info);
	String& column_name=*static_cast<String *>(item);
	Value *value;
	if(const String *column_item=ri.table->item(column_name))
		value=new(*ri.pool) VString(*column_item);
	else
		value=new(*ri.pool) VVoid(*ri.pool);
	ri.hash->put(column_name, value);
}
static void _record(Request& r, const String& method_name, MethodParams *) {
	Table& table=static_cast<VTable *>(r.self)->table();
	if(const Array *columns=table.columns()) {
		Pool& pool=r.pool();
		Value& result=*new(pool) VHash(pool);
		Record_info record_info={&pool, &table, result.get_hash()};
		columns->for_each(store_column_item_to_hash, &record_info);
		result.set_name(method_name);
		r.write_no_lang(result);
	}
}

/// used by table: _hash / table_row_to_hash
struct Row_info {
	Table *table;
	int key_field;
	Array *value_fields;
	Hash *hash;
};
static void table_row_to_hash(Array::Item *value, void *info) {
	Array& row=*static_cast<Array *>(value);
	Row_info& ri=*static_cast<Row_info *>(info);
	Pool& pool=ri.table->pool();

	if(ri.key_field<row.size()) {
		VHash& result=*new(pool) VHash(pool);
		Hash& hash=*result.get_hash();
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
	Table& table=static_cast<VTable *>(r.self)->table();
	if(const Array *columns=table.columns()) 
		if(columns->size()>1) {
			Pool& pool=r.pool();

			const String& key_field_name=params->as_no_junction(0, 
				"key field name must not be code").as_string();
			int key_field=table.column_name2index(key_field_name, true);
			int value_fields_count=params->size()-1;
			bool value_fields_by_params=value_fields_count!=0;
			if(!value_fields_by_params)
				value_fields_count=columns->size()-1; // all columns except key
			Array value_fields(pool, value_fields_count);
			if(value_fields_by_params) {
				for(int i=1; i<params->size(); i++) {
					const String& value_field_name=params->as_no_junction(i, 
						"value field name must not be code").as_string();
					value_fields+=table.column_name2index(value_field_name, true);
				}
			} else { // by all columns except key
				for(int i=0; i<columns->size(); i++)
					if(i!=key_field)
						value_fields+=i;
			}

			// integers: key_field & value_fields
			Value& result=*new(pool) VHash(pool);
			Row_info row_info={&table, key_field, &value_fields, result.get_hash()};
			table.for_each(table_row_to_hash, &row_info);
			result.set_name(method_name);
			r.write_no_lang(result);
		}
}

/// used by table: _sort / sort_cmp_string|sort_cmp_double
struct Table_seq_item {
	Array *row;
	union {
		char *c_str;
		double d;
	} value;
};
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
	Value& key_maker=params->as_junction(0, "key-maker must be code");

	bool reverse=params->size()==2/*..[asc|desc]*/?
		reverse=params->as_no_junction(1, "order must not be code").as_string()=="desc":
		false;

	Table& table=static_cast<VTable *>(r.self)->table();

	// anything to sort?
	if(!table.size())
		return;

	Table_seq_item *seq=(Table_seq_item *)malloc(sizeof(Table_seq_item)*table.size());
	int i;

	// calculate key values
	bool key_values_are_strings=true;
	for(i=0; i<table.size(); i++) {
		table.set_current(i);
		// calculate key value
		seq[i].row=(MethodParams *)table.get(i);
		Value& value=*r.process(key_maker).as_expr_result(true/*return string as-is*/);
		if(i==0) // determining key values type by first one
			key_values_are_strings=value.is_string();

		if(key_values_are_strings)
			seq[i].value.c_str=value.as_string().cstr();
		else
			seq[i].value.d=value.as_double();
	}
	// sort keys
	_qsort(seq, table.size(), sizeof(Table_seq_item), 
		key_values_are_strings?sort_cmp_string:sort_cmp_double);

	// reorder table as they require in 'seq'
	for(i=0; i<table.size(); i++)
		table.put(i, seq[reverse?table.size()-1-i:i].row);

	// reset 'current'
	table.set_current(0);
}

static void _locate(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	VTable& vtable=*static_cast<VTable *>(r.self);
	Table& table=vtable.table();
	Value& result=*new(pool) VBool(pool, table.locate(
		params->get(0).as_string(), 
		params->get(1).as_string()));
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
					new_row+=column<old_row.size()?old_row.get(column):empty_string;
				}
				new_table+=&new_row;
			}

	vtable.set_table(new_table);
}

static void _append(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	// data is last parameter
	const String& string=
		r.process(params->as_junction(0, "body must be code")).as_string();

	// parse cells
	Array& row=*new(pool) Array(pool);
	string.split(row, 0, "\t", 1, String::UL_CLEAN);

	VTable& vtable=*static_cast<VTable *>(r.self);
	// disable ^a.menu{^a.append[]}
	vtable.lock();
	vtable.table()+=&row;
	vtable.unlock();
}

static void _join(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Table *maybe_src=params->as_no_junction(0, "table ref must not be code").get_table();
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

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	if(!r.connection)
		PTHROW(0, 0, 
			&method_name, 
			"without connect");

	Value& statement=params->as_junction(0, "statement must be code");

	ulong limit=0;
	if(params->size()>1) {
		Value& limit_code=params->as_junction(1, "limit must be expression");
		limit=(uint)r.process(limit_code).as_double();
	}

	ulong offset=0;
	if(params->size()>2) {
		Value& offset_code=params->as_junction(2, "offset must be expression");
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
	PCATCH(e) { // query problem
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

	for(unsigned long row=0; row<sql_row_count; row++) {
		SQL_Driver::Cell *sql_cells=sql_rows[row];
		Array& table_row=*new(pool) Array(pool);
		
		for(unsigned int i=0; i<sql_column_count; i++) {
			String& table_cell=*new(pool) String(pool);
			if(sql_cells[i].size)
				table_cell.APPEND_TAINTED(
					(const char *)sql_cells[i].ptr, sql_cells[i].size, 
					statement_cstr, row);
			table_row+=&table_cell;
		}
		table+=&table_row;
	}

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(table);
}

static void _dir(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& relative_path=params->as_no_junction(0, "path must not be code");

	const String *regexp;
	pcre *regexp_code;
	int ovecsize;
	int *ovector;
	if(params->size()>1) {
		regexp=&params->as_no_junction(1, "regexp must not be code").as_string();

		const char *pattern=regexp->cstr(String::UL_AS_IS);
		const char *errptr;
		int erroffset;
		regexp_code=pcre_compile(pattern, PCRE_EXTRA | PCRE_DOTALL, 
			&errptr, &erroffset, 
			r.pcre_tables);

		if(!regexp_code)
			PTHROW(0, 0, 
				&regexp->mid(erroffset, regexp->size()), 
				"regular expression syntax error - %s", errptr);

		ovector=(int *)malloc(sizeof(int)*(ovecsize=(1/*match*/)*3));
	} else 
		regexp_code=0;


	const char* absolute_path_cstr=r.absolute(relative_path.as_string())
		.cstr(String::UL_FILE_NAME);

	Array& columns=*new(pool) Array(pool);
	columns+=new(pool) String(pool, "name");	
	Table& table=*new(pool) Table(pool, &method_name, &columns);

	LOAD_DIR(absolute_path_cstr, 
		size_t file_name_size=strlen(ffblk.ff_name);
		bool suits=true;
		if(regexp_code) {
			int exec_result=pcre_exec(regexp_code, 0, 
				ffblk.ff_name, file_name_size, 0, 
				0, ovector, ovecsize);
			
			if(exec_result==PCRE_ERROR_NOMATCH)
				suits=false;
			else if(exec_result<0) {
				(*pcre_free)(regexp_code);
				PTHROW(0, 0, 
					regexp, 
					"regular expression execute (%d)", 
						exec_result);
			}
		}

		if(suits) {
			char *file_name_cstr=(char *)r.malloc(file_name_size);
			memcpy(file_name_cstr, ffblk.ff_name, file_name_size);
			String &file_name=*new(pool) String(pool);
			file_name.APPEND(file_name_cstr, file_name_size, String::UL_FILE_NAME, 
				method_name.origin().file, method_name.origin().line);
		
			Array& row=*new(pool) Array(pool);
			row+=&file_name;
			table+=&row;
		}
	);

	if(regexp_code)
		(*pcre_free)(regexp_code);

	// replace any previous table value
	static_cast<VTable *>(r.self)->set_table(table);
}

static void _columns(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();

	Array& result_columns=*new(pool) Array(pool);
	result_columns+=new(pool) String(pool, "column");
	Table& result_table=*new(pool) Table(pool, &method_name, &result_columns);

	Table& source_table=static_cast<VTable *>(r.self)->table();
	if(const Array *source_columns=source_table.columns()) {
		int size=source_columns->quick_size();
		for(int i=0; i<size; i++) {
			Array& result_row=*new(pool) Array(pool);
			result_row+=source_columns->quick_get(i);
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

	// ^table:set{data}
	// ^table:set[nameless]{data}
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 2);

	// ^table:load[file]  
	// ^table:load[nameless;file]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 2);

	// ^table:calendar[month|montheng;year;month]  
	// ^table:calendar[week|weekeng;year;month;day]
	add_native_method("calendar", Method::CT_DYNAMIC, _calendar, 3, 4);

	// ^table.save[file]  
	// ^table.save[nameless;file]
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^table.count[]
	add_native_method("count", Method::CT_DYNAMIC, _count, 0, 0);

	// ^table.line[]
	add_native_method("line", Method::CT_DYNAMIC, _line, 0, 0);

	// ^table.offset[]  
	// ^table.offset[offset]
	add_native_method("offset", Method::CT_DYNAMIC, _offset, 0, 1);

	// ^table.menu{code}  
	// ^table.menu{code}[delim]
	add_native_method("menu", Method::CT_DYNAMIC, _menu, 1, 2);

	// ^table.empty[]
	add_native_method("empty", Method::CT_DYNAMIC, _empty, 0, 0);

	// ^table.record[]
	add_native_method("record", Method::CT_DYNAMIC, _record, 0, 0);

	// ^table:hash[key field name]
	// ^table:hash[key field name][value field name;...]
	add_native_method("hash", Method::CT_DYNAMIC, _hash, 1, 1000);

	// ^table.sort{string-key-maker} ^table.sort{string-key-maker}[asc|desc]
	// ^table.sort(numeric-key-maker) ^table.sort(numeric-key-maker)[asc|desc]
	add_native_method("sort", Method::CT_DYNAMIC, _sort, 1, 2);

	// ^table.locate[field;value]
	add_native_method("locate", Method::CT_DYNAMIC, _locate, 2, 2);

	// ^table.flip[]
	add_native_method("flip", Method::CT_DYNAMIC, _flip, 0, 0);

	// ^table.append{r{tab}e{tab}c{tab}o{tab}r{tab}d}
	add_native_method("append", Method::CT_DYNAMIC, _append, 1, 1);

	// ^table.join[table]
	add_native_method("join", Method::CT_DYNAMIC, _join, 1, 1);


	// ^table:sql[query][(count[;offset])]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 3);

	// ^table:dir[path]
	// ^table:dir[path][regexp]
	add_native_method("dir", Method::CT_DYNAMIC, _dir, 1, 2);

	// ^table:columns[]
	add_native_method("columns", Method::CT_DYNAMIC, _columns, 0, 0);
}

// global variable

Methoded *table_class;

// creator

Methoded *MTable_create(Pool& pool) {
	return table_class=new(pool) MTable(pool);
}
