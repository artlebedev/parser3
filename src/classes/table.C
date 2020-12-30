/** @file
	Parser: @b table parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#if (!defined(NO_STRINGSTREAM) && !defined(FREEBSD4) && !defined(PA_DEBUG_DISABLE_GC))
#include <sstream>
#include "../lib/gc/include/gc_allocator.h"
#define USE_STRINGSTREAM
#endif

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_charsets.h"
#include "pa_vtable.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"
#include "pa_vbool.h"
#include "pa_array.h"

volatile const char * IDENT_TABLE_C="$Id: table.C,v 1.358 2020/12/30 10:16:53 moko Exp $";

// class

class MTable: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VTable(); }
public:
	MTable();
};

// global variable

DECLARE_CLASS_VAR(table, new MTable);

// globals

#define TABLE_REVERSE_NAME "reverse"
String table_reverse_name(TABLE_REVERSE_NAME);

// methods

static Table::Action_options get_action_options(Request& r, MethodParams& params, size_t options_index, const Table& source) {
	Table::Action_options result;
	if(params.count() <= options_index)
		return result;

	HashStringValue* options=params.as_hash(options_index);
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
				throw Exception(PARSER_RUNTIME, &soffset, "must be 'cur' string or expression");
		} else 
			result.offset=r.process(*voffset).as_int();
	}
	if(Value* vlimit=options->get(sql_limit_name)) {
		valid_options++;
		result.limit=r.process(*vlimit).as_int();
	}
	if(Value *vreverse=(Value *)options->get(table_reverse_name)) { 
		valid_options++; 
		result.reverse=r.process(*vreverse).as_bool(); 
		if(result.reverse && !defined_offset) 
			result.offset=source.count()-1; 
	} 

	if(valid_options!=options->count())
		throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);

	return result;
}

struct TableControlChars {
	char separator;  const String* sseparator;
	char encloser; const String* sencloser;

	char separators[3];

	TableControlChars():
		separator('\t'), sseparator(new String("\t")),
		encloser(0), sencloser(0)
	{
		strcpy(separators,"\t\n");
	}

	int load( HashStringValue& options ) {
		int result=0;
		if(Value* vseparator=options.get(PA_COLUMN_SEPARATOR_NAME)) {
			sseparator=&vseparator->as_string();
			if(sseparator->length()!=1)
				throw Exception(PARSER_RUNTIME, sseparator, "separator must be one byte character");
			separator=sseparator->first_char();
			separators[0]=separator;
			result++;
		}
		if(Value* vencloser=options.get(PA_COLUMN_ENCLOSER_NAME)) {
			sencloser=&vencloser->as_string();
			if(sencloser->is_empty()){
				encloser=0;
			} else {
				if(sencloser->length()!=1)
					throw Exception(PARSER_RUNTIME, sencloser, "encloser must be empty or one byte character");
				encloser=sencloser->first_char();
			}
			result++;
		}
		return result;
	}
};


struct lsplit_sresult {
	String* piece;
	char delim;

	lsplit_sresult() : piece(0), delim(0){}

	operator bool() { return piece!=0; }

	void append(String *str){
		if(piece)
			*piece << *str;
		else
			piece = str;
	}
};

class StringSplitHelper : public String {
public:
	char* base;

	StringSplitHelper(String astring) : String(astring), base(cstrm()) {}

	bool check_lang(const char *pos){
		return langs.check_lang(L_AS_IS, pos-base, 1);
	}

	String *extract(char *pos){
		String *result=new String;
		if(size_t len=strlen(pos)){
			// first: their langs
			result->langs.append(result->body, langs, pos-base, len);
			// next: letters themselves
			result->body=Body(pos);
		}
		return result;
	}
};

inline lsplit_sresult lsplit(char* *string_ref, const char* delims, StringSplitHelper& helper) {
	lsplit_sresult result;
	if(char *pos=*string_ref) {
		while(pos=strpbrk(pos, delims)) {
			if(helper.check_lang(pos)){
				result.delim=*pos;
				*pos=0;
				result.piece=helper.extract(*string_ref);
				*string_ref=pos+1;
				return result;
			}
			pos++;
		}
		result.piece=helper.extract(*string_ref);
		*string_ref=0;
	}
	return result;
}

static lsplit_sresult lsplit(char** string_ref, const char* delims, char encloser, StringSplitHelper& helper) {
	lsplit_sresult result;

	if(char *pos=*string_ref) {
		if(encloser && *pos==encloser && helper.check_lang(pos)) {
			*string_ref=++pos;

			// we are enclosed, searching for second encloser
			while(1) {
				if(pos=strchr(pos, encloser)){
					if(helper.check_lang(pos)){
						*(pos++)=0;
						result.append(helper.extract(*string_ref));
						if(*pos==encloser && helper.check_lang(pos)){ // double-encloser stands for encloser
							*string_ref=pos;
						} else {
							*string_ref=pos;
							break;
						}
					}
					pos++;
				} else {
					result.append(helper.extract(*string_ref));
					*string_ref=0;
					return result;
				}
			}

			// we are no longer enclosed, searching for delimiter
			while(pos=strpbrk(pos, delims)) {
				if(helper.check_lang(pos)){
					result.delim=*pos;
					if(pos>*string_ref){
						*pos=0;
						result.append(helper.extract(*string_ref));
					}
					*string_ref=pos+1;
					return result;
				}
				pos++;
			}
			result.append(helper.extract(*string_ref));
			*string_ref=0;
		} else
			return lsplit(string_ref, delims, helper);
	}
	return result;
}

static void skip_clean_empty_lines(char** data_ref, StringSplitHelper& helper) {
	if(*data_ref) {
		while(**data_ref == '\n' && helper.check_lang(*data_ref))
			(*data_ref)++;
	}
}

static void _create(Request& r, MethodParams& params) {
	// clone/copy part?
	if(Table *source=params[0].get_table()) {
		Table::Action_options o=get_action_options(r, params, 1, *source);
		if(params.count()>2)
			throw Exception(PARSER_RUNTIME, 0, "too many parameters");
		GET_SELF(r, VTable).set_table(*new Table(*source, o));
		return;
	}

	size_t data_param_index=0;
	bool nameless=false;

	if(params.count()>1) {
		if(params[0].is_string()){ // can be nameless only
			const String& snameless=params.as_string(0, "called with more then 1 param, first param may be only string 'nameless' or junction");
			if(snameless!="nameless")
				throw Exception(PARSER_RUNTIME, &snameless, "table::create called with more then 1 param, first param may be only 'nameless'");
			nameless=true;
			data_param_index++;
		}
	}

	HashStringValue *options=0;
	TableControlChars control_chars;

	size_t options_param_index=data_param_index+1;
	if( options_param_index<params.count() && (options=params.as_hash(options_param_index)) ) {
		if(control_chars.load(*options)!=options->count())
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
	}

	// data
	StringSplitHelper sdata(r.process_to_string(params.as_junction(data_param_index, "body must be table or code")));
	char *data=sdata.base;

	// parse columns
	Table::columns_type columns;
	if(nameless) {
		columns=0; // nameless
	} else {
		columns=new ArrayString;
		while( lsplit_sresult sr=lsplit(&data, control_chars.separators, control_chars.encloser, sdata) ) {
			*columns+=sr.piece;
			if(sr.delim=='\n') 
				break;
		}
	}
	
	Table& table=*new Table(columns);
	int columns_count=columns ? columns->count(): 0;

	// parse cells
	Table::element_type row(new ArrayString(columns_count));
	skip_clean_empty_lines(&data, sdata);
	while( lsplit_sresult sr=lsplit(&data, control_chars.separators, control_chars.encloser, sdata) ) {
		if(sr.piece->is_empty() && !sr.delim && !row->count()) // append last empty column [if without \n]
			break;
		*row+=sr.piece;
		if(sr.delim=='\n') {
			table+=row;
			row=new ArrayString(columns_count);
			skip_clean_empty_lines(&data, sdata);
		}
	}
	// last line [if without \n]
	if(row->count())
		table+=row;
	
	// replace any previous table value
	GET_SELF(r, VTable).set_table(table);
}

struct lsplit_result {
	char* piece;
	char delim;

	lsplit_result(char *apiece=0) : piece(apiece), delim(0){}
	operator bool() { return piece!=0; }
};

inline lsplit_result lsplit(char* *string_ref, const char* delims) {
	lsplit_result result(*string_ref);
	if(result.piece) {
		if(char* v=strpbrk(result.piece, delims)) {
			result.delim=*v;
			*v=0;
			*string_ref=v+1;
			return result;
		}
		*string_ref=0;
	}
	return result;
}

static lsplit_result lsplit(char** string_ref, const char* delims, char encloser) {
	lsplit_result result(*string_ref);

	if(result.piece) {
		if(encloser && *result.piece==encloser) {
			result.piece++;
			
			char c;
			char *read;
			char *write;
			write=read=result.piece;

			// we are enclosed, searching for second encloser
			while(c=*read++) {
				if(c==encloser) {
					if(*read==encloser) // double-encloser stands for encloser
						read++;
					else
						break; // note: skipping encloser
				}
				*write++=c;
			}

			// we are no longer enclosed, searching for delimiter
			while(c=*read++) {
				if(c==delims[0] || c==delims[1]) {
					result.delim=c;
					break;
				} else 
					*write++=c;
			}

			*write=0; // terminate
			*string_ref=c ? read : 0;
			return result;
		} else
			return lsplit(string_ref, delims);
	}
	return result;
}

static void skip_empty_and_comment_lines( char** data_ref ) {
	while(*data_ref) {
		if(**data_ref == '\n'){
			(*data_ref)++;
		} else {
			if(**data_ref == '#' )
				/*nowhere=*/getrow(data_ref);
			else
				break;
		}
	}
}

