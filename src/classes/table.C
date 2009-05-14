/** @file
	Parser: @b table parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_TABLE_C="$Date: 2009/05/14 08:10:09 $";

#ifndef NO_STRINGSTREAM
#include <sstream>
using namespace std;
#endif

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vtable.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"
#include "pa_vbool.h"

// class

class MTable: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&, HashStringValue&) { return new VTable(); }

public:
	MTable();

public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(table, new MTable, 0);

// externs

extern String cycle_data_name;

#define TABLE_REVERSE_NAME "reverse"

// globals

String sql_bind_name(SQL_BIND_NAME);
String sql_limit_name(PA_SQL_LIMIT_NAME);
String sql_offset_name(PA_SQL_OFFSET_NAME);
String sql_default_name(SQL_DEFAULT_NAME);
String sql_distinct_name(SQL_DISTINCT_NAME);
String sql_value_type_name(SQL_VALUE_TYPE_NAME);
String table_reverse_name(TABLE_REVERSE_NAME);

// methods

static Table::Action_options get_action_options(Request& r, MethodParams& params, 
						const Table& source) {
	Table::Action_options result;
	if(!params.count())
		return result;

	Value& maybe_options=params.last();
/* can not do it: 
	want to enable ^table::create[$source;
#		$.option[]
	]
	but there is ^table.locate[name;value]

	...if(voptions.is_defined() && !voptions.is_string()))
	if(maybe_options.is_string()) { // allow empty options
		result.defined=true;
		return result;
	}
*/
	HashStringValue* options=maybe_options.get_hash();
	if(!options)
		return result;

	result.defined=true;
	bool defined_offset=false;

	int valid_options=0;
	if(Value* voffset=options->get(sql_offset_name)) {
		valid_options++;
		defined_offset=true;
		if(voffset->is_string()) {
			const String&  soffset=*voffset->get_string();
			if(soffset == "cur")
				result.offset=source.current();
			else
				throw Exception(PARSER_RUNTIME,
					&soffset,
					"must be 'cur' string or expression");
		} else 
			result.offset=r.process_to_value(*voffset).as_int();
	}
	if(Value* vlimit=options->get(sql_limit_name)) {
		valid_options++;
		result.limit=r.process_to_value(*vlimit).as_int();
	}
	if(Value *vreverse=(Value *)options->get(table_reverse_name)) { 
		valid_options++; 
		result.reverse=r.process_to_value(*vreverse).as_bool(); 
		if(result.reverse && !defined_offset) 
			result.offset=source.count()-1; 
	} 

	if(valid_options!=options->count())
		throw Exception(PARSER_RUNTIME,
			0,
			"called with invalid option");

	return result;
}
static void check_option_param(bool options_defined, 
			  MethodParams& params, size_t next_param_index,
			  const char *msg) {
	if(next_param_index+(options_defined?1:0) != params.count())
		throw Exception(PARSER_RUNTIME,
			0,
			"%s", msg);
}

struct TableSeparators {
	char column;  const String* scolumn;
	char encloser; const String* sencloser;

	TableSeparators():
		column('\t'), scolumn(new String("\t")),
		encloser(0), sencloser(0)
	{}
	int load( HashStringValue& options ) {
		int result=0;
		if(Value* vseparator=options.get(PA_COLUMN_SEPARATOR_NAME)) {
			scolumn=&vseparator->as_string();
			if(scolumn->length()!=1)
				throw Exception(PARSER_RUNTIME,
					scolumn,
					"separator must be one character long");
			column=scolumn->first_char();
			result++;
		}
		if(Value* vencloser=options.get(PA_COLUMN_ENCLOSER_NAME)) {
			sencloser=&vencloser->as_string();
			if(sencloser->length()!=1)
				throw Exception(PARSER_RUNTIME,
					sencloser,
					"encloser must be one character long");
			encloser=sencloser->first_char();
			result++;
		}
		return result;
	}
};

