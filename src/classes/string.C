/** @file
	Parser: @b string parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_STRING_C="$Date: 2002/08/15 10:38:18 $";

#include "classes.h"
#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_string.h"
#include "pa_sql_connection.h"
#include "pa_dictionary.h"

// class

class MString : public Methoded {
public:
	MString(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _length(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	double result=r.self->get_string()->size();
	r.write_no_lang(*new(pool) VDouble(pool, result));
}

static void _int(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	int converted;
	Value *default_code=params->size()>0?
		default_code=&params->as_junction(0, "default must be int"):0; // (default)
	try {
		converted=r.self->as_int();
	} catch(...) { // convert problem
		if(!default_code) // we have a problem when no default
			/*re*/throw;
		else
			converted=r.process_to_value(*default_code).as_int();
	}
	r.write_no_lang(*new(pool) VInt(pool, converted));
}

static void _double(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	double converted;
	Value *default_code=params->size()>0?
		default_code=&params->as_junction(0, "default must be double"):0; // (default)
	try {
		converted=r.self->as_double();
	} catch(...) { // convert problem
		if(!default_code) // we have a problem when no default
			/*re*/throw;  
		else
			converted=r.process_to_value(*default_code).as_double();
	}

	r.write_no_lang(*new(pool) VDouble(pool, converted));
}

/*not static*/void _string_format(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& fmt_maybe_code=params->get(0);
	// for some time due to stupid {} in original design
	const String& fmt=r.process_to_string(fmt_maybe_code);

	char *buf=format(pool, r.self->as_double(), fmt.cstr());

	String result(pool);
	result.APPEND_CLEAN(buf, 0, 
		method_name.origin().file,
		method_name.origin().line);
	r.write_no_lang(result);
}

static void _left(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	size_t n=(size_t)params->as_int(0, "n must be int", r);
	
	const String& string=static_cast<VString *>(r.self)->string();
	r.write_assign_lang(string.mid(0, n));
}

static void _right(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	size_t n=(size_t)params->as_int(0, "n must be int", r);
	
	const String& string=static_cast<VString *>(r.self)->string();
	r.write_assign_lang(string.mid(string.size()-n, string.size()));
}

static void _mid(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();
	const String& string=*r.self->get_string();

	size_t p=(size_t)max(0, params->as_int(0, "p must be int", r));
	size_t n=params->size()>1?
		(size_t)max(0, params->as_int(1, "n must be int", r)):string.size();
	
	r.write_assign_lang(string.mid(p, p+n));
}

static void _pos(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& substr=params->as_no_junction(0, "substr must not be code");
	
	const String& string=static_cast<VString *>(r.self)->string();
	r.write_assign_lang(*new(pool) VInt(pool, string.pos(substr.as_string())));
}

static void split_list(Request& r, const String& method_name, MethodParams *params,
					   const String& string, 
					   Array& result) {
	Value& delim_value=params->as_no_junction(0, "delimiter must not be code");

	string.split(result, 0, delim_value.as_string());
}

static void _lsplit(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& string=*r.self->get_string();

	Array pieces(pool);
	split_list(r, method_name, params, string, pieces);

	Array& columns=*new(pool) Array(pool);
	columns+=new(pool) String(pool, "piece");

	Table& table=*new(pool) Table(pool, &string, 
		&columns, pieces.size());
	Array_iter i(pieces);
	while(i.has_next()) {
		Array& row=*new(pool) Array(pool);
		row+=i.next();
		table+=&row;
	}
	r.write_no_lang(*new(pool) VTable(pool, &table));
}

static void _rsplit(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& string=*r.self->get_string();

	Array pieces(pool);
	split_list(r, method_name, params, string, pieces);

	Array& columns=*new(pool) Array(pool);
	columns+=new(pool) String(pool, "piece");

	Table& table=*new(pool) Table(pool, &string, 
		&columns, pieces.size());
	for(int i=pieces.size(); --i>=0; ) {
		Array& row=*new(pool) Array(pool);
		row+=pieces.get(i);
		table+=&row;
	}

	r.write_no_lang(*new(pool) VTable(pool, &table));
}

static void search_action(Table& table, Array *row, int, int, int, int, void *) {
	if(row)
		table+=row;
}