static void skip_empty_lines( char** data_ref ) {
	if(*data_ref) {
		while(**data_ref == '\n')
			(*data_ref)++;
	}
}

typedef void (*Skip_lines_action)(char** data_ref);

static void _load(Request& r, MethodParams& params) {
	const String&  first_param=params.as_string(0, FILE_NAME_MUST_BE_STRING);
	int filename_param_index=0;
	bool nameless=first_param=="nameless";
	if(nameless)
		filename_param_index++;
	size_t options_param_index=filename_param_index+1;

	HashStringValue *options=0;
	TableControlChars control_chars;
	if(options_param_index<params.count() && (options=params.as_hash(options_param_index)))
		control_chars.load(*options);

	// loading text
	char *data=file_load_text(r, r.full_disk_path(params.as_string(filename_param_index, FILE_NAME_MUST_BE_STRING)), true, options);

	Skip_lines_action skip_lines_action = (control_chars.separator=='#' || control_chars.encloser=='#') ? skip_empty_lines : skip_empty_and_comment_lines;

	// parse columns
	Table::columns_type columns;
	if(nameless) {
		columns=0; // nameless
	} else {
		columns=new ArrayString;

		skip_lines_action(&data);
		while( lsplit_result sr=lsplit(&data, control_chars.separators, control_chars.encloser) ) {
			*columns+=new String(sr.piece, String::L_TAINTED);
			if(sr.delim=='\n') 
				break;
		}
	}
	
	Table& table=*new Table(columns);
	int columns_count=columns ? columns->count(): 0;

	// parse cells
	Table::element_type row(new ArrayString(columns_count));
	skip_lines_action(&data);
	while( lsplit_result sr=lsplit(&data, control_chars.separators, control_chars.encloser) ) {
		if(!*sr.piece && !sr.delim && !row->count()) // append last empty column [if without \n]
			break;
		*row+=new String(sr.piece, String::L_TAINTED);
		if(sr.delim=='\n') {
			table+=row;
			row=new ArrayString(columns_count);
			skip_lines_action(&data);
		}
	}
	// last line [if without \n]
	if(row->count())
		table+=row;
	
	// replace any previous table value
	GET_SELF(r, VTable).set_table(table);
}