static void _create(Request& r, MethodParams& params) {
	// clone/copy part?
	if(Table *source=params[0].get_table()) {
		Table::Action_options o=get_action_options(r, params, *source);
		check_option_param(o.defined, params, 1, 
			"too many parameters");
		GET_SELF(r, VTable).set_table(*new Table(*source, o));
		return;
	}

	size_t data_param_index=0;
	bool nameless=false;

	if(params.count()>1) {
		if(params[0].is_string()){ // can be nameless only
			const String& snameless=params.as_string(0, "called with more then 1 param, first param may be only string 'nameless' or junction");
			if(snameless!="nameless")
				throw Exception(PARSER_RUNTIME,
					&snameless,
					"table::create called with more then 1 param, first param may be only 'nameless'");
			nameless=true;
			data_param_index++;
		}
	}

	HashStringValue *options=0;
	TableSeparators separators;

	size_t options_param_index=data_param_index+1;
	if(
		options_param_index<params.count()
		&& (options=params.as_no_junction(options_param_index, "additional params must be hash").get_hash())
	) {
		// cloning, so that we could change
		options=new HashStringValue(*options);
		separators.load(*options);
		if(separators.encloser){
			throw Exception(PARSER_RUNTIME,
				0,
				"encloser not supported for table::create yet");
		}
	}

	// data
	Temp_lang temp_lang(r, String::L_PASS_APPENDED);
	const String&  data=
		r.process_to_string(params.as_junction(data_param_index, "body must be code"));

	// parse columns
	size_t raw_pos_after=0;
	Table::columns_type columns;

	if(nameless){
		columns=Table::columns_type(0); // nameless
	} else {
		columns=Table::columns_type(new ArrayString);

		ArrayString head;
		data.split(head, raw_pos_after, "\n", String::L_AS_IS, 1);
		if(head.count()) {
			size_t col_pos_after=0;
			head[0]->split(*columns, col_pos_after, *separators.scolumn, String::L_AS_IS);
		}
	}

	Table& table=*new Table(columns);
	// parse cells

	ArrayString rows;
	data.split(rows, raw_pos_after, "\n", String::L_AS_IS);
	Array_iterator<const String*> i(rows);
	while(i.has_next()) {
		Table::element_type row(new ArrayString);
		const String& string=*i.next();
		// remove comment lines
		if(string.is_empty())
			continue;

		size_t col_pos_after=0;
		string.split(*row, col_pos_after, *separators.scolumn, String::L_AS_IS);
		table+=row;
	}

	// replace any previous table value
	GET_SELF(r, VTable).set_table(table);
}

struct lsplit_result {
	char* piece;
	char delim;

	operator bool() { return piece!=0; }
};

inline lsplit_result lsplit(char* string, char delim1, char delim2) {
	lsplit_result result;
    if(string) {
		char delims[]={delim1, delim2, 0};
		if(char* v=strpbrk(string, delims)) {
			result.delim=*v;
			*v=0;
			result.piece=v+1;
			return result;
		}
    }
	result.piece=0;
	result.delim=0;
    return result;
}

inline lsplit_result lsplit(char* *string_ref, char delim1, char delim2) {
    lsplit_result result;
	result.piece=*string_ref;
	lsplit_result next=lsplit(*string_ref, delim1, delim2);
	result.delim=next.delim;
	*string_ref=next.piece;
    return result;
}

static lsplit_result lsplit(char** string_ref, char delim1, char delim2, char encloser) {
	lsplit_result result;

    if(char* string=*string_ref) {
		if(encloser && *string==encloser) {
            string++;
			
			char *read;
			char *write;
			write=read=string;
			char c;
			while((c=*read++)) {
				if(c==encloser) {
					char n=*read;
					if(n==encloser) // double-encloser stands for encloser
						read++;
					else if(n==delim1 || n==delim2) {
						result.delim=n;
						read++;
						break;
					}
				}

				*write++=c;
			}
			*write=0; // terminate
			*string_ref=c? read: 0;
			result.piece=string;
			return result;
		} else
			return lsplit(string_ref, delim1, delim2);
    }
	result.piece=0;
    return result;
}

static void skip_empty_and_comment_lines( char** data_ref ) {
	if(char *data=*data_ref) {
		while( char c=*data ) {
			if( c== '\n' || c == '#' ) {
				/*nowhere=*/getrow(&data); // remove empty&comment lines
				if(!(*data_ref=data))
					break;
				continue;
			}
			break;
		}
	}
}

static void _load(Request& r, MethodParams& params) {
	const String&  first_param=params.as_string(0, FILE_NAME_MUST_BE_STRING);
	int filename_param_index=0;
	bool nameless=first_param=="nameless";
	if(nameless)
		filename_param_index++;
	size_t options_param_index=filename_param_index+1;

	HashStringValue *options=0;
	TableSeparators separators;
	if(options_param_index<params.count()
		&& (options=params.as_no_junction(options_param_index, "additional params must be hash").get_hash())) {
		// cloning, so that we could change [to simplify checks on params inside file_read_text
		options=new HashStringValue(*options);
		separators.load(*options);
	}

	// loading text
	char *data=file_load_text(r,
		r.absolute(params.as_string(filename_param_index, FILE_NAME_MUST_BE_STRING)),
		true,
		options
	);

	// parse columns
	Table::columns_type columns;
	if(nameless) {
		columns=Table::columns_type(0); // nameless
	} else {
		columns=Table::columns_type(new ArrayString);

		skip_empty_and_comment_lines(&data);
		while( lsplit_result sr=lsplit(&data, separators.column, '\n', separators.encloser) ) {
			*columns+=new String(sr.piece, String::L_TAINTED);
			if(sr.delim=='\n') 
				break;
		}
	}
	
	Table& table=*new Table(columns);
	int columns_count=columns? columns->count(): 0;

	// parse cells
	Table::element_type row(new ArrayString(columns_count));
	skip_empty_and_comment_lines(&data);
	while( lsplit_result sr=lsplit(&data, separators.column, '\n', separators.encloser) ) {
		if(!*sr.piece && !sr.delim && !row->count()) // append last empty column [if without \n]
			break;
		*row+=new String(sr.piece, String::L_TAINTED);
		if(sr.delim=='\n') {
			table+=row;
			row=new ArrayString(columns_count);
			skip_empty_and_comment_lines(&data);
		}
	}
	// last line [if without \n]
	if(row->count())
		table+=row;
	
	// replace any previous table value
	GET_SELF(r, VTable).set_table(table);
}

