/** @file
	Parser: @b string parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_STRING_C="$Date: 2009/07/06 12:13:30 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vdouble.h"
#include "pa_vint.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_string.h"
#include "pa_sql_connection.h"
#include "pa_dictionary.h"
#include "pa_vmethod_frame.h"
#include "pa_vregex.h"

// class

class MString: public Methoded {
public:
	MString();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(string, new MString, 0);

// defines for statics

#define MATCH_VAR_NAME "match"
#define TRIM_START_OPTION "left"
#define TRIM_END_OPTION "right"
#define TRIM_BOTH_OPTION "both"

// statics

static const String match_var_name(MATCH_VAR_NAME);

// methods

static void _length(Request& r, MethodParams&) {
	double result=GET_SELF(r, VString).string().length(r.charsets.source());
	r.write_no_lang(*new VDouble(result));
}

static void _int(Request& r, MethodParams& params) {
	const String& self_string=GET_SELF(r, VString).string();
	int converted;
	try {
		if(self_string.is_empty())
			throw Exception(PARSER_RUNTIME,
				0,
				"parameter is empty string, error converting");
		converted=self_string.as_int();
	} catch(...) { // convert problem
		if(params.count()>0)
			converted=params.as_int(0, "default must be int", r); // (default)
		else
			rethrow; // we have a problem when no default			
	}
	r.write_no_lang(*new VInt(converted));
}

static void _double(Request& r, MethodParams& params) {
	const String& self_string=GET_SELF(r, VString).string();
	double converted;
	try {
		if(self_string.is_empty())
			throw Exception(PARSER_RUNTIME,
				0,
				"parameter is empty string, error converting");
		converted=self_string.as_double();
	} catch(...) { // convert problem
		if(params.count()>0)
			converted=params.as_double(0, "default must be double", r); // (default)
		else
			rethrow; // we have a problem when no default
	}

	r.write_no_lang(*new VDouble(converted));
}

static void _bool(Request& r, MethodParams& params) {
	const String& self_string=GET_SELF(r, VString).string();
	bool converted;
	try {
		if(self_string.is_empty())
			throw Exception(PARSER_RUNTIME,
				0,
				"parameter is empty string, error converting");
		
		try {
			converted=self_string.as_bool();
		} catch(...) {
			const String& lower_string=self_string.change_case(r.charsets.source(), String::CC_LOWER);
			if(lower_string == "true"){
				converted=true;
			} else if (lower_string == "false"){
				converted=false;
			} else {
				rethrow;
			}
		}
	} catch(...) { // convert problem
		if(params.count()>0)
			converted=params.as_bool(0, "default must be bool", r); // (default)
		else
			rethrow; // we have a problem when no default
	}

	r.write_no_lang(VBool::get(converted));
}

/*not static*/void _string_format(Request& r, MethodParams& params) {

	Value& fmt_maybe_code=params[0];
	// for some time due to stupid {} in original design
	const String& fmt=r.process_to_string(fmt_maybe_code);

	const char* buf=format(r.get_self().as_double(), fmt.trim().cstrm());

	r.write_no_lang(String(buf));
}

static void _left(Request& r, MethodParams& params) {
	ssize_t sn=params.as_int(0, "n must be int", r);
	if(sn<0)
		throw Exception(PARSER_RUNTIME,
			0, 
			"n(%d) must be >=0", sn);
	size_t n=(size_t)sn;

	const String& string=GET_SELF(r, VString).string();
	r.write_assign_lang(string.mid(r.charsets.source(), 0, n));
}

static void _right(Request& r, MethodParams& params) {
	ssize_t sn=(size_t)params.as_int(0, "n must be int", r);
	if(sn<0)
		throw Exception(PARSER_RUNTIME,
			0, 
			"n(%d) must be >=0", sn);
	size_t n=(size_t)sn;

	const String& string=GET_SELF(r, VString).string();
	size_t length=string.length(r.charsets.source());
	r.write_assign_lang(n<length?string.mid(r.charsets.source(), length-n, length, length):string);
}