#ifdef USE_STRINGSTREAM

typedef std::basic_stringstream<char, std::char_traits<char>, gc_allocator<char> > pa_stringstream;
typedef std::basic_string<char, std::char_traits<char>, gc_allocator<char> > pa_string;

static void enclose( pa_stringstream& to, const String* from, char encloser ) {
	if(from){
		to<<encloser;
		// while we have 'encloser'...
		size_t pos_after=0;
		for( size_t pos_before; (pos_before=from->pos( encloser, pos_after ))!=STRING_NOT_FOUND; pos_after=pos_before) {
			pos_before++; // including first encloser (and skipping it for next pos)
			to<<from->mid(pos_after, pos_before).cstr();
			to<<encloser; // doubling encloser
		}
		// last piece
		size_t from_length=from->length();
		if(pos_after<from_length)
			to<<from->mid(pos_after, from_length).cstr();
		to<<encloser;
	} else {
		to<<encloser<<encloser;
	}
}

static void table_to_csv(pa_stringstream& result, Table& table, TableControlChars& control_chars, bool output_column_names) {
	if(output_column_names) {
		if(table.columns()) { // named table
			if(control_chars.encloser){
				for(Array_iterator<const String*> i(*table.columns()); i.has_next(); ) {
					enclose( result, i.next(), control_chars.encloser );
					if(i.has_next())
						result<<control_chars.separator;
				}
			} else {
				for(Array_iterator<const String*> i(*table.columns()); i.has_next(); ) {
					result<<i.next()->cstr();
					if(i.has_next())
						result<<control_chars.separator;
				}
			}
		} else { // nameless table [we were asked to output column names]
			if(int lsize=table.count()?table[0]->count():0)
				for(int column=0; column<lsize; column++) {
					if(control_chars.encloser) {
						result<<control_chars.encloser<<column<<control_chars.encloser;
					} else {
						result<<column;
					}
					if(column<lsize-1){
						result<<control_chars.separator;
					}
				}
			else
				result<<"empty nameless table";
		}
		result<<'\n';
	}

	// process data lines
	Array_iterator<ArrayString*> i(table);
	if(control_chars.encloser){
		while(i.has_next()) {
			for(Array_iterator<const String*> c(*i.next()); c.has_next(); ) {
				enclose( result, c.next(), control_chars.encloser );
				if(c.has_next())
					result<<control_chars.separator;
			}
			result<<'\n';
		}
	} else {
		while(i.has_next()) {
			for(Array_iterator<const String*> c(*i.next()); c.has_next(); ) {
				result<<c.next()->cstr();
				if(c.has_next())
					result<<control_chars.separator;
			}
			result<<'\n';
		}
	}
}

#else

void enclose( String& to, const String* from, char encloser, const String* sencloser ) {
	if(from){
		to<<*sencloser;
		// while we have 'encloser'...
		size_t pos_after=0;
		for( size_t pos_before; (pos_before=from->pos( encloser, pos_after ))!=STRING_NOT_FOUND; pos_after=pos_before) {
			pos_before++; // including first encloser (and skipping it for next pos)
			to<<from->mid(pos_after, pos_before);
			to<<*sencloser; // doubling encloser
		}
		// last piece
		size_t from_length=from->length();
		if(pos_after<from_length)
			to<<from->mid(pos_after, from_length);
		to<<*sencloser;
	} else {
		to<<*sencloser<<*sencloser;
	}
}