#ifdef NO_STRINGSTREAM

void maybe_enclose( String& to, const String& from, char encloser, const String* sencloser ) {
	if(encloser) {
		to<<*sencloser;
		// while we have 'encloser'...
		size_t pos_after=0;
		for( size_t pos_before; (pos_before=from.pos( encloser, pos_after ))!=STRING_NOT_FOUND; pos_after=pos_before) {
			pos_before++; // including first encloser (and skipping it for next pos)
			to<<from.mid(pos_after, pos_before);
			to<<*sencloser; // doubling encloser
		}
		// last piece
		size_t from_length=from.length();
		if(pos_after<from_length)
			to<<from.mid(pos_after, from_length);

		to<<*sencloser;
	} else
		to<<from;
}

#else

void maybe_enclose( ostringstream& to, const String& from, char encloser ) {
	if(encloser) {
		to<<encloser;
		// while we have 'encloser'...
		size_t pos_after=0;
		for( size_t pos_before; (pos_before=from.pos( encloser, pos_after ))!=STRING_NOT_FOUND; pos_after=pos_before) {
			pos_before++; // including first encloser (and skipping it for next pos)
            to<<from.mid(pos_after, pos_before).cstr();
			to<<encloser; // doubling encloser
		}
		// last piece
		size_t from_length=from.length();
		if(pos_after<from_length)
			to<<from.mid(pos_after, from_length).cstr();

		to<<encloser;
	} else
		to<<from.cstr();
}

#endif

static void _save(Request& r, MethodParams& params) {
	const String& first_arg=params.as_string(0, FIRST_ARG_MUST_NOT_BE_CODE);
	size_t param_index=1;

	bool do_append=false;
	bool output_column_names=true;

	// mode?
	if(first_arg=="append")
		do_append=true;
	else if(first_arg=="nameless")
		output_column_names=false;
	else
		--param_index;

	const String& file_name=params.as_string(param_index++, FILE_NAME_MUST_NOT_BE_CODE);
	String file_spec=r.absolute(file_name);

	if(do_append && file_exist(file_spec))
		output_column_names=false;

	TableSeparators separators;
	if(param_index<params.count()) {
		Value& voptions=params.as_no_junction(param_index++, "additional params must be hash");
		if( voptions.is_defined() && !voptions.is_string() ) {
			if(HashStringValue* options=voptions.get_hash()) {
				int valid_options=separators.load(*options);
				if(valid_options!=options->count())
					throw Exception(PARSER_RUNTIME,
						0,
						"invalid option passed");
			} else {
				throw Exception(PARSER_RUNTIME,
					0,
					"additional params must be hash (did you spell mode parameter correctly?)");
			}
		}
	}
	if(param_index<params.count())
		throw Exception(PARSER_RUNTIME,
			0,
			"bad mode (must be nameless or append)");

	Table& table=GET_SELF(r, VTable).table();

#ifdef NO_STRINGSTREAM

	String sdata;
	if(output_column_names) {
		if(table.columns()) { // named table
			for(Array_iterator<const String*> i(*table.columns()); i.has_next(); ) {
				maybe_enclose( sdata, *i.next(), separators.encloser, separators.sencloser );
				if(i.has_next())
					sdata<<*separators.scolumn;
			}
		} else { // nameless table [we were asked to output column names]
			if(int lsize=table.count()?table[0]->count():0)
				for(int column=0; column<lsize; column++) {
					char *cindex_tab=new(PointerFreeGC) char[MAX_NUMBER];
					sdata.append_know_length(cindex_tab, 
						snprintf(cindex_tab, MAX_NUMBER, 
							column<lsize-1?"%d%c":"%d", column, separators.column),
							String::L_CLEAN);
				}
			else
				sdata.append_help_length("empty nameless table", 0, String::L_CLEAN);
		}
		sdata.append_know_length("\n", 1, String::L_CLEAN);
	}

	// data lines
	Array_iterator<ArrayString*> i(table);
	while(i.has_next()) {
		for(Array_iterator<const String*> c(*i.next()); c.has_next(); ) {
			maybe_enclose( sdata, *c.next(), separators.encloser, separators.sencloser );
			if(c.has_next())
				sdata<<*separators.scolumn;
		}
		sdata.append_know_length("\n", 1, String::L_CLEAN);
	}

	// write
	{
		const char* data_cstr=sdata.cstr();
		file_write(file_spec, 
			data_cstr, sdata.length(), true, do_append);
		if(*data_cstr) // not empty (when empty it's not heap memory)
			pa_free((void*)data_cstr); // not needed anymore
	}

#else

	ostringstream ost(stringstream::out);

	// process header
	if(output_column_names) {
		if(table.columns()) { // named table
			for(Array_iterator<const String*> i(*table.columns()); i.has_next(); ) {
				maybe_enclose( ost, *i.next(), separators.encloser );
				if(i.has_next()){
					ost<<separators.column;
				}
			}
		} else { // nameless table [we were asked to output column names]
			if(int lsize=table.count()?table[0]->count():0)
				for(int column=0; column<lsize; column++) {
					if(separators.encloser) {
						ost<<separators.encloser<<column<<separators.encloser;
					} else {
						ost<<column;
					}
					if(column<lsize-1){
						ost<<separators.column;
					}
				}
			else
				ost<<"empty nameless table";
		}
		ost<<'\n';
	}

	// process data lines
	Array_iterator<ArrayString*> i(table);
	while(i.has_next()) {
		for(Array_iterator<const String*> c(*i.next()); c.has_next(); ) {
			maybe_enclose( ost, *c.next(), separators.encloser );
			if(c.has_next())
				ost<<separators.column;
		}
		ost<<'\n';
	}

	// write
	{
		string data=ost.str();
		const char* data_cstr=data.c_str();

		file_write(file_spec, data_cstr, data.length(), true /* as text */, do_append);
	}

#endif
}