static void _mid(Request& r, MethodParams& params) {
	const String& string=GET_SELF(r, VString).string();

	ssize_t sbegin=params.as_int(0, "p must be int", r);
	if(sbegin<0)
		throw Exception(PARSER_RUNTIME,
			0, 
			"p(%d) must be >=0", sbegin);
	size_t begin=(size_t)sbegin;

	size_t end;
	size_t length=0;
	if(params.count()>1) {
		ssize_t sn=params.as_int(1, "n must be int", r);
		if(sn<0)
			throw Exception(PARSER_RUNTIME,
				0, 
				"n(%d) must be >=0", sn);
		end=begin+(size_t)sn;
	} else {
		length=string.length(r.charsets.source());
		end=length;
	}

	r.write_assign_lang(string.mid(r.charsets.source(), begin, end, length));
}

static void _pos(Request& r, MethodParams& params) {
	Value& substr=params.as_no_junction(0, "substr must not be code");
	
	const String& string=GET_SELF(r, VString).string();
	ssize_t offset=0;
	if(params.count()>1){
		offset=params.as_int(1, "n must be int", r);
		if(offset<0)
			throw Exception(PARSER_RUNTIME,
				0, 
				"n(%d) must be >=0", offset);
	}

	r.write_no_lang(*new VInt((int)string.pos(r.charsets.source(), substr.as_string(), (size_t)offset)));
}

static void split_list(MethodParams& params, int paramIndex,
		       const String& string, 
		       ArrayString& result) {
	Value& delim_value=params.as_no_junction(paramIndex, "delimiter must not be code");

	size_t pos_after=0;
	string.split(result, pos_after, delim_value.as_string());
}

#define SPLIT_LEFT 0x0001
#define SPLIT_RIGHT 0x0010
#define SPLIT_HORIZONTAL 0x0100
#define SPLIT_VERTICAL 0x1000

static int split_options(const String* options) {
    struct Split_option {
		const char* keyL;
		const char* keyU;
		int setBit;
		int checkBit;
    } split_option[]={
		{"l", "L", SPLIT_LEFT, SPLIT_RIGHT}, // 0xVHRL
		{"r", "R", SPLIT_RIGHT, SPLIT_LEFT},
		{"h", "H", SPLIT_HORIZONTAL, SPLIT_VERTICAL},
		{"v", "V", SPLIT_VERTICAL, SPLIT_HORIZONTAL},
		{0, 0, 0, 0}
    };

	int result=0;
	if(options) {
		for(Split_option *o=split_option; o->keyL; o++) 
			if(options->pos(o->keyL)!=STRING_NOT_FOUND 
				|| (o->keyU && options->pos(o->keyU)!=STRING_NOT_FOUND)) {
				if(result & o->checkBit)
					throw Exception(PARSER_RUNTIME,
						options,
						"conflicting split options");
				result |= o->setBit;
			}
	}

	return result;
}

static Table& split_vertical(ArrayString& pieces, bool right, const String* column_name) {
	Table::columns_type columns(new ArrayString);
	*columns+=column_name;

	Table& table=*new Table(columns, pieces.count());
	if(right) { // right
		for(int i=pieces.count(); --i>=0; ) {
			Table::element_type row(new ArrayString);
			*row+=pieces[i];
			table+=row;
		}
	} else { // left
		Array_iterator<const String*> i(pieces);
		while(i.has_next()) {
			Table::element_type row(new ArrayString);
			*row+=i.next();
			table+=row;
		}
	}

	return table;
}

static Table& split_horizontal(ArrayString& pieces, bool right) {
	Table& table=*new Table(Table::columns_type(0) /* nameless */);
	Table::element_type row(new ArrayString(pieces.count()));
	if(right) { // right
		for(int i=pieces.count(); --i>=0; )
			*row+=pieces[i];
	} else { // left
		for(Array_iterator<const String*> i(pieces); i.has_next(); )
			*row+=i.next();
	}
	table+=row;

	return table;
}