static void table_to_csv(String& result, Table& table, TableControlChars& control_chars, bool output_column_names) {
	if(output_column_names) {
		if(table.columns()) { // named table
			if(control_chars.encloser) {
				for(Array_iterator<const String*> i(*table.columns()); i.has_next(); ) {
					enclose( result, i.next(), control_chars.encloser, control_chars.sencloser );
					if(i.has_next())
						result<<*control_chars.sseparator;
				}
			} else {
				for(Array_iterator<const String*> i(*table.columns()); i.has_next(); ) {
					result<<*i.next();
					if(i.has_next())
						result<<*control_chars.sseparator;
				}
			}
		} else { // nameless table [we were asked to output column names]
			if(int lsize=table.count()?table[0]->count():0)
				for(int column=0; column<lsize; column++) {
					if(control_chars.encloser) {
						result<<*control_chars.sencloser<<String::Body::Format(column)<<*control_chars.sencloser;
					} else {
						result<<String::Body::Format(column);
					}
					if(column<lsize-1){
						result<<*control_chars.sseparator;
					}
				}
			else
				result.append_help_length("empty nameless table", 0, String::L_CLEAN);
		}
		result.append_know_length("\n", 1, String::L_CLEAN);
	}

	// data lines
	Array_iterator<ArrayString*> i(table);
	if(control_chars.encloser){
		while(i.has_next()) {
			for(Array_iterator<const String*> c(*i.next()); c.has_next(); ) {
				enclose( result, c.next(), control_chars.encloser, control_chars.sencloser );
				if(c.has_next())
					result<<*control_chars.sseparator;
			}
			result.append_know_length("\n", 1, String::L_CLEAN);
		}
	} else {
		while(i.has_next()) {
			for(Array_iterator<const String*> c(*i.next()); c.has_next(); ) {
				result<<*c.next();
				if(c.has_next())
					result<<*control_chars.sseparator;
			}
			result.append_know_length("\n", 1, String::L_CLEAN);
		}
	}
}
#endif // don't use stringstream


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
	String file_spec=r.full_disk_path(file_name);

	if(do_append && file_exist(file_spec))
		output_column_names=false;

	TableControlChars control_chars;
	if(param_index<params.count())
		if(HashStringValue* options=params.as_hash(param_index++)) {
			int valid_options=control_chars.load(*options);
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	if(param_index<params.count())
		throw Exception(PARSER_RUNTIME, 0, "bad mode (must be nameless or append)");

	Table& table=GET_SELF(r, VTable).table();

#ifdef USE_STRINGSTREAM
	pa_stringstream ost(std::stringstream::out);

	table_to_csv(ost, table, control_chars, output_column_names);

	// write
		pa_string data=ost.str();
		const char* data_cstr=data.c_str();
		file_write(r.charsets, file_spec, data_cstr, data.length(), true /* as text */, do_append);
#else
	String sdata;

	table_to_csv(sdata, table, control_chars, output_column_names);

	// write
		const char* data_cstr=sdata.cstr();
	file_write(r.charsets, file_spec, data_cstr, sdata.length(), true /* as text */, do_append);
		if(*data_cstr) // not empty (when empty it's not heap memory)
			pa_free((void*)data_cstr); // not needed anymore
#endif
}

static void _csv_string(Request& r, MethodParams& params) {
	bool output_column_names=true;
	size_t param_index=0;
	if(params.count()>0 && params[0].is_string()) {
		if(params.as_string(0, FIRST_ARG_MUST_NOT_BE_CODE)=="nameless") {
			output_column_names=false;
			param_index++;
		} else {
			throw Exception(PARSER_RUNTIME, 0, "bad mode (must be nameless)");
		}
	}

	TableControlChars control_chars;
	if(param_index<params.count())
		if(HashStringValue* options=params.as_hash(param_index++)) {
			int valid_options=control_chars.load(*options);
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	Table& table=GET_SELF(r, VTable).table();

#ifdef USE_STRINGSTREAM
	pa_stringstream ost(std::stringstream::out);

	table_to_csv(ost, table, control_chars, output_column_names);

	r.write(*new VString(*new String(pa_strdup(ost.str().c_str()), String::L_CLEAN)));
#else
	String sdata;

	table_to_csv(sdata, table, control_chars, output_column_names);

	r.write(*new VString(*new String(sdata.cstr(), String::L_CLEAN)));
#endif
}

static void _count(Request& r, MethodParams& params) {
	Table& table=GET_SELF(r, VTable).table();
	size_t result=0;
	if(params.count()) {
		const String& param=params.as_string(0, PARAMETER_MUST_BE_STRING);
		if(param == "columns")
			result = table.columns() ? table.columns()->count() : table.max_cells();
		else if(param == "cells")
			result = table.count() ? table[table.current()]->count() : 0;
		else if(param == "rows") // synonim for ^table.count[]
			result = table.count();
		else
			throw Exception(PARSER_RUNTIME, &param, "parameter must be 'columns', 'cells' and 'rows' only");
	} else
		result = table.count();

	r.write(*new VInt(result));
}

static void _line(Request& r, MethodParams&) {
	int result=1+GET_SELF(r, VTable).table().current();
	r.write(*new VInt(result));
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
				throw Exception(PARSER_RUNTIME, &whence, "is invalid whence, valid are 'cur' or 'set'");
		}
		
		int offset=params.as_int(params.count()-1, "offset must be expression", r);
		table.offset(absolute, offset);
	} else
		r.write(*new VInt(table.current()));
}