static void _count(Request& r, MethodParams&) {
	int result=GET_SELF(r, VTable).table().count();
	r.write_no_lang(*new VInt(result));
}

static void _line(Request& r, MethodParams&) {
	int result=1+GET_SELF(r, VTable).table().current();
	r.write_no_lang(*new VInt(result));
}

static void _offset(Request& r, MethodParams& params) {
	Table& table=GET_SELF(r, VTable).table();
	if(params.count()) {
		bool absolute=false;
		if(params.count()>1) {
		    const String&  whence=params.as_string(0, "whence must be string");
		    if(whence=="cur")
				absolute=false;
		    else if(whence=="set")
				absolute=true;
		    else
				throw Exception(PARSER_RUNTIME,
					&whence,
					"is invalid whence, valid are 'cur' or 'set'");
		}
		
		int offset=params.as_int(params.count()-1, "offset must be expression", r);
		table.offset(absolute, offset);
	} else
		r.write_no_lang(*new VInt(table.current()));
}

static void _menu(Request& r, MethodParams& params) {
	Temp_hash_value<const String::Body, void*> 
		cycle_data_setter(r.classes_conf, cycle_data_name, /*any not null flag*/&r);

	Value& body_code=params.as_junction(0, "body must be code");
	
	Value* delim_maybe_code=params.count()>1?&params[1]:0;

	Table& table=GET_SELF(r, VTable).table();
	int saved_current=table.current();
	int size=table.count();

	if(delim_maybe_code) { // delimiter set
		bool need_delim=false;
		for(int row=0; row<size; row++) {
			table.set_current(row);

			StringOrValue sv_processed=r.process(body_code);
			Request::Skip lskip=r.get_skip(); r.set_skip(Request::SKIP_NOTHING);

			const String* s_processed=sv_processed.get_string();
			if(s_processed && !s_processed->is_empty()) { // we have body
				if(need_delim) // need delim & iteration produced string?
					r.write_pass_lang(r.process(*delim_maybe_code));
				else
					need_delim=true;
			}

			r.write_pass_lang(sv_processed);

			if(lskip==Request::SKIP_BREAK)
				break;
		}
	} else {
		for(int row=0; row<size; row++) {
			table.set_current(row);
 
			r.process_write(body_code);
			Request::Skip lskip=r.get_skip(); r.set_skip(Request::SKIP_NOTHING);
 
			if(lskip==Request::SKIP_BREAK)
				break;
		}
	}
	table.set_current(saved_current);
}

#ifndef DOXYGEN
struct Row_info {
	Request *r;
	Table *table;
	Value* key_code;
	size_t key_field;
	Array<int>* value_fields;
	HashStringValue* hash;
	Table2hash_distint distinct;
	size_t row;
	Table2hash_value_type value_type;
};
#endif
static void table_row_to_hash(Table::element_type row, Row_info *info) {
	const String* key;
	if(info->key_code) {
		info->table->set_current(info->row++); // change context row
		StringOrValue sv_processed=info->r->process(*info->key_code);
		key=&sv_processed.as_string();
	} else {
		key=info->key_field<row->count()?row->get(info->key_field):0;
	}

	if(!key)
		return; // ignore rows without key [too-short-record_array if-indexed]
		
	bool exist=false;
	switch(info->value_type) {
		case C_STRING: {
			exist=info->hash->put_dont_replace(*key, new VString(*row->get(info->value_fields->get(0))));
			break;
		}
		case C_HASH: {
			VHash* vhash=new VHash;
			HashStringValue& hash=vhash->hash();
			for(Array_iterator<int> i(*info->value_fields); i.has_next(); ) {
				size_t value_field=i.next();
				if(value_field<row->count())
					hash.put(
						*info->table->columns()->get(value_field), 
						new VString(*row->get(value_field)));
			}

			exist=info->hash->put_dont_replace(*key, vhash);
			break;
		}
		case C_TABLE: {
			VTable* vtable=(VTable*)info->hash->get(*key); // table exist?
			Table* table;
			if(vtable) {
				if(info->distinct==D_ILLEGAL) {
					exist=true;
					break;
				}
				table=vtable->get_table();
			} else {
				// no? creating table of same structure as source
				Table::Action_options table_options(0, 0);
				table=new Table(*info->table, table_options/*no rows, just structure*/);
				info->hash->put(*key, new VTable(table));
			}
			*table+=row;
			break;
		}
	}
	if(exist && info->distinct==D_ILLEGAL)
		throw Exception(PARSER_RUNTIME,
			key,
			"duplicate key");
}