static void split_with_options(Request& r, MethodParams& params,
							   int bits) {
	const String& string=GET_SELF(r, VString).string();
	size_t params_count=params.count();

	ArrayString pieces;
	split_list(params, 0, string, pieces);

	if(!bits) {
		const String* options=0;
		if(params_count>1)
			options=&params.as_string(1, "options must not be code");
		
		bits=split_options(options);
	}

	bool right=(bits & SPLIT_RIGHT) != 0;
	bool horizontal=(bits & SPLIT_HORIZONTAL) !=0;

	const String* column_name=0;
	if(params_count>2){
		column_name=&params.as_string(2, COLUMN_NAME_MUST_BE_STRING);
		if (horizontal && !column_name->is_empty()) 
			throw Exception(PARSER_RUNTIME,
				column_name,
				"column name can't be specified with horisontal split");
	} 
	if(!column_name || column_name->is_empty())
		column_name=new String("piece");

	Table& table=horizontal?split_horizontal(pieces, right):split_vertical(pieces, right, column_name);

	r.write_no_lang(*new VTable(&table));
}
static void _split(Request& r, MethodParams& params) {
	split_with_options(r, params, 0 /* maybe-determine from param #2 */);
}
static void _lsplit(Request& r, MethodParams& params) {
	split_with_options(r, params, SPLIT_LEFT);
}
static void _rsplit(Request& r, MethodParams& params) {
	split_with_options(r, params, SPLIT_RIGHT);
}

static void search_action(Table& table, Table::element_type row, int, int, int, int, void *) {
	if(row)
		table+=row;
}

#ifndef DOXYGEN
struct Replace_action_info {
	Request* request;
	const String* src;
	String* dest;
	VTable* vtable;
	Value* replacement_code;
};
#endif
/// @todo they can do $global[$result] there, getting pointer to later-invalid local var, kill this
static void replace_action(Table& table, ArrayString* row, 
				int prestart, int prefinish, 
				int poststart, int postfinish,
				void *info) {
	Replace_action_info& ai=*static_cast<Replace_action_info *>(info);
	if(row) { // begin&middle
		// piece from last match['prestart'] to beginning of this match['prefinish']
		if(prestart!=prefinish)
			*ai.dest << ai.src->mid(prestart, prefinish);//ai.dest->APPEND_CONST("-");
		// store found parts in one-record VTable
		if(table.count()) // middle
			table.put(0, row);
		else // begin
			table+=row;

		{ // execute 'replacement_code' in 'table' context
			if(ai.replacement_code){
				ai.vtable->set_table(table);
				*ai.dest << ai.request->process_to_string(*ai.replacement_code);
			}
		}
	} else // end
		*ai.dest << ai.src->mid(poststart, postfinish);
}

static void _match(Request& r, MethodParams& params) {
	size_t params_count=params.count();

	Value& regexp=params.as_no_junction(0, "regexp must not be code");
	Value* options=(params_count>1)?&params.as_no_junction(1, "options must not be code"):0;

	VRegex* vregex;
	VRegexCleaner vrcleaner;

	if(Value* value=regexp.as(VREGEX_TYPE, false)){
		if(options && options->is_defined())
			throw Exception(PARSER_RUNTIME,
				0,
				"you can not specify regex-object and options together"
			);
		vregex=static_cast<VRegex*>(value);
	} else {
		vregex=new VRegex(r.charsets.source(),
			&regexp.as_string(),
			(options)?(&options->as_string()):0);
		vregex->study();
		vrcleaner.vregex=vregex;
	}

	Temp_lang temp_lang(r, String::L_PASS_APPENDED);
	const String& src=GET_SELF(r, VString).string();
	int matches_count=0;

	if(params_count<3) { // search
		Table* table=src.match(vregex,
			search_action, 0,
			matches_count);

		if(table){
			r.write_no_lang(*new VTable(table));
		} else {
			r.write_no_lang(*new VInt(matches_count));
		}

	} else { // replace

		Value* replacement_code=0;
		bool is_junction=false;

		Value* replacement=&params[2];
		if(replacement->get_junction()){
			replacement_code=replacement;
			is_junction=true;
		} else if(replacement->is_string()){
			if(replacement->is_defined())
				replacement_code=replacement;
		} else if(!replacement->is_void())
			throw Exception(PARSER_RUNTIME,
				0,
				"replacement option should be junction or string");

		String result;
		VTable* vtable=new VTable;
		Replace_action_info info={
			&r,
			&src,
			&result,
			vtable,
			replacement_code
		};

		Temp_value_element* temp_match_var=0;

		if(is_junction)
			temp_match_var=new Temp_value_element(
				*replacement_code->get_junction()->method_frame,
				match_var_name, vtable);

		src.match(vregex,
			replace_action, &info,
			matches_count);

		if(temp_match_var)
			delete temp_match_var;

		r.write_assign_lang(result);
	}
}