static void _menu(Request& r, MethodParams& params) {
	InCycle temp(r);

	Value& body_code=params.as_junction(0, "body must be code");
	
	Value* delim_maybe_code=params.count()>1?&params[1]:0;

	Table& table=GET_SELF(r, VTable).table();
	size_t saved_current=table.current();

	if(delim_maybe_code) { // delimiter set
		bool need_delim=false;
		for(size_t row=0; row<table.count(); row++) {
			table.set_current(row);

			Value& sv_processed=r.process(body_code);
			TempSkip4Delimiter skip(r);

			const String* s_processed=sv_processed.get_string();
			if(s_processed && !s_processed->is_empty()) { // we have body
				if(need_delim) // need delim & iteration produced string?
					r.write(r.process(*delim_maybe_code));
				else
					need_delim=true;
			}

			r.write(sv_processed);

			if(skip.check_break())
				break;
		}
	} else {
		for(size_t row=0; row<table.count(); row++) {
			table.set_current(row);
 
			r.process_write(body_code);
 
			if(r.check_skip_break())
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
	Value* value_code;
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
		Value& sv_processed=info->r->process(*info->key_code);
		key=&sv_processed.as_string();
	} else {
		key=info->key_field < row->count() ? row->get(info->key_field) : 0;
	}

	if(!key)
		return; // ignore rows without key [too-short-record_array if-indexed]
		
	bool exist=false;
	switch(info->value_type) {
		case C_STRING: {
			if(info->value_fields->count()){
				size_t index=info->value_fields->get(0);
				exist=info->hash->put_dont_replace(*key, (index < row->count()) ? new VString(*row->get(index)) : VString::empty());
			} else {
				exist=info->hash->put_dont_replace(*key, VString::empty());
			}
			break;
		}
		case C_HASH: {
			VHash* vhash=new VHash;
			HashStringValue& hash=vhash->hash();
			Table::columns_type columns=info->table->columns();
			if(info->value_fields){ // selected fields (can be empty)
				for(Array_iterator<int> i(*info->value_fields); i.has_next(); ) {
					size_t value_field=i.next();
					if(value_field<row->count())
						hash.put(columns ? *columns->get(value_field) : String(format(value_field, 0)), new VString(*row->get(value_field)));
				}
			} else { // all fields
				for(size_t index=0; index<row->count(); index++) {
					hash.put(columns && index < columns->count() ? *columns->get(index) : String(format(index, 0)), new VString(*row->get(index)));
				}
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
			Table::element_type row_copy(new ArrayString(row->count()));
			row_copy->append(*row);
			*table+=row_copy;
			break;
		}
		case C_CODE: {
			if(!info->key_code)
				info->table->set_current(info->row++); // change context row
			exist=info->hash->put_dont_replace(*key, &info->r->process(*info->value_code));
			break;
		}
	}
	if(exist && info->distinct==D_ILLEGAL)
		throw Exception(PARSER_RUNTIME, key, "duplicate key");
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
			throw Exception(PARSER_RUNTIME, &svalue_type, "must be 'hash', 'table' or 'string'");
		}
	} else {
		throw Exception(PARSER_RUNTIME, 0, "'type' must be string");
	}
}

static Table2hash_distint get_distinct(Value& vdistinct, Table2hash_value_type& value_type){
	if(vdistinct.is_string()) {
		const String& sdistinct=*vdistinct.get_string();
		if(sdistinct!="tables")
			throw Exception(PARSER_RUNTIME, &sdistinct, "must be 'tables' or true/false");
		value_type=C_TABLE;
		return D_FIRST;
	}
	return vdistinct.as_bool() ? D_FIRST : D_ILLEGAL;
}

static void _hash(Request& r, MethodParams& params) {
	Table& self_table=GET_SELF(r, VTable).table();
	VHash& result=*new VHash;
	Table::columns_type columns=self_table.columns();

	Table2hash_distint distinct=D_ILLEGAL;
	Table2hash_value_type value_type=C_HASH;

	int param_index=params.count()-1;
	if(param_index>0) {
		if(params[1].get_junction())
			value_type=C_CODE;

		if(HashStringValue* options=params[param_index].get_hash()){ // can't use .as_hash because the 2nd param could be table so .as_hash throws an error
			--param_index;
			int valid_options=0;
			if(Value* vdistinct_code=options->get(sql_distinct_name)) { // $.distinct ?
				valid_options++;
				distinct=get_distinct(r.process(*vdistinct_code), value_type);
			}
			if(Value* vvalue_type_code=options->get(sql_value_type_name)) { // $.type ?
				if(value_type==C_TABLE) // $.distinct[tables] already was specified
					throw Exception(PARSER_RUNTIME, 0, "you can't specify $.distinct[tables] and $.type[] together");
				if(value_type==C_CODE)
					throw Exception(PARSER_RUNTIME, 0, "you can't specify $.type[] if value is code");
				valid_options++;
				value_type=get_value_type(r.process(*vvalue_type_code));
			}

			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}
	}

	if(param_index==2) // options were specified but not as hash
		throw Exception(PARSER_RUNTIME, 0, "options must be hash");

	bool value_all=false;
	Array<int> value_fields;
	Value* value_code=0;

	if(param_index==0){ // list of columns wasn't specified
		if(value_type==C_STRING)
			throw Exception(PARSER_RUNTIME, 0, "you must specify one value field with option $.type[string]");

		value_all=true; // all columns, including key
	} else { // list of columns or code was specified
		if(value_type==C_TABLE)
			throw Exception(PARSER_RUNTIME, 0, "you can't specify value field(s) with option $.distinct[tables] or $.type[tables]");

		Value& value_fields_param=params[1];
		if(value_fields_param.get_junction()){ // code specified
			value_code=&value_fields_param;
		} else if(value_fields_param.is_string()) { // one column as string was specified
			const String &field_name=*value_fields_param.get_string();
			if(!field_name.is_empty())
				value_fields+=self_table.column_name2index(field_name, true);
		} else if(Table* value_fields_table=value_fields_param.get_table()) { // list of columns were specified in table
			for(Array_iterator<Table::element_type> i(*value_fields_table); i.has_next(); ) {
				const String& value_field_name =*i.next()->get(0);
				value_fields +=self_table.column_name2index(value_field_name, true);
			}
		} else
			throw Exception(PARSER_RUNTIME, 0, "value field(s) must be string or table or code");

		if(value_type==C_STRING && value_fields.count()>1)
			throw Exception(PARSER_RUNTIME, 0, "you can't specify more then one value field with option $.type[string]");
	}

	Value* key_param=&params[0];
	Row_info info={
		&r,
		&self_table,
		/*key_code=*/key_param->get_junction() ? key_param : 0,
		/*key_field=*/0/*filled below*/,
		value_all ? NULL : &value_fields,
		value_code,
		&result.hash(),
		distinct,
		/*row=*/0,
		value_type
	};
	info.key_field=(info.key_code ? -1 : self_table.column_name2index(key_param->as_string(), true));

	int saved_current=self_table.current();
	self_table.for_each(table_row_to_hash, &info);
	self_table.set_current(saved_current);

	result.extract_default();

	r.write(result);
}