Table2hash_value_type get_value_type(Value& vvalue_type){
	if(vvalue_type.is_string()) {
		const String& svalue_type=*vvalue_type.get_string();
		if(svalue_type == "table"){
			return C_TABLE;
		} else if (svalue_type == "string") {
			return C_STRING;
		} else if (svalue_type == "hash") {
			return C_HASH;
		} else {
			throw Exception(PARSER_RUNTIME,
				&svalue_type,
				"must be 'hash', 'table' or 'string'");
		}
	} else {
		throw Exception(PARSER_RUNTIME,
			0,
			"'type' must be hash");
	}
}

static void _hash(Request& r, MethodParams& params) {
	Table& self_table=GET_SELF(r, VTable).table();
	VHash& result=*new VHash;
	if(Table::columns_type columns=self_table.columns()){
		if(columns->count()>0) {
			Table2hash_distint distinct=D_ILLEGAL;
			Table2hash_value_type value_type=C_HASH;
			int param_index=params.count()-1;
			if(param_index>0) {
				if(HashStringValue* options=params.as_no_junction(param_index, PARAM_MUST_NOT_BE_CODE).get_hash()){ // options where specified
					--param_index;
					int valid_options=0;
					if(Value* vdistinct_code=options->get(sql_distinct_name)) { // $.distinct ?
						valid_options++;
						Value& vdistinct_value=r.process_to_value(*vdistinct_code);
						if(vdistinct_value.is_string()) {
							const String& sdistinct=*vdistinct_value.get_string();
							if(sdistinct=="tables") {
								value_type=C_TABLE;
								distinct=D_FIRST;
							} else {
								throw Exception(PARSER_RUNTIME,
									&sdistinct,
									"must be 'tables' or true/false");
							}
						} else {
							distinct=vdistinct_value.as_bool()?D_FIRST:D_ILLEGAL;
						}
					}
					if(Value* vvalue_type_code=options->get(sql_value_type_name)) { // $.type ?
						if(value_type==C_TABLE) // $.distinct[tables] already was specified
							throw Exception(PARSER_RUNTIME,
								0,
								"you can't specify $.distinct[tables] and $.type[] together");

						valid_options++;
						value_type=get_value_type(r.process_to_value(*vvalue_type_code));
					}

					if(valid_options!=options->count())
						throw Exception(PARSER_RUNTIME,
							0,
							"called with invalid option");
				}
			}

			if(param_index==2) // options was specified but not as hash
				throw Exception(PARSER_RUNTIME,
					0,
					"options must be hash");

			Array<int> value_fields;
			if(param_index==0){ // list of columns wasn't specified
				if(value_type==C_STRING) // $.type[string]
					throw Exception(PARSER_RUNTIME,
						0,
						"you must specify one value field with option $.type[string]");
				
				for(size_t i=0; i<columns->count(); i++) // by all columns, including key
					value_fields+=i;

			} else { // list of columns was specified
				if(value_type==C_TABLE)
					throw Exception(PARSER_RUNTIME,
						0,
						"you can't specify value field(s) with option $.distinct[tables] or $.type[tables]");

				Value& value_fields_param=params.as_no_junction(param_index, "value field(s) must not be code");
				if(value_fields_param.is_string()) { // one column as string was specified
					value_fields+=self_table.column_name2index(*value_fields_param.get_string(), true);
				} else if(Table* value_fields_table=value_fields_param.get_table()) { // list of columns were specified in table
					for(Array_iterator<Table::element_type> i(*value_fields_table); i.has_next(); ) {
						const String& value_field_name =*i.next()->get(0);
						value_fields +=self_table.column_name2index(value_field_name, true);
					}
				} else
					throw Exception(PARSER_RUNTIME,
						0,
						"value field(s) must be string or table");
			}

			if(value_type==C_STRING && value_fields.count()!=1)
				throw Exception(PARSER_RUNTIME,
					0,
					"you can specify only one value field with option $.type[string]");

			{
				Value* key_param=&params[0];
				Row_info info={
					&r,
					&self_table,
					/*key_code=*/key_param->get_junction()?key_param:0,
					/*key_field=*/0/*filled below*/,
					&value_fields,
					&result.hash(),
					distinct,
					/*row=*/0,
					value_type
				};
				info.key_field=(info.key_code?-1
						:self_table.column_name2index(key_param->as_string(), true));

				int saved_current=self_table.current();
				self_table.for_each(table_row_to_hash, &info);
				self_table.set_current(saved_current);

				result.extract_default();
			}
		}
	}
	r.write_no_lang(result);
}