#ifndef DOXYGEN
struct Replace_action_info {
	Request *request;  const String *origin;
	const String *src;  String *dest;
	VTable *vtable;
	Value *replacement_code;
};
#endif
/// @todo they can do $global[$result] there, getting pointer to later-invalid local var, kill this
static void replace_action(Table& table, Array *row, 
						   int prestart, int prefinish, 
						   int poststart, int postfinish,
						   void *info) {
	Replace_action_info& ai=*static_cast<Replace_action_info *>(info);
	if(row) { // begin&middle
		// piece from last match['prestart'] to beginning of this match['prefinish']
		if(prestart!=prefinish)
			*ai.dest << ai.src->mid(prestart, prefinish);//ai.dest->APPEND_CONST("-");
		// store found parts in one-record VTable
		if(table.size()) // middle
			table.put(0, row);
		else // begin
			table+=row;
		{ // execute 'replacement_code' in 'table' context
			ai.vtable->set_table(table);

			*ai.dest << ai.request->process_to_string(*ai.replacement_code);
		}
	} else // end
		*ai.dest << ai.src->mid(poststart, postfinish);
}

/// @todo use pcre:study somehow
static void _match(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	Value& regexp=params->as_no_junction(0, "regexp must not be code");

	const String *options=
		params->size()>1?
		&params->as_no_junction(1, "options must not be code").as_string():0;

	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	Table *table;
	if(params->size()<3) { // search
		const String& src=static_cast<VString *>(r.self)->string();

		bool was_global;
		bool matched=src.match(
			&method_name, 
			regexp.as_string(), options,
			&table,
			search_action, 0,
			&was_global);
		Value *result;
		// matched
		// not (just matched[3=pre/match/post], no substrings) or Global search
		if(table->columns()->size()>3 || was_global) 
			result=new(pool) VTable(pool, table/*TODO: clone this when table would be stacked!*/); // table of pre/match/post+substrings
		else 
			result=new(pool) VBool(pool, matched);			
		r.write_assign_lang(*result);
	} else { // replace
		const String& src=*r.self->get_string();

		Value& replacement_code=params->as_junction(2, "replacement param must be code");

		String& result=*new(pool) String(pool);
		VTable vtable(pool);
		Replace_action_info replace_action_info={
			&r, &method_name,
			&src, &result,
			&vtable,
			&replacement_code
		};
		Temp_value_element temp_match_var(
			*replacement_code.get_junction()->root, 
			*match_var_name, &vtable);
		src.match(
			&method_name, 
			r.process_to_string(regexp), options,
			&table,
			replace_action, &replace_action_info);
		r.write_assign_lang(result);
	}
}

static void change_case(Request& r, const String& method_name, MethodParams *params, 
						String::Change_case_kind kind) {
	Pool& pool=r.pool();
	const String& src=static_cast<VString *>(r.self)->string();

	r.write_assign_lang(src.change_case(pool, kind));
}
static void _upper(Request& r, const String& method_name, MethodParams *params) {
	change_case(r, method_name, params, String::CC_UPPER);
}
static void _lower(Request& r, const String& method_name, MethodParams *params) {
	change_case(r, method_name, params, String::CC_LOWER);
}

#ifndef DOXYGEN
class String_sql_event_handlers : public SQL_Driver_query_event_handlers {
public:
	String_sql_event_handlers(Pool& apool, 
		const String& astatement_string, const char *astatement_cstr) :
		pool(apool), 
		statement_string(astatement_string),
		statement_cstr(astatement_cstr),
		got_column(false), got_cell(false) {
		result=new(pool) String(pool);
	}

	void add_column(void *ptr, size_t size) {
		if(got_column)
			throw Exception("parser.runtime",
				&statement_string,
				"result must contain exactly one column");
		got_column=true;
	}
	void before_rows() { /* ignore */ }
	void add_row() { /* ignore */ }
	void add_row_cell(void *ptr, size_t size) {
		if(got_cell)
			throw Exception("parser.runtime",
				&statement_string,
				"result must not contain more then one row");
		got_cell=true;

		result->APPEND_TAINTED((const char *)ptr, size, statement_cstr, 0);
	}

private:
	Pool& pool;
	const String& statement_string; const char *statement_cstr;
	bool got_column;
public:
	bool got_cell;
	String *result;
};
#endif
const String* sql_result_string(Request& r, const String& method_name, MethodParams *params,
								Hash *& options, Value *& default_code) {
	Pool& pool=r.pool();

	Value& statement=params->as_junction(0, "statement must be code");

	ulong limit=0;
	ulong offset=0;
	default_code=0;
	if(params->size()>1) {
		Value& voptions=params->as_no_junction(1, "options must be hash, not code");
		if(!voptions.is_string())
			if(options=voptions.get_hash(&method_name)) {
				if(Value *vlimit=(Value *)options->get(*sql_limit_name))
					limit=(ulong)r.process_to_value(*vlimit).as_double();
				if(Value *voffset=(Value *)options->get(*sql_offset_name))
					offset=(ulong)r.process_to_value(*voffset).as_double();
				if(default_code=(Value *)options->get(*sql_default_name)) {
					if(Junction *default_junction=default_code->get_junction())
						default_junction->change_context(statement.get_junction());
					else
						throw Exception("parser.runtime",
							&method_name,
							"default option must be code");
				}
			} else
				throw Exception("parser.runtime",
					&method_name,
					"options must be hash");
	} else
		options=0;

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process_to_string(statement);
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection(&method_name));
	String_sql_event_handlers handlers(pool, statement_string, statement_cstr);
	r.connection(&method_name)->query(
		statement_cstr, offset, limit, 
		handlers,
		statement_string);
	
	if(!handlers.got_cell)
		return 0; // no lines, caller should return second param[default value]

	return handlers.result;
}

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Hash *options;
	Value *default_code;
	const String *string=sql_result_string(r, method_name, params, options, default_code);
	if(!string) {
		if(default_code) {
			string=&r.process_to_string(*default_code);
			if(!string)
				string=new(pool) String(pool);
		} else
			throw Exception("parser.runtime",
				&method_name,
				"produced no result, but no default option specified");
	}

	r.write_assign_lang(*string);
}

