/** @file
	Parser: @b string parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: string.C,v 1.71 2001/08/28 09:27:42 parser Exp $"; 

#include "classes.h"
#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_string.h"
#include "pa_sql_connection.h"
#include "pa_dictionary.h"

// defines

#define STRING_CLASS_NAME "string"

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
	Value& result=*new(pool) VDouble(pool, r.self->get_string()->size());
	result.set_name(method_name);
	r.write_no_lang(result);
}

static void _int(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	Value& result=*new(pool) VInt(pool, r.self->as_int());
	result.set_name(method_name);
	r.write_no_lang(result);
}

static void _double(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	Value& result=*new(pool) VDouble(pool, r.self->as_double());
	result.set_name(method_name);
	r.write_no_lang(result);
}

/*not static*/void _string_format(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& fmt=params->as_junction(0, "fmt must be code");

	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	char *buf=format(pool, r.self->as_double(), r.process(fmt).as_string().cstr());

	String result(pool);
	result.APPEND_CLEAN(buf, 0, 
		method_name.origin().file,
		method_name.origin().line);
	r.write_no_lang(result);
}

static void _left(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	size_t n=(size_t)r.process(params->get(0)).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.mid(0, n)));
}

static void _right(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	size_t n=(size_t)r.process(params->get(0)).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.mid(string.size()-n, string.size())));
}

static void _mid(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	size_t p=(size_t)r.process(params->get(0)).as_double();
	size_t n=(size_t)r.process(params->get(1)).as_double();
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
	r.write_assign_lang(*new(pool) VString(string.mid(p, p+n)));
}