#ifndef DOXYGEN
struct Table_seq_item {
	ArrayString* row;
	union {
		const char *c_str;
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
static void _sort(Request& r, MethodParams& params) {
	Value& key_maker=params.as_junction(0, "key-maker must be code");

	bool reverse=params.count()>1/*..[desc|asc|]*/?
		reverse=params.as_no_junction(1, "order must not be code").as_string()=="desc":
		false; // default=asc

	Table& old_table=GET_SELF(r, VTable).table();
	Table& new_table=*new Table(old_table.columns());

	Table_seq_item* seq=new(PointerFreeGC) Table_seq_item[old_table.count()];
	int i;

	// calculate key values
	bool key_values_are_strings=true;
	int old_count=old_table.count();
	for(i=0; i<old_count; i++) {
		old_table.set_current(i);
		// calculate key value
		seq[i].row=old_table[i];
		Value& value=r.process_to_value(key_maker).as_expr_result(true/*return string as-is*/);
		if(i==0) // determining key values type by first one
			key_values_are_strings=value.is_string();

		if(key_values_are_strings)
			seq[i].value.c_str=value.as_string().cstr();
		else
			seq[i].value.d=value.as_double();
	}
	// sort keys
	_qsort(seq, old_count, sizeof(Table_seq_item), 
		key_values_are_strings?sort_cmp_string:sort_cmp_double);

	// reorder table as they require in 'seq'
	for(i=0; i<old_count; i++)
		new_table+=Table::element_type(seq[reverse?old_count-1-i:i].row);

	delete[] seq;

	// replace any previous table value
	GET_SELF(r, VTable).set_table(new_table);
}

#ifndef DOXYGEN
struct Expression_is_true_info {
	Request* r;
	Value* expression_code;
};
#endif
static bool expression_is_true(Table&, Expression_is_true_info* info) {
	return info->r->process_to_value(*info->expression_code).as_bool();
}
static bool _locate_expression(Table& table, Table::Action_options o,
			       Request& r, MethodParams& params) {
	check_option_param(o.defined, params, 1,
		"locate by expression only has parameters: expression and, maybe, options");
	Value& expression_code=params.as_junction(0, "must be expression");

	Expression_is_true_info info={&r, &expression_code};
	return table.table_first_that(expression_is_true, &info, o);
}
static bool _locate_name_value(Table& table, Table::Action_options o,
			       Request&, MethodParams& params) {
	check_option_param(o.defined, params, 2,
		"locate by locate by name has parameters: name, value and, maybe, options");
	const String& name=params.as_string(0, "column name must be string");
	const String& value=params.as_string(1, VALUE_MUST_BE_STRING);
	return table.locate(name, value, o);
}
static void _locate(Request& r, MethodParams& params) {
	Table& table=GET_SELF(r, VTable).table();

	Table::Action_options o=get_action_options(r, params, table);

	bool result=params[0].get_junction()?
		_locate_expression(table, o, r, params) :
		_locate_name_value(table, o, r, params);
	r.write_no_lang(VBool::get(result));
}


static void _flip(Request& r, MethodParams&) {
	Table& old_table=GET_SELF(r, VTable).table();
	Table& new_table=*new Table(0);
	if(size_t old_count=old_table.count())
		if(size_t old_cols=old_table[0]->count()) 
			for(size_t column=0; column<old_cols; column++) {
				Table::element_type new_row(new ArrayString(old_count));
				for(size_t i=0; i<old_count; i++) {
					Table::element_type old_row=old_table[i];
					*new_row+=column<old_row->count()?old_row->get(column):new String;
				}
				new_table+=new_row;
			}

	r.write_no_lang(*new VTable(&new_table));
}

static void _append(Request& r, MethodParams& params) {
	// data
	Temp_lang temp_lang(r, String::L_PASS_APPENDED);
	const String& string=r.process_to_string(params.as_junction(0, "body must be code"));

	// parse cells
	Table::element_type row=new ArrayString;
	size_t pos_after=0;
	string.split(*row, pos_after, "\t", String::L_AS_IS);

	GET_SELF(r, VTable).table()+=row;
}

static void join_named_row(Table& src, Table* dest) {
	Table::columns_type dest_columns=dest->columns();
	size_t dest_columns_count=dest_columns->count();
	Table::element_type dest_row(new ArrayString(dest_columns_count));
	for(size_t dest_column=0; dest_column<dest_columns_count; dest_column++) {
		const String* src_item=src.item(*dest_columns->get(dest_column));
		*dest_row+=src_item?src_item:new String;
	}
	*dest+=dest_row;
}
static void join_nameless_row(Table& src, Table* dest) {
	*dest+=src[src.current()];
}
static void _join(Request& r, MethodParams& params) {
	Table* maybe_src=params.as_no_junction(0, "table ref must not be code").get_table();
	if(!maybe_src)
		throw Exception(PARSER_RUNTIME, 
			0, 
			"source is not a table");
	Table& src=*maybe_src;

	Table::Action_options o=get_action_options(r, params, src);
	check_option_param(o.defined, params, 1,
		"invalid extra parameter");

	Table& dest=GET_SELF(r, VTable).table();
	if(&src == &dest)
		throw Exception(PARSER_RUNTIME, 
			0, 
			"source and destination are same table");

	if(dest.columns()) // dest is named
		src.table_for_each(join_named_row, &dest, o);
	else // dest is nameless
		src.table_for_each(join_nameless_row, &dest, o);
}

#ifndef DOXYGEN
class Table_sql_event_handlers: public SQL_Driver_query_event_handlers {
	ArrayString& columns;
	int columns_count;
	ArrayString* row;
public:
	Table* table;
public:
	Table_sql_event_handlers() :
		columns(*new ArrayString), row(0), table(0) {
	}