static void change_case(Request& r, MethodParams&, 
						String::Change_case_kind kind) {
	const String& src=GET_SELF(r, VString).string();

	r.write_assign_lang(src.change_case(r.charsets.source(), kind));
}
static void _upper(Request& r, MethodParams& params) {
	change_case(r, params, String::CC_UPPER);
}
static void _lower(Request& r, MethodParams& params) {
	change_case(r, params, String::CC_LOWER);
}

#ifndef DOXYGEN
class String_sql_event_handlers: public SQL_Driver_query_event_handlers {
	const String& statement_string; const char* statement_cstr;
	bool got_column;
public:
	bool got_cell;
	String& result;
public:
	String_sql_event_handlers(
		const String& astatement_string, const char* astatement_cstr):
		statement_string(astatement_string), statement_cstr(astatement_cstr),
		got_column(false),
		got_cell(false),
		result(*new String) {}

	bool add_column(SQL_Error& error, const char* /*str*/, size_t /*length*/) {
		if(got_column) {
			error=SQL_Error(PARSER_RUNTIME,
				//statement_string,
				"result must contain exactly one column");
			return true;
		}
		got_column=true;
		return false;
	}
	bool before_rows(SQL_Error& /*error*/ ) { /* ignore */ return false; }
	bool add_row(SQL_Error& /*error*/) { /* ignore */ return false; }
	bool add_row_cell(SQL_Error& error, const char* str, size_t length) {
		if(got_cell) {
			error=SQL_Error(PARSER_RUNTIME,
				//statement_string,
				"result must not contain more then one row");
			return true;
		}

		try {
			got_cell=true;
			result.append_know_length(str, length, String::L_TAINTED);
			return false;
		} catch(...) {
			error=SQL_Error("exception occured in String_sql_event_handlers::add_row_cell");
			return true;
		}
	}
};
#endif
extern String sql_bind_name;
extern String sql_limit_name;
extern String sql_offset_name;
extern String sql_default_name;
extern String sql_distinct_name;
extern int marshal_binds(HashStringValue& hash, SQL_Driver::Placeholder*& placeholders);
extern void unmarshal_bind_updates(HashStringValue& hash, int placeholder_count, SQL_Driver::Placeholder* placeholders);

const String* sql_result_string(Request& r, MethodParams& params,
				HashStringValue*& options, Value*& default_code) {
	Value& statement=params.as_junction(0, "statement must be code");

	HashStringValue* bind=0;
	ulong limit=SQL_NO_LIMIT;
	ulong offset=0;
	default_code=0;
	if(params.count()>1) {
		Value& voptions=params.as_no_junction(1, "options must be hash, not code");
		if(voptions.is_defined() && !voptions.is_string())
			if((options=voptions.get_hash())) {
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
				if((default_code=options->get(sql_default_name))) {
					valid_options++;
				}
				if(valid_options!=options->count())
					throw Exception(PARSER_RUNTIME,
						0,
						"called with invalid option");
			} else
				throw Exception(PARSER_RUNTIME,
					0,
					"options must be hash");
	} else
		options=0;

	SQL_Driver::Placeholder* placeholders=0;
	uint placeholders_count=0;
	if(bind)
		placeholders_count=marshal_binds(*bind, placeholders);

	Temp_lang temp_lang(r, String::L_SQL);
	const String& statement_string=r.process_to_string(statement);
	const char* statement_cstr=statement_string.untaint_cstr(String::L_AS_IS, r.connection());

	String_sql_event_handlers handlers(statement_string, statement_cstr);

	r.connection()->query(
		statement_cstr, 
		placeholders_count, placeholders,
		offset, limit, 
		handlers,
		statement_string);
	
	if(bind)
		unmarshal_bind_updates(*bind, placeholders_count, placeholders);

	if(!handlers.got_cell)
		return 0; // no lines, caller should return second param[default value]

	return &handlers.result;
}