static void _pos(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& substr=params->as_no_junction(0, "substr must not be code");
	
	const String& string=*static_cast<VString *>(r.self)->get_string();
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
	const String& string=*static_cast<VString *>(r.self)->get_string();

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
	const String& string=*static_cast<VString *>(r.self)->get_string();

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

static void search_action(Table& table, Array *row, int, int, void *) {
	if(row)
		table+=row;
}

/// used by string: _match / replace_action
struct Replace_action_info {
	Request *request;  const String *origin;
	const String *src;  String *dest;
	Value *replacement_code;
	const String *post_match;
};
static void replace_action(Table& table, Array *row, int start, int finish, 
							   void *info) {
	Replace_action_info& ai=*static_cast<Replace_action_info *>(info);
	if(row) { // begin&middle
		// piece from last match['start'] to beginning of this match['finish']
		if(start!=finish)
			*ai.dest << ai.src->mid(start, finish);//ai.dest->APPEND_CONST("-");
		// store found parts in one-record VTable
		if(table.size()) // middle
			table.put(0, row);
		else // begin
			table+=row;
		{ // execute 'replacement_code' in 'table' context
			VTable& vtable=*new(table.pool()) VTable(table.pool(), &table);
			vtable.set_name(*ai.origin);

			Junction *junction=ai.replacement_code->get_junction();
			Value *saved_match_var_value=junction->root->get_element(*match_var_name);
			junction->root->put_element(*match_var_name, &vtable);
			Value& replaced=ai.request->process(*ai.replacement_code, ai.origin, false);
			junction->root->put_element(*match_var_name, saved_match_var_value);

			/*
			ai.dest->APPEND_CONST("(");
				*ai.dest << *(String *)row->get(1/*match* /);
			ai.dest->APPEND_CONST(")");
			*/
			*ai.dest << replaced.as_string();
		}
		ai.post_match=(String *)row->get(2/*post_match*/);
	} else // end
		*ai.dest << *ai.post_match;
}

/// @todo use pcre:study somehow
static void _match(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& src=*static_cast<VString *>(r.self)->get_string();

	Value& regexp=params->as_no_junction(0, "regexp must not be code");

	const String *options=
		params->size()>1?
		&params->as_no_junction(1, "options must not be code").as_string():0;

	Value *result;
	Temp_lang temp_lang(r, String::UL_PASS_APPENDED);
	Table *table;
	if(params->size()<3) { // search
		bool was_global;
		bool matched=src.match(r.pcre_tables,
			&method_name, 
			regexp.as_string(), options,
			&table,
			search_action, 0,
			&was_global);
		// matched
		// not (just matched[3=pre/match/post], no substrings) or Global search
		if(table->columns()->size()>3 || was_global) 
			result=new(pool) VTable(pool, table); // table of pre/match/post+substrings
		else 
			result=new(pool) VBool(pool, matched);			
	} else { // replace
		Value& replacement_code=params->as_junction(2, "replacement code must be code");

		String& dest=*new(pool) String(pool);
		Replace_action_info replace_action_info={
			&r, &method_name,
			&src, &dest,
			&replacement_code,
			&src
		};
		src.match(r.pcre_tables,
			&method_name, 
			r.process(regexp).as_string(), options,
			&table,
			replace_action, &replace_action_info);
		result=new(pool) VString(dest);
	}
	result->set_name(method_name);
	r.write_assign_lang(*result);
}

static void change_case(Request& r, const String& method_name, MethodParams *params, 
						String::Change_case_kind kind) {
	Pool& pool=r.pool();
	const String& src=*static_cast<VString *>(r.self)->get_string();

	r.write_assign_lang(*new(pool) VString(src.change_case(pool, r.pcre_tables,
		kind)));
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
			PTHROW(0, 0,
				&statement_string,
				"result must contain exactly one column");
		got_column=true;
	}
	void before_rows() { /* ignore */ }
	void add_row() { /* ignore */ }
	void add_row_cell(void *ptr, size_t size) {
		if(got_cell)
			PTHROW(0, 0,
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
								Hash *&options) {
	Pool& pool=r.pool();

	if(!r.connection)
		PTHROW(0, 0,
			&method_name,
			"without connect");

	Value& statement=params->as_junction(0, "statement must be code");

	ulong limit=0;
	ulong offset=0;
	if(params->size()>1) {
		Value& options_param=params->as_no_junction(1, "options must be hash, not code");
		if(options=options_param.get_hash()) {
			if(Value *vlimit=(Value *)options->get(*sql_limit_name))
				limit=(ulong)r.process(*vlimit).as_double();
			if(Value *voffset=(Value *)options->get(*sql_offset_name))
				offset=(ulong)r.process(*voffset).as_double();
		} else
			PTHROW(0, 0,
				&method_name,
				"options must be hash");
	} else
		options=0;

	Temp_lang temp_lang(r, String::UL_SQL);
	const String& statement_string=r.process(statement).as_string();
	const char *statement_cstr=
		statement_string.cstr(String::UL_UNSPECIFIED, r.connection);
	String_sql_event_handlers handlers(pool, statement_string, statement_cstr);
	bool need_rethrow=false; Exception rethrow_me;
	PTRY {
		r.connection->query(
			statement_cstr, offset, limit, 
			handlers);
	}
	PCATCH(e) { // query problem
		rethrow_me=e;  need_rethrow=true;
	}
	PEND_CATCH
	if(need_rethrow)
		PTHROW(rethrow_me.type(), rethrow_me.code(),
			&statement_string, // setting more specific source [were url]
			rethrow_me.comment());
	
	if(!handlers.got_cell)
		return 0; // no lines, caller should return second param[default value]

	return handlers.result;
}

static void _sql(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Hash *options;
	const String *string=sql_result_string(r, method_name, params, options);
	if(!string) {
		if(options) {
			if(Value *vdefault=(Value *)options->get(*sql_default_name)) {
				if(!vdefault->get_junction())
					PTHROW(0, 0,
						&method_name,
						"default option must be code");
				string=r.process(*vdefault).get_string();
				if(!string)
					string=empty_string;
			} else
				PTHROW(0, 0,
					&method_name,
					"produced no result, but no default option specified");
		} else
			PTHROW(0, 0,
				&method_name,
				"produced no result, but no options (no default) specified");
	}
	VString& result=*new(pool) VString(*string);
	result.set_name(method_name);
	r.write_assign_lang(result);
}

static void _replace(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& src=*static_cast<VString *>(r.self)->get_string();

	Table *table=params->as_no_junction(0, "parameter must not be code").get_table();
	if(!table)
		PTHROW(0, 0,
			&method_name,
			"parameter must be table");

	Dictionary dict(*table);
	r.write_assign_lang(*new(pool) VString(src.replace(pool, dict)));
}
// constructor

MString::MString(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), STRING_CLASS_NAME));


	// ^string.length[]
	add_native_method("length", Method::CT_DYNAMIC, _length, 0, 0);
	
	// ^string.int[]
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 0);
	
	// ^string.double[]
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 0);

	// ^string.format{format}
	add_native_method("format", Method::CT_DYNAMIC, _string_format, 1, 1);

	// ^string.left(n)
	add_native_method("left", Method::CT_DYNAMIC, _left, 1, 1);
	// ^string.right(n)
	add_native_method("right", Method::CT_DYNAMIC, _right, 1, 1);
	// ^string.mid(p;n)
	add_native_method("mid", Method::CT_DYNAMIC, _mid, 2, 2);

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
}	

// global variable

Methoded *string_class;

// creator

Methoded *MString_create(Pool& pool) {
	return string_class=new(pool) MString(pool);
}