	bool add_column(SQL_Error& error, const char *str, size_t) {
		try {
			columns+=new String(str, String::L_TAINTED);
			return false;
		} catch(...) {
			error=SQL_Error("exception occured in Table_sql_event_handlers::add_column");
			return true;
		}
	}
	bool before_rows(SQL_Error& error) { 
		try {
			table=new Table(&columns);
			columns_count=columns.count();
			return false;
		} catch(...) {
			error=SQL_Error("exception occured in Table_sql_event_handlers::before_rows");
			return true;
		}
	}
	bool add_row(SQL_Error& error) {
		try {
			*table+=row=new ArrayString(columns_count);
			return false;
		} catch(...) {
			error=SQL_Error("exception occured in Table_sql_event_handlers::add_row");
			return true;
		}
	}
	bool add_row_cell(SQL_Error& error, const char* str, size_t) {
		try {
			*row+=new String(str, String::L_TAINTED);
			return false;
		} catch(...) {
			error=SQL_Error("exception occured in Table_sql_event_handlers::add_row_cell");
			return true;
		}
	}
};
#endif

static void marshal_bind(
						 HashStringValue::key_type aname, 
						 HashStringValue::value_type avalue,
						 SQL_Driver::Placeholder** pptr) 
{
	SQL_Driver::Placeholder& ph=**pptr;
	ph.name=aname.cstr();
	ph.value=avalue->as_string().cstr(String::L_UNSPECIFIED);
	ph.is_null=avalue->get_class()==void_class;
	ph.were_updated=false;

	(*pptr)++;
}

// not static, used elsewhere
int marshal_binds(HashStringValue& hash, SQL_Driver::Placeholder*& placeholders) {
	int hash_count=hash.count();
	placeholders=new(UseGC) SQL_Driver::Placeholder[hash_count];
	SQL_Driver::Placeholder* ptr=placeholders;
	hash.for_each<SQL_Driver::Placeholder**>(marshal_bind, &ptr);
	return hash_count;
}

// not static, used elsewhere
void unmarshal_bind_updates(HashStringValue& hash, int placeholder_count, SQL_Driver::Placeholder* placeholders) {
	SQL_Driver::Placeholder* ph=placeholders;
	for(int i=0; i<placeholder_count; i++, ph++)
		if(ph->were_updated) {
			Value* value;
			if(ph->is_null)
				value=VVoid::get();
			else
				value=new VString(*new String(ph->value, String::L_TAINTED));
			hash.put(ph->name, value);
		}
}

static void _sql(Request& r, MethodParams& params) {
	Value& statement=params.as_junction(0, "statement must be code");

	HashStringValue* bind=0;
	ulong limit=SQL_NO_LIMIT;
	ulong offset=0;
	if(params.count()>1) {
		Value& voptions=params.as_no_junction(1, "options must be hash, not code");
		if(voptions.is_defined() && !voptions.is_string())
			if(HashStringValue* options=voptions.get_hash()) {
				int valid_options=0;
				if(Value* vbind=options->get(sql_bind_name)) {
					valid_options++;
					bind=vbind->get_hash();
				}
				if(Value* vlimit=options->get(sql_limit_name)) {
					valid_options++;
					limit=(ulong)r.process_to_value(*vlimit).as_double();
				}
				if(Value* voffset=options->get(sql_offset_name)) {
					valid_options++;
					offset=(ulong)r.process_to_value(*voffset).as_double();
				}
				if(valid_options!=options->count())
					throw Exception(PARSER_RUNTIME,
						0,
						"called with invalid option");
			} else
				throw Exception(PARSER_RUNTIME,
					0,
					"options must be hash");
	}

	SQL_Driver::Placeholder* placeholders=0;
	uint placeholders_count=0;
	if(bind)
		placeholders_count=marshal_binds(*bind, placeholders);

	Temp_lang temp_lang(r, String::L_SQL);
	const String&  statement_string=r.process_to_string(statement);
	const char* statement_cstr=
		statement_string.cstr(String::L_UNSPECIFIED, r.connection());
	Table_sql_event_handlers handlers;
#ifdef RESOURCES_DEBUG
	struct timeval mt[2];
	//measure:before
	gettimeofday(&mt[0],NULL);
#endif	
	r.connection()->query(
		statement_cstr, 
		placeholders_count, placeholders,
		offset, limit, 
		handlers,
		statement_string);
	
#ifdef RESOURCES_DEBUG
		//measure:after connect
	gettimeofday(&mt[1],NULL);
	
	double t[2];
	for(int i=0;i<2;i++)
	    t[i]=mt[i].tv_sec+mt[i].tv_usec/1000000.0;
	    
	r.sql_request_time+=t[1]-t[0];
#endif	    			

	if(bind)
		unmarshal_bind_updates(*bind, placeholders_count, placeholders);

	Table& result=
		handlers.table?*handlers.table: // query resulted in table? return it
		*new Table(Table::columns_type(0)); // query returned no table, fake it

	// replace any previous table value
	GET_SELF(r, VTable).set_table(result);
}

static void _columns(Request& r, MethodParams& params) {
	const String* column_column_name;
	if(params.count()>0)
		column_column_name=&params.as_string(0, COLUMN_NAME_MUST_BE_STRING);
	else 
		column_column_name=new String("column");

	Table::columns_type result_columns(new ArrayString);
	*result_columns+=column_column_name;
	Table& result_table=*new Table(result_columns);

	Table& source_table=GET_SELF(r, VTable).table();
	if(Table::columns_type source_columns=source_table.columns()) {
		for(Array_iterator<const String*> i(*source_columns); i.has_next(); ) {
			Table::element_type result_row(new ArrayString);
			*result_row+=i.next();
			result_table+=result_row;
		}
	}

	r.write_no_lang(*new VTable(&result_table));
}

static void _select(Request& r, MethodParams& params) {
	Value& vcondition=params.as_expression(0, "condition must be number, bool or expression");

	Table& source_table=GET_SELF(r, VTable).table();
	Table& result_table=*new Table(source_table.columns());

	int saved_current=source_table.current();
	int size=source_table.count();
	for(int row=0; row<size; row++) {
		source_table.set_current(row);

		bool condition=r.process_to_value(vcondition, 
				false/*don't intercept string*/).as_bool();

		if(condition) // ...condition is true=
			result_table+=source_table[row]; // =green light to go to result
	}
	source_table.set_current(saved_current);

	r.write_no_lang(*new VTable(&result_table));
}

// constructor

MTable::MTable(): Methoded("table") {
	// ^table::create{data}
	// ^table::create[nameless]{data}
	// ^table::create[table]
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 3);
	// old name for compatibility with <= v 1.141 2002/01/25 11:33:45 paf
	add_native_method("set", Method::CT_DYNAMIC, _create, 1, 3); 