static void _sql(Request& r, MethodParams& params) {

	HashStringValue* options;
	Value* default_code;
	const String* string=sql_result_string(r, params, options, default_code);
	if(!string) {
		if(default_code) {
			string=&r.process_to_string(*default_code);
		} else
			throw Exception(PARSER_RUNTIME,
				0,
				"produced no result, but no default option specified");
	}

	r.write_assign_lang(*string);
}

static void _replace(Request& r, MethodParams& params) {
	const String& src=GET_SELF(r, VString).string();

	Table* table=params.as_no_junction(0, PARAM_MUST_NOT_BE_CODE).get_table();
	if(!table)
		throw Exception(PARSER_RUNTIME,
			0,
			"parameter must be table");

	Dictionary dict(*table);
	r.write_assign_lang(src.replace(dict));
}

static void _save(Request& r, MethodParams& params) {
	size_t params_count=params.count();
	const String& file_name=params.as_string(params_count-1, FILE_NAME_MUST_BE_STRING);

	const String& src=GET_SELF(r, VString).string();

	bool do_append=false;
	if(params_count>1) {
		const String& mode=params.as_string(0, "mode must be string");
		if(mode=="append")
			do_append=true;
		else
			throw Exception(PARSER_RUNTIME,
				&mode,
				"unknown mode, must be 'append'");
	}		

	// write
	const char* buf=src.untaint_cstr(String::L_AS_IS, r.connection(false/*no error if none*/));
	file_write(r.absolute(file_name), 
		buf, strlen(buf), true, do_append);
}

static void _normalize(Request& r, MethodParams&) {
	const String& src=GET_SELF(r, VString).string();

	r.write_assign_lang(src);
}

static void _trim(Request& r, MethodParams& params) {
	const String& src=GET_SELF(r, VString).string();

	String::Trim_kind kind=String::TRIM_BOTH;
	size_t params_count=params.count();
	const char* chars=0;
	if(params_count>0) {
		const String& skind=params.as_string(0, "'where' must be string");
		if(!skind.is_empty())
			if(skind==TRIM_BOTH_OPTION)
				kind=String::TRIM_BOTH;
			else if(skind==TRIM_START_OPTION || skind=="start")
				kind=String::TRIM_START;
			else if(skind==TRIM_END_OPTION || skind=="end")
				kind=String::TRIM_END;
			else
				throw Exception(PARSER_RUNTIME,
					&skind,
					"'kind' must be one of "TRIM_START_OPTION", "TRIM_BOTH_OPTION", "TRIM_END_OPTION);

		if(params_count>1) {
			const String& schars=params.as_string(1, "'chars' must be string");
			if(!schars.is_empty())
				chars=schars.cstr();
		}
	}

	r.write_assign_lang(src.trim(kind, chars, &r.charsets.source()));
}

static void _append(Request& r, MethodParams& params) {
	// c=a+b
	VString& va=GET_SELF(r, VString);
	const String& a=va.string();
	const String& b=params.as_string(0, PARAMETER_MUST_BE_STRING);
	String& c=*new String(a);
	c.append(b, String::L_PASS_APPENDED);
	va.set_string(c);
}

static void _base64(Request& r, MethodParams& params) {
	if(params.count()) {
		// decode: ^string:base64[encoded]
		const char* cstr=params.as_string(0, PARAMETER_MUST_BE_STRING).cstr();
		char* decoded=0;
		size_t length=0;
		pa_base64_decode(cstr, strlen(cstr), decoded, length);
		if(decoded && length){
			if(memchr((const char*)decoded, 0, length))
				throw Exception(PARSER_RUNTIME,
					0,
					"Invalid \\x00 character found while decode to string. Decode it to file instead.");

			fix_line_breaks(decoded, length);
			if(length)
				r.write_assign_lang(*new String(decoded, String::L_TAINTED));
		}
	} else {
		// encode: ^str.base64[]
		VString& self=GET_SELF(r, VString);
		const char* cstr=self.string().cstr();
		const char* encoded=pa_base64_encode(cstr, strlen(cstr));
		r.write_assign_lang(*new String(encoded, String::L_TAINTED/*once ?param=base64(something) was needed*/));
	}
}