#ifndef DOXYGEN
struct Table_seq_item : public PA_Allocated {
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

	bool reverse=params.count()>1 /*..[desc|asc|]*/ && params.as_no_junction(1, "order must not be code").as_string()=="desc"; // default=asc

	Table& old_table=GET_SELF(r, VTable).table();
	Table& new_table=*new Table(old_table.columns());

	Table_seq_item* seq=new Table_seq_item[old_table.count()];
	int i;

	// calculate key values
	bool key_values_are_strings=true;
	int old_count=old_table.count();
	for(i=0; i<old_count; i++) {
		old_table.set_current(i);
		// calculate key value
		seq[i].row=old_table[i];
		Value& value=r.process(key_maker);
		if(i==0) // determining key values type by first one
			key_values_are_strings=value.is_string();

		if(key_values_are_strings)
			seq[i].value.c_str=value.as_string().cstr();
		else
			seq[i].value.d=value.as_expr_result().as_double();
	}

	// @todo: handle this elsewhere
	if(r.charsets.source().NAME()=="KOI8-R" && key_values_are_strings) {
		for(i=0; i<old_count; i++)
			if(*seq[i].value.c_str)
				seq[i].value.c_str=Charset::transcode(seq[i].value.c_str, r.charsets.source(), pa_UTF8_charset).cstr();
	}

	// sort keys
	qsort(seq, old_count, sizeof(Table_seq_item), key_values_are_strings?sort_cmp_string:sort_cmp_double);

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
	return info->r->process(*info->expression_code).as_bool();
}

static bool _locate_expression(Table& table, Request& r, MethodParams& params) {
	Value& expression_code=params.as_junction(0, "must be expression");
	Table::Action_options o=get_action_options(r, params, 1, table);
	if(params.count()>2)
		throw Exception(PARSER_RUNTIME, 0, "locate by expression only has parameters: expression and, maybe, options");
	Expression_is_true_info info={&r, &expression_code};
	return table.table_first_that(expression_is_true, &info, o);
}

static bool _locate_name_value(Table& table, Request& r, MethodParams& params) {
	const String& name=params.as_string(0, "column name must be string");
	const String& value=params.as_string(1, VALUE_MUST_BE_STRING);
	Table::Action_options o=get_action_options(r, params, 2, table);
	return table.locate(name, value, o);
}

static void _locate(Request& r, MethodParams& params) {
	Table& table=GET_SELF(r, VTable).table();

	bool result=params[0].get_junction() || (params.count() == 1) ?
		_locate_expression(table, r, params) :
		_locate_name_value(table, r, params);
	r.write(VBool::get(result));
}


static void _flip(Request& r, MethodParams&) {
	Table& old_table=GET_SELF(r, VTable).table();
	Table& new_table=*new Table(0);
	if(size_t old_count=old_table.count())
		if(size_t old_cols=old_table.columns()?old_table.columns()->count():old_table.max_cells())
			for(size_t column=0; column<old_cols; column++) {
				Table::element_type new_row(new ArrayString(old_count));
				for(size_t i=0; i<old_count; i++) {
					Table::element_type old_row=old_table[i];
					*new_row+=column<old_row->count()?old_row->get(column):new String;
				}
				new_table+=new_row;
			}

	r.write(*new VTable(&new_table));
}