	// ^table::load[file]  
	// ^table::load[nameless;file]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 3);

	// ^table.save[file]  
	// ^table.save[nameless;file]
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 3);

	// add_native_method("save_old", Method::CT_DYNAMIC, _save_old, 1, 3);

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

	// ^table.hash[key field name]
	// ^table.hash[key field name][value field name(s) string/table]
	add_native_method("hash", Method::CT_DYNAMIC, _hash, 1, 3);

	// ^table.sort{string-key-maker} ^table.sort{string-key-maker}[desc|asc]
	// ^table.sort(numeric-key-maker) ^table.sort(numeric-key-maker)[desc|asc]
	add_native_method("sort", Method::CT_DYNAMIC, _sort, 1, 2);

	// ^table.locate[field;value]
	add_native_method("locate", Method::CT_DYNAMIC, _locate, 1, 3);

	// ^table.flip[]
	add_native_method("flip", Method::CT_DYNAMIC, _flip, 0, 0);

	// ^table.append{r{tab}e{tab}c{tab}o{tab}r{tab}d}
	add_native_method("append", Method::CT_DYNAMIC, _append, 1, 1);

	// ^table.join[table][$.limit(10) $.offset(1) $.offset[cur] ]
	add_native_method("join", Method::CT_DYNAMIC, _join, 1, 2);


	// ^table::sql[query]
	// ^table::sql[query][$.limit(1) $.offset(2)]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// ^table.columns[[column name]]
	add_native_method("columns", Method::CT_DYNAMIC, _columns, 0, 1);

	// ^table.select(expression) = table
	add_native_method("select", Method::CT_DYNAMIC, _select, 1, 1);
}