static void _escape(Request& r, MethodParams&){
	const String& src=GET_SELF(r, VString).string();
	r.write_assign_lang(src.escape(r.charsets.source()));
}

static void _unescape(Request& r, MethodParams& params){
	const String& src=params.as_string(0, PARAMETER_MUST_BE_STRING);
	if(const char* result=unescape_chars(src.cstr(), src.length(), &r.charsets.source(), true/* don't unescape '+' char */))
		r.write_assign_lang(*new String(result));
}

// constructor

MString::MString(): Methoded("string") {
	// ^string.length[]
	add_native_method("length", Method::CT_DYNAMIC, _length, 0, 0);
	
	// ^string.int[]
	// ^string.int(default)
	add_native_method("int", Method::CT_DYNAMIC, _int, 0, 1);
	// ^string.double[]
	// ^string.double(default)
	add_native_method("double", Method::CT_DYNAMIC, _double, 0, 1);
	// ^void.bool[]
	// ^void.bool(default)
	add_native_method("bool", Method::CT_DYNAMIC, _bool, 0, 1);

	// ^string.format[format]
	add_native_method("format", Method::CT_DYNAMIC, _string_format, 1, 1);

	// ^string.left(n)
	add_native_method("left", Method::CT_DYNAMIC, _left, 1, 1);
	// ^string.right(n)
	add_native_method("right", Method::CT_DYNAMIC, _right, 1, 1);
	// ^string.mid(p)
	// ^string.mid(p;n)
	add_native_method("mid", Method::CT_DYNAMIC, _mid, 1, 2);

	// ^string.pos[substr]
	// ^string.pos[substr](n)
	add_native_method("pos", Method::CT_DYNAMIC, _pos, 1, 2);

	// ^string.split[delim]
	// ^string.split[delim][options]
	// ^string.split[delim][options][column name]
	add_native_method("split", Method::CT_DYNAMIC, _split, 1, 3);
		// old names for backward compatibility
		// ^string.lsplit[delim]
		add_native_method("lsplit", Method::CT_DYNAMIC, _lsplit, 1, 1);
		// ^string.rsplit[delim]
		add_native_method("rsplit", Method::CT_DYNAMIC, _rsplit, 1, 1);
	
	// ^string.match[regexp][options]
	// ^string.match[regexp][options]{replacement-code}
	add_native_method("match", Method::CT_DYNAMIC, _match, 1, 3);

	// ^string.upper[]
	add_native_method("upper", Method::CT_DYNAMIC, _upper, 0, 0);
	// ^string.lower[]
	add_native_method("lower", Method::CT_DYNAMIC, _lower, 0, 0);

	// ^sql[query]
	// ^sql[query][options hash]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);

	// ^string.replace[table]
	add_native_method("replace", Method::CT_DYNAMIC, _replace, 1, 1);

	// ^string.save[file]  
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^string.normalize[]  
	add_native_method("normalize", Method::CT_DYNAMIC, _normalize, 0, 0);

	// ^string.trim[[start|both|end][;chars]]
	add_native_method("trim", Method::CT_DYNAMIC, _trim, 0, 2);

	// ^string.append[string]
	add_native_method("append", Method::CT_DYNAMIC, _append, 1, 1);

	// ^string.base64[] << encode
	// ^string:base64[encoded string] << decode	
	add_native_method("base64", Method::CT_ANY, _base64, 0, 1);

	// ^string.js-escape[]
	// ^string:js-unescape[escaped%uXXXXstring]
	add_native_method("js-escape", Method::CT_ANY, _escape, 0, 0);
	add_native_method("js-unescape", Method::CT_STATIC, _unescape, 1, 1);
}	