static void _foreach(Request& r, MethodParams& params) {
	InCycle temp(r);

	const String* rownum_var_name=&params.as_string(0, "rownum-var name must be string");
	const String* value_var_name=&params.as_string(1, "value-var name must be string");

	Value& body_code=params.as_junction(2, "body must be code");
	
	Value* delim_maybe_code=params.count()>3?&params[3]:0;

	Table& table=GET_SELF(r, VTable).table();
	size_t saved_current=table.current();

	rownum_var_name=rownum_var_name->is_empty()? 0 : rownum_var_name;
	value_var_name=value_var_name->is_empty()? 0 : value_var_name;

	Value* var_context=r.get_method_frame()->caller();

	if(delim_maybe_code) { // delimiter set
		bool need_delim=false;
		for(size_t row=0; row<table.count(); row++) {
			table.set_current(row);

			if(rownum_var_name)
				r.put_element(*var_context, *rownum_var_name, new VString(*new String(String::Body::Format(row), String::L_CLEAN)));
			if(value_var_name)
				r.put_element(*var_context, *value_var_name, new VTable(&table));

			Value& sv_processed=r.process(body_code);
			TempSkip4Delimiter skip(r);

			const String* s_processed=sv_processed.get_string();
			if(s_processed && !s_processed->is_empty()) { // we have body
				if(need_delim) // need delim & iteration produced string?
					r.write(r.process(*delim_maybe_code));
				else
					need_delim=true;
			}

			r.write(sv_processed);

			if(skip.check_break())
				break;
		}
	} else {
		for(size_t row=0; row<table.count(); row++) {
			table.set_current(row);
 
			if(rownum_var_name)
				r.put_element(*var_context, *rownum_var_name, new VString(*new String(String::Body::Format(row), String::L_CLEAN)));
			if(value_var_name)
				r.put_element(*var_context, *value_var_name, new VTable(&table));

			r.process_write(body_code);
 
			if(r.check_skip_break())
				break;
		}
	}
	table.set_current(saved_current);
}

static void update_cell(HashStringValue::key_type aname, HashStringValue::value_type avalue, VTable *dest) {
	dest->put_element(String(aname, String::L_CLEAN), avalue); // new not required
}

inline Table::element_type row_from_string(Request& r, Value &param){
	if(!param.is_string() && !param.get_junction())
		throw Exception(PARSER_RUNTIME, 0, "row must be string, code or hash");

	const String& string=r.process_to_string(param);

	// parse cells
	Table::element_type row=new ArrayString;
	string.split(*row, 0, "\t", String::L_AS_IS);

	return row;
}

static void _append(Request& r, MethodParams& params) {
	VTable vtable=GET_SELF(r, VTable);
	Table& table=vtable.table();

	HashStringValue* hash=params[0].get_hash();
	if(hash){
		table+=new ArrayString();
		size_t saved_current=table.current();
		table.set_current(table.count()-1);
		hash->for_each<VTable*>(update_cell, &vtable);
		table.set_current(saved_current);
	} else {
		table+=row_from_string(r, params[0]);
	}
}

static void _insert(Request& r, MethodParams& params) {
	VTable vtable=GET_SELF(r, VTable);
	Table& table=vtable.table();
	HashStringValue* hash=params[0].get_hash();
	if(hash){
		table.insert(table.current(), new ArrayString());
		hash->for_each<VTable*>(update_cell, &vtable);
	} else {
		table.insert(table.current(), row_from_string(r, params[0]));
	}
}

static void _delete(Request& r, MethodParams&) {
	Table& table=GET_SELF(r, VTable).table();
	table.remove_current();
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
	if(Table* src=params.as_table(0, "source")){
		Table::Action_options o=get_action_options(r, params, 1, *src);

		Table& dest=GET_SELF(r, VTable).table();
		if(src == &dest)
			throw Exception(PARSER_RUNTIME, 0, "source and destination are same table");

		if(dest.columns()) // dest is named
			src->table_for_each(join_named_row, &dest, o);
		else // dest is nameless
			src->table_for_each(join_nameless_row, &dest, o);
	}
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

	bool add_column(SQL_Error& error, const char *str, size_t ) {
		try {
			columns+=new String(str, String::L_TAINTED /* no length as 0x00 can be inside */);
			return false;
		} catch(...) {
			error=SQL_Error("exception occurred in Table_sql_event_handlers::add_column");
			return true;
		}
	}
	bool before_rows(SQL_Error& error) {
		if(table) {
			error=SQL_Error("result must contain exactly one table");
			return true;
		}
		try {
			table=new Table(&columns);
			columns_count=columns.count();
			return false;
		} catch(...) {
			error=SQL_Error("exception occurred in Table_sql_event_handlers::before_rows");
			return true;
		}
	}
	bool add_row(SQL_Error& error) {
		try {
			*table+=row=new ArrayString(columns_count);
			return false;
		} catch(...) {
			error=SQL_Error("exception occurred in Table_sql_event_handlers::add_row");
			return true;
		}
	}
	bool add_row_cell(SQL_Error& error, const char* str, size_t ) {
		try {
			*row+=str?new String(str, String::L_TAINTED /* no length as 0x00 can be inside */):&String::Empty;
			return false;
		} catch(...) {
			error=SQL_Error("exception occurred in Table_sql_event_handlers::add_row_cell");
			return true;
		}
	}
};
#endif

static void marshal_bind( HashStringValue::key_type aname, HashStringValue::value_type avalue, SQL_Driver::Placeholder** pptr) {
	SQL_Driver::Placeholder& ph=**pptr;
	ph.name=aname.cstr();
	ph.value=avalue->as_string().untaint_cstr(String::L_AS_IS);
	ph.is_null=avalue->get_class()==void_class;
	ph.were_updated=false;

	(*pptr)++;
}