static void _replace(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& src=*r.self->get_string();

	Table *table=params->as_no_junction(0, "parameter must not be code").get_table();
	if(!table)
		throw Exception("parser.runtime",
			&method_name,
			"parameter must be table");

	Dictionary dict(*table);
	r.write_assign_lang(src.replace(pool, dict));
}

static void _save(Request& r, const String& method_name, MethodParams *params) {
	const String& file_name=params->as_string(params->size()-1, 
		"file name must be string");

	const String& src=static_cast<VString *>(r.self)->string();

	bool do_append=false;
	if(params->size()>1) {
		const String& mode=params->as_string(0, "mode must be string");
		if(mode=="append")
			do_append=true;
		else
			throw Exception("parser.runtime",
				&mode,
				"unknown mode, must be 'append'");
	}		

	// write
	const char *buf=src.cstr(String::UL_UNSPECIFIED);
	file_write(r.absolute(file_name), 
		buf, strlen(buf), true, do_append);
}

static void _normalize(Request& r, const String& method_name, MethodParams * /*params*/) {
 	r.write_assign_lang(r.self->get_string()->join_chains(r.pool(), 0/*cstr*/));
}

// constructor

MString::MString(Pool& apool) : Methoded(apool, "string") {
	// ^string.length[]
	add_native_method("length", Method::CT_DYNAMIC, _length, 0, 0);
	
	// ^string.int[]
	// ^string.int(default)
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);
	// ^string.double[]
	// ^string.double(default)
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);

	// ^string.format{format}
	add_native_method("format", Method::CT_DYNAMIC, _string_format, 1, 1);

	// ^string.left(n)
	add_native_method("left", Method::CT_DYNAMIC, _left, 1, 1);
	// ^string.right(n)
	add_native_method("right", Method::CT_DYNAMIC, _right, 1, 1);
	// ^string.mid(p;n)
	add_native_method("mid", Method::CT_DYNAMIC, _mid, 1, 2);

	// ^string.pos[substr]
	add_native_method("pos", Method::CT_DYNAMIC, _pos, 1, 1);

	// ^string.lsplit[delim]
	add_native_method("lsplit", Method::CT_DYNAMIC, _lsplit, 1, 1);
	// ^string.rsplit[delim]
	add_native_method("rsplit", Method::CT_DYNAMIC, _rsplit, 1, 1);

	// ^string.match[regexp][options]
	// ^string.match[regexp][options]{replacement-code}
	add_native_method("match", Method::CT_DYNAMIC, _match, 1, 3);

	// ^string.toupper[]
	add_native_method("upper", Method::CT_DYNAMIC, _upper, 0, 0);
	// ^string.tolower[]
	add_native_method("lower", Method::CT_DYNAMIC, _lower, 0, 0);

	// ^sql[query]
	// ^sql[query][$.limit(1) $.offset(2) $.default[n/a]]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);

	// ^string.replace[table]
	add_native_method("replace", Method::CT_DYNAMIC, _replace, 1, 1);

	// ^string.save[file]  
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^string.normalize[]  
	add_native_method("normalize", Method::CT_DYNAMIC, _normalize, 0, 0);
}	

// global variable

Methoded *string_class;

// creator

Methoded *MString_create(Pool& pool) {
	return string_class=new(pool) MString(pool);
}