// not static, used elsewhere
int marshal_binds(HashStringValue& hash, SQL_Driver::Placeholder*& placeholders) {
	int hash_count=hash.count();
	placeholders=new(PointerGC) SQL_Driver::Placeholder[hash_count];
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
	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1, "sql options")) {
			int valid_options=0;
			if(Value* vbind=options->get(sql_bind_name)) {
				valid_options++;
				bind=vbind->get_hash();
			}
			if(Value* vlimit=options->get(sql_limit_name)) {
				valid_options++;
				limit=(ulong)r.process(*vlimit).as_double();
			}
			if(Value* voffset=options->get(sql_offset_name)) {
				valid_options++;
				offset=(ulong)r.process(*voffset).as_double();
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	SQL_Driver::Placeholder* placeholders=0;
	uint placeholders_count=0;
	if(bind)
		placeholders_count=marshal_binds(*bind, placeholders);

	const String&  statement_string=r.process_to_string(statement);
	const char* statement_cstr=statement_string.untaint_cstr(String::L_SQL, r.connection());

	Table_sql_event_handlers handlers;

	r.connection()->query(
		statement_cstr, 
		placeholders_count, placeholders,
		offset, limit, 
		handlers,
		statement_string);
	
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

	r.write(*new VTable(&result_table));
}

static void _select(Request& r, MethodParams& params) {
	Value& vcondition=params.as_expression(0, "condition must be number, bool or expression");

	Table& source_table=GET_SELF(r, VTable).table();

	int limit=source_table.count();
	int offset=0;
	bool reverse=false;
	
	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1)) {
			int valid_options=0;
			if(Value* vlimit=options->get(sql_limit_name)) {
				valid_options++;
				limit=r.process(*vlimit).as_int();
			}
			if(Value* voffset=options->get(sql_offset_name)) {
				valid_options++;
				offset=r.process(*voffset).as_int();
			}
			if(Value* vreverse=options->get(table_reverse_name)) {
				valid_options++;
				reverse=r.process(*vreverse).as_bool();
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	Table& result_table=*new Table(source_table.columns());

	size_t size=source_table.count();
	if(offset<0)
		offset+=size;
	if(size && limit>0 && offset>=0 && (size_t)offset<size){
		size_t saved_current=source_table.current();
		size_t appended=0;

		if(reverse){
			for(size_t row=size-1; result_table.count() < (size_t)limit; row--) {
				source_table.set_current(row);

				bool condition=r.process(vcondition).as_bool();

				if(condition && ++appended > (size_t)offset) // ...condition is true, adding to the result
					result_table+=source_table[row];
				if(row==0) break;
			}
		} else {
			for(size_t row=0; row < size && result_table.count() < (size_t)limit; row++) {
				source_table.set_current(row);

				bool condition=r.process(vcondition).as_bool();

				if(condition && ++appended > (size_t)offset) // ...condition is true, adding to the result
					result_table+=source_table[row];
			}
		}
		source_table.set_current(saved_current);
	}

	r.write(*new VTable(&result_table));
}


static void _rename(Request& r, MethodParams& params) {
	const String* name_from=NULL;
	const String* name_to=NULL;
	HashStringValue* names=NULL;

	if(params.count()>1){
		name_from=&params.as_string(0, COLUMN_NAME_MUST_BE_STRING);
		name_to=&params.as_string(1, COLUMN_NAME_MUST_BE_STRING);
	} else
		names=params.as_hash(0);

	Table& table=GET_SELF(r, VTable).table();
	if(Table::columns_type columns=table.columns()) {
		if(names){
			for(int i=0; i<columns->count(); i++) {
				const String *column = columns->get(i);
				if(Value* vto=names->get(*column)){
					if(const String *sto=vto->get_string())
						columns->put(i, sto);
					else
						throw Exception(PARSER_RUNTIME, column, COLUMN_NAME_MUST_BE_STRING);
				}
			}
		} else if(name_from){
			for(int i=0; i<columns->count(); i++) {
				const String *column = columns->get(i);
				if(*column == *name_from)
					columns->put(i, name_to);
			}
		}
		table.column_names_init();
	} else
		throw Exception(PARSER_RUNTIME, 0, "columns renaming is not supported for nameless tables");
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

	// ^table.csv-string[]
	// ^table.csv-string[nameless]
	// ^table.csv-string[nameless;$.encloser["] $.separator[,]]
	add_native_method("csv-string", Method::CT_DYNAMIC, _csv_string, 0, 2);

	// ^table.count[]
	// ^table.count[rows]
	// ^table.count[columns]
	// ^table.count[cells]
	add_native_method("count", Method::CT_DYNAMIC, _count, 0, 1);

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

	// ^table.foreach[row-num;value]{code}
	// ^table.foreach[row-num;value]{code}[delim]
	add_native_method("foreach", Method::CT_DYNAMIC, _foreach, 3, 4);

	// ^table.append{row{tab}data}
	add_native_method("append", Method::CT_DYNAMIC, _append, 1, 1);

	// ^table.insert{row{tab}data} before current row
	add_native_method("insert", Method::CT_DYNAMIC, _insert, 1, 1);

	// ^table.delete[] current row
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 0, 0);

	// ^table.join[table][$.limit(10) $.offset(1) $.offset[cur] ]
	add_native_method("join", Method::CT_DYNAMIC, _join, 1, 2);


	// ^table::sql[query]
	// ^table::sql[query][$.limit(1) $.offset(2)]
	add_native_method("sql", Method::CT_DYNAMIC, _sql, 1, 2);

	// ^table.columns[[column name]]
	add_native_method("columns", Method::CT_DYNAMIC, _columns, 0, 1);

	// ^table.select(expression) = table
	add_native_method("select", Method::CT_DYNAMIC, _select, 1, 2);

	// ^table.rename[column name from;column name to] 
	// ^table.rename[ $.[column name from][column name to] ... ]
	add_native_method("rename", Method::CT_DYNAMIC, _rename, 1, 2);
}
