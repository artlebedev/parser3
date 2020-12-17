/** @file
	Parser: @b string parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"
#include "pa_base64.h"

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
#include "pa_charsets.h"

volatile const char * IDENT_STRING_C="$Id: string.C,v 1.250 2020/12/17 20:47:00 moko Exp $";

// class

class MString: public Methoded {
public:
	MString();
};

// global variable

DECLARE_CLASS_VAR(string, new MString);

// void class, inherited from string and thus should be inited afterwards

class MVoid: public Methoded {
public:
	MVoid();
};

// void global variable should be after string global variable

DECLARE_CLASS_VAR(void, new MVoid);

// defines for statics

#define MATCH_VAR_NAME "match"
#define TRIM_START_OPTION "left"
#define TRIM_END_OPTION "right"
#define TRIM_BOTH_OPTION "both"

#define MODE_APPEND "append"

#define UNESCAPE_MODE_JS "js"
#define UNESCAPE_MODE_URI "uri"

// statics

static const String match_var_name(MATCH_VAR_NAME);

// methods

static void _length(Request& r, MethodParams&) {
	double result=GET_SELF(r, VString).string().length(r.charsets.source());
	r.write(*new VDouble(result));
}

static void _int(Request& r, MethodParams& params) {
	const String& self_string=GET_SELF(r, VString).string();
	int converted;

	if(self_string.is_empty()) {
		if(params.count()>0)
			converted=params.as_int(0, "default must be int", r); // (default)
		else
			throw Exception(PARSER_RUNTIME, 0, "unable to convert empty string without default specified");
	} else {
		try {
			converted=self_string.as_int();
		} catch(...) { // convert problem
			if(params.count()>0)
				converted=params.as_int(0, "default must be int", r); // (default)
			else
				rethrow; // we have a problem when no default
		}
	}

	r.write(*new VInt(converted));
}

static void _double(Request& r, MethodParams& params) {
	const String& self_string=GET_SELF(r, VString).string();

	if(self_string.is_empty()) {
		if(params.count()>0)
			r.write(*new VDouble(params.as_double(0, "default must be double", r))); // (default)
		else
			throw Exception(PARSER_RUNTIME, 0, "unable to convert empty string without default specified");
	} else {
		try {
			r.write(*new VDouble(self_string.as_double()));
		} catch(...) { // convert problem
			if(params.count()>0)
				r.write(*new VDouble(params.as_double(0, "default must be double", r))); // (default)
			else
				rethrow; // we have a problem when no default
		}
	}
}

static void _bool(Request& r, MethodParams& params) {
	const String& self_string=GET_SELF(r, VString).string();
	bool converted;
	const char *str=self_string.cstr();

	if(self_string.is_empty()) {
		if(params.count()>0)
			converted=params.as_bool(0, "default must be bool", r); // (default)
		else
			throw Exception(PARSER_RUNTIME, 0, "unable to convert empty string without default specified");
	} else if( (str[0]=='T' || str[0]=='t') && (str[1]=='R' || str[1]=='r') && (str[2]=='U' || str[2]=='u') &&
		   (str[3]=='E' || str[3]=='e') && str[4]==0 ) { // "true"
		converted=true;
	} else if( (str[0]=='F' || str[0]=='f') && (str[1]=='A' || str[1]=='a') && (str[2]=='L' || str[2]=='l') &&
		   (str[3]=='S' || str[3]=='s') && (str[4]=='E' || str[4]=='e') && str[5]==0 ) { // "false"
		converted=false;
	} else {
		try {
			converted=self_string.as_bool();
		} catch(...) { // convert problem
			if(params.count()>0)
				converted=params.as_bool(0, "default must be bool", r); // (default)
			else
				rethrow; // we have a problem when no default
		}
	}

	r.write(VBool::get(converted));
}

/*not static*/void _string_format(Request& r, MethodParams& params) {

	Value& fmt_maybe_code=params[0];
	// for some time due to stupid {} in original design
	const String& fmt=r.process_to_string(fmt_maybe_code);

	const char* buf=format(r.get_self().as_double(), fmt.trim().cstrm());

	r.write(String(buf));
}

static void _left(Request& r, MethodParams& params) {
	int sn=params.as_int(0, "n must be int", r);
	const String& string=GET_SELF(r, VString).string();
	r.write(sn<0 ? string : string.mid(r.charsets.source(), 0, sn));
}

static void _right(Request& r, MethodParams& params) {
	int sn=params.as_int(0, "n must be int", r);
	if(sn>0){
		size_t n=sn;
		const String& string=GET_SELF(r, VString).string();
		size_t length=string.length(r.charsets.source());
		r.write(n<length ? string.mid(r.charsets.source(), length-n, length, length) : string);
	}
}

static void _mid(Request& r, MethodParams& params) {
	const String& string=GET_SELF(r, VString).string();

	int begin=params.as_int(0, "p must be int", r);
	if(begin<0)
		throw Exception(PARSER_RUNTIME, 0,  "p(%d) must be >=0", begin);

	size_t end;
	size_t length=0;

	if(params.count()>1) {
		int n=params.as_int(1, "n must be int", r);
		if(n<0)
			throw Exception(PARSER_RUNTIME, 0, "n(%d) must be >=0", n);
		end=begin+n;
	} else {
		length=string.length(r.charsets.source());
		end=length;
	}

	r.write(string.mid(r.charsets.source(), begin, end, length));
}

static void _pos(Request& r, MethodParams& params) {
	Value& substr=params.as_no_junction(0, "substr must not be code");
	
	const String& string=GET_SELF(r, VString).string();
	int offset=0;
	if(params.count()>1){
		offset=params.as_int(1, "n must be int", r);
		if(offset<0)
			throw Exception(PARSER_RUNTIME, 0, "n(%d) must be >=0", offset);
	}

	r.write(*new VInt((int)string.pos(r.charsets.source(), substr.as_string(), offset)));
}

struct Split_action_info {
	const String& src;
	ArrayString &result;
};

static void split_action(Table& , ArrayString* row, int prestart, int prefinish, int poststart, int postfinish, void *info) {
	Split_action_info& ai=*static_cast<Split_action_info *>(info);
	if(row) { // begin&middle
		// piece from last match['prestart'] to beginning of this match['prefinish']
		ai.result += &ai.src.mid(prestart, prefinish);
	} else // end
		if(poststart != postfinish)
			ai.result += &ai.src.mid(poststart, postfinish);
}

static void split_list(Value& delim_value, const String& string, ArrayString& result) {
	if(Value* value=delim_value.as(VREGEX_TYPE)){
		VRegex *vregex=static_cast<VRegex*>(value);
		vregex->study();

		int matches_count=0;
		Split_action_info ai = { string, result };

		string.match(vregex, split_action, &ai, matches_count);
	} else
		string.split(result, 0, delim_value.as_string());
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
					throw Exception(PARSER_RUNTIME, options, "conflicting split options");
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

static void split_with_options(Request& r, MethodParams& params, int bits) {
	const String& string=GET_SELF(r, VString).string();
	size_t params_count=params.count();

	ArrayString pieces;
	split_list(params.as_no_junction(0, "delimiter must not be code"), string, pieces);

	if(!bits) {
		const String* options=0;
		if(params_count>1)
			options=&params.as_string(1, OPTIONS_MUST_NOT_BE_CODE);
		bits=split_options(options);
	}

	bool right=(bits & SPLIT_RIGHT) != 0;
	bool horizontal=(bits & SPLIT_HORIZONTAL) !=0;

	const String* column_name=0;
	if(params_count>2){
		column_name=&params.as_string(2, COLUMN_NAME_MUST_BE_STRING);
		if (horizontal && !column_name->is_empty()) 
			throw Exception(PARSER_RUNTIME, column_name, "column name can't be specified with horisontal split");
	} 
	if(!column_name || column_name->is_empty())
		column_name=new String("piece");

	Table& table=horizontal?split_horizontal(pieces, right):split_vertical(pieces, right, column_name);

	r.write(*new VTable(&table));
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
static void replace_action(Table& table, ArrayString* row, int prestart, int prefinish, int poststart, int postfinish, void *info) {
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
	Value* options=(params_count>1)?&params.as_no_junction(1, OPTIONS_MUST_NOT_BE_CODE):0;

	VRegex* vregex;
	VRegexCleaner vrcleaner;

	if(Value* value=regexp.as(VREGEX_TYPE)){
		if(options && options->is_defined())
			throw Exception(PARSER_RUNTIME, 0, "you can not specify regex-object and options together");
		vregex=static_cast<VRegex*>(value);
	} else {
		vregex=new VRegex(r.charsets.source(), &regexp.as_string(), (options) ? (&options->as_string()) : 0);
		vregex->study();
		vrcleaner.vregex=vregex;
	}

	const String& src=GET_SELF(r, VString).string();
	int matches_count=0;

	if(params_count<3) { // search
		Table* table=src.match(vregex, search_action, 0, matches_count);

		if(table){
			r.write(*new VTable(table));
		} else {
			r.write(*new VInt(matches_count));
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
			throw Exception(PARSER_RUNTIME, 0, "replacement option should be junction or string");

		Value* default_code=(params_count==4) ? &params.as_junction(3, "default value must be code") : 0;

		String result;
		VTable* vtable=new VTable;
		Replace_action_info info={
			&r,
			&src,
			&result,
			vtable,
			replacement_code
		};

		if(is_junction){
			Temp_value_element temp(r, *replacement_code->get_junction()->method_frame, match_var_name, vtable);
			src.match(vregex, replace_action, &info, matches_count);
		} else {
			src.match(vregex, replace_action, &info, matches_count);
		}

		if(!matches_count && default_code)
			r.process_write(*default_code);
		else
			r.write(result);
	}
}

static void change_case(Request& r, MethodParams&, String::Change_case_kind kind) {
	const String& src=GET_SELF(r, VString).string();

	r.write(src.change_case(r.charsets.source(), kind));
}
static void _upper(Request& r, MethodParams& params) {
	change_case(r, params, String::CC_UPPER);
}
static void _lower(Request& r, MethodParams& params) {
	change_case(r, params, String::CC_LOWER);
}

#ifndef DOXYGEN
class String_sql_event_handlers: public SQL_Driver_query_event_handlers {
	bool got_column;
public:
	bool got_cell;
	const String* result;
public:
	String_sql_event_handlers():
		got_column(false),
		got_cell(false),
		result(&String::Empty) {}

	bool add_column(SQL_Error& error, const char* /*str*/, size_t /*length*/) {
		if(got_column) {
			error=SQL_Error("result must contain exactly one column");
			return true;
		}
		got_column=true;
		return false;
	}
	bool before_rows(SQL_Error& /*error*/ ) { /* ignore */ return false; }
	bool add_row(SQL_Error& /*error*/) { /* ignore */ return false; }
	bool add_row_cell(SQL_Error& error, const char* str, size_t) {
		if(got_cell) {
			error=SQL_Error("result must not contain more then one row");
			return true;
		}
		try {
			got_cell=true;
			result=new String(str, String::L_TAINTED /* no length as 0x00 can be inside */ );
			return false;
		} catch(...) {
			error=SQL_Error("exception occurred in String_sql_event_handlers::add_row_cell");
			return true;
		}
	}
};
#endif

extern int marshal_binds(HashStringValue& hash, SQL_Driver::Placeholder*& placeholders);
extern void unmarshal_bind_updates(HashStringValue& hash, int placeholder_count, SQL_Driver::Placeholder* placeholders);

const String* sql_result_string(Request& r, MethodParams& params, Value*& default_code) {
	Value& statement=params.as_junction(0, "statement must be code");

	HashStringValue* bind=0;
	ulong limit=SQL_NO_LIMIT;
	ulong offset=0;
	default_code=0;
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
			if((default_code=options->get(sql_default_name))) {
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	SQL_Driver::Placeholder* placeholders=0;
	uint placeholders_count=0;
	if(bind)
		placeholders_count=marshal_binds(*bind, placeholders);

	const String& statement_string=r.process_to_string(statement);
	const char* statement_cstr=statement_string.untaint_cstr(String::L_SQL, r.connection());

	String_sql_event_handlers handlers;

	r.connection()->query(statement_cstr, placeholders_count, placeholders, offset, limit, handlers, statement_string);
	
	if(bind)
		unmarshal_bind_updates(*bind, placeholders_count, placeholders);

	if(!handlers.got_cell)
		return 0; // no lines, caller should return second param[default value]

	return handlers.result;
}

static void _sql(Request& r, MethodParams& params) {

	Value* default_code;
	const String* string=sql_result_string(r, params, default_code);
	if(!string) {
		if(default_code) {
			string=&r.process_to_string(*default_code);
		} else
			throw Exception(PARSER_RUNTIME, 0, "produced no result, but no default option specified");
	}

	r.write(*string);
}

static void _replace(Request& r, MethodParams& params) {
	const String& src=GET_SELF(r, VString).string();

	if(params.count()==1) {
		// ^string.replace[table]
		if(Table* table=params.as_table(0, "param")){
			Dictionary dict(*table);
			r.write(src.replace(dict));
		} else
			r.write(src);
	} else {
		// ^string.replace[from-string;to-string]
		Dictionary dict(params.as_string(0, "from must be string"), params.as_string(1, "to must be string"));
		r.write(src.replace(dict));
	}

}

static void _save(Request& r, MethodParams& params) {
	bool do_append=false;
	Charset* asked_charset=0;

	size_t file_name_index=0;
	if(params.count()>1) {
		if(HashStringValue* options=params.as_no_junction(1, "second parameter should be string or hash").get_hash()){
			// ^file.save[filespec;$.charset[] $.append(true)]
			int valid_options=0;
			if(Value* vcharset_name=options->get(PA_CHARSET_NAME)){
				asked_charset=&pa_charsets.get(vcharset_name->as_string());
				valid_options++;
			}
			if(Value* vappend=options->get(MODE_APPEND)){
				do_append=vappend->as_bool();
				valid_options++;
			}
			if(valid_options != options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		} else {
			// ^file.save[append;filespec]
			const String& mode=params.as_string(0, "mode must be string");
			if(mode==MODE_APPEND){
				do_append=true;
				file_name_index++;
			} else
				throw Exception(PARSER_RUNTIME,
					&mode,
					"unknown mode, must be 'append'");
		}
	}

	const String& file_name=params.as_string(file_name_index, FILE_NAME_MUST_BE_STRING);
	const String& src=GET_SELF(r, VString).string();

	String::Body sbody=src.cstr_to_string_body_untaint(String::L_AS_IS, r.connection(false), &r.charsets);

	// write
	file_write(r.charsets, r.full_disk_path(file_name), sbody.cstr(), sbody.length(), true, do_append, asked_charset);
}

static void _normalize(Request& r, MethodParams&) {
	const String& src=GET_SELF(r, VString).string();

	r.write(src);
}

static void _trim(Request& r, MethodParams& params) {
	const String& src=GET_SELF(r, VString).string();

	String::Trim_kind kind=String::TRIM_BOTH;
	size_t params_count=params.count();
	const char* chars=0;
	if(params_count>0) {
		const String& skind=params.as_string(0, "'where' must be string");
		if(!skind.is_empty()) {
			if(skind==TRIM_BOTH_OPTION)
				kind=String::TRIM_BOTH;
			else if(skind==TRIM_START_OPTION || skind=="start")
				kind=String::TRIM_START;
			else if(skind==TRIM_END_OPTION || skind=="end")
				kind=String::TRIM_END;
			else if(params_count==1)
				chars=skind.cstr();
			else
				throw Exception(PARSER_RUNTIME, &skind, "'kind' must be one of " TRIM_START_OPTION ", " TRIM_BOTH_OPTION ", " TRIM_END_OPTION);
		}

		if(params_count>1) {
			const String& schars=params.as_string(1, "'chars' must be string");
			if(!schars.is_empty())
				chars=schars.cstr();
		}
	}

	r.write(src.trim(kind, chars, &r.charsets.source()));
}

Base64Options base64_encode_options(Request& r, HashStringValue* options) {
	Base64Options result;
	if(options) {
		int valid_options=0;
		for(HashStringValue::Iterator i(*options); i; i.next()) {
			String::Body key=i.key();
			Value* value=i.value();
			if(key == "pad") {
				result.pad=r.process(*value).as_bool();
				valid_options++;
			} else if(key == "wrap") {
				result.wrap=r.process(*value).as_bool();
				valid_options++;
			} else if(key == "url-safe") {
				if(r.process(*value).as_bool())
					result.set_url_safe_abc();
				valid_options++;
			}
		}

		if(valid_options != options->count())
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
	}
	return result;
}

Base64Options base64_decode_options(Request& r, HashStringValue* options) {
	Base64Options result;
	if(options) {
		int valid_options=0;
		for(HashStringValue::Iterator i(*options); i; i.next() ) {
			String::Body key=i.key();
			Value* value=i.value();
			if(key == "pad") {
				result.pad=r.process(*value).as_bool();
				valid_options++;
			} else if(key == "strict") {
				result.strict=r.process(*value).as_bool();
				valid_options++;
			} else if(key == "url-safe") {
				if(r.process(*value).as_bool())
					result.set_url_safe_abc();
				valid_options++;
			}
		}

		if(valid_options != options->count())
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
	}
	return result;
}

static void _base64(Request& r, MethodParams& params) {
	if(&r.get_self() == string_class) {
		// decode: ^string:base64[encoded[;options]]
		const char* cstr=params.count() ? params.as_string(0, PARAMETER_MUST_BE_STRING).cstr() : "";
		Base64Options options = base64_decode_options(r, params.count() > 1 ? params.as_hash(1) : NULL);

		char* decoded=0;
		size_t length=pa_base64_decode(cstr, strlen(cstr), decoded, options);

		if(decoded && length){
			if(memchr(decoded, 0, length))
				throw Exception(PARSER_RUNTIME, 0, "Invalid \\x00 character found while decoding to string. Decode to file instead.");

			fix_line_breaks(decoded, length);

			if(length)
				r.write(*new String(decoded, String::L_TAINTED));
		}
	} else {
		// encode: ^str.base64[options]
		VString& self=GET_SELF(r, VString);
		const char* cstr=self.string().cstr();
		Base64Options options = base64_encode_options(r, params.count() ? params.as_hash(0) : NULL);
		const char* encoded=pa_base64_encode(cstr, strlen(cstr), options);
		r.write(*new String(encoded, String::L_TAINTED /*once ?param=base64(something) was needed*/ ));
	}
}

static void _idna(Request& r, MethodParams& params) {
	if(&r.get_self() == string_class) {
		// decode: ^string:idna[encoded]
		const char* cstr=params.count() ? params.as_string(0, PARAMETER_MUST_BE_STRING).cstr() : "";
		r.write(*new String(pa_idna_decode(cstr, r.charsets.source()), String::L_TAINTED));
	} else {
		// encode: ^str.idna[]
		VString& self=GET_SELF(r, VString);
		const char* cstr=self.string().cstr();
		r.write(*new String(pa_idna_encode(cstr, r.charsets.source()), String::L_TAINTED));
	}
}

static void _js_escape(Request& r, MethodParams&){
	const String& src=GET_SELF(r, VString).string();
	r.write(src.escape(r.charsets.source()));
}

static void _js_unescape(Request& r, MethodParams& params){
	const String& src=params.as_string(0, PARAMETER_MUST_BE_STRING);
	if(const char* result=unescape_chars(src.cstr(), src.length(), &r.charsets.source(), true))
		r.write(*new String(result, String::L_TAINTED));
}

static void _unescape(Request& r, MethodParams& params){
	const String& mode=params.as_string(0, MODE_MUST_NOT_BE_CODE);
	const String& src=params.as_string(1, PARAMETER_MUST_BE_STRING);

	Charset* from_charset=&r.charsets.client();

	if(params.count() > 2)
		if(HashStringValue* options=params.as_hash(2)) {
			int valid_options=0;
			if(Value* vcharset_name=options->get(PA_CHARSET_NAME)){
				from_charset=&pa_charsets.get(vcharset_name->as_string());
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	bool mode_js;
	if(mode==UNESCAPE_MODE_JS){
		mode_js=true;
	} else if(mode==UNESCAPE_MODE_URI){
		mode_js=false;
	} else {
		throw Exception(PARSER_RUNTIME, &mode, "is invalid mode, must be either '" UNESCAPE_MODE_JS "' or '" UNESCAPE_MODE_URI "'");
	}

	const char* unescaped=unescape_chars(src.cstr(), src.length(), from_charset, mode_js);
	if(*unescaped){
		const String* result=new String(Charset::transcode(unescaped, *from_charset, r.charsets.source()), String::L_TAINTED);
		r.write(*result);
	}
}

static void _contains(Request& r, MethodParams& params) {
	// empty or whitespace string is hash compatible
	GET_SELF(r, VString).get_element(params.as_string(0, "key must be string"));
	// ignoring result as it allways null
	r.write(VBool::get(false));
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
	// ^string.match[regexp][options]{replacement-code}{code-if-nothing-is-found}
	add_native_method("match", Method::CT_DYNAMIC, _match, 1, 4);

	// ^string.upper[]
	add_native_method("upper", Method::CT_DYNAMIC, _upper, 0, 0);
	// ^string.lower[]
	add_native_method("lower", Method::CT_DYNAMIC, _lower, 0, 0);

	// ^string:sql{query}
	// ^string:sql{query}[options hash]
	add_native_method("sql", Method::CT_STATIC, _sql, 1, 2);

	// ^string.replace[table]
	add_native_method("replace", Method::CT_DYNAMIC, _replace, 1, 2);

	// ^string.save[append][file]
	// ^string.save[file]
	// ^string.save[file][$.append(true) $.charset[...]]
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^string.normalize[]  
	add_native_method("normalize", Method::CT_DYNAMIC, _normalize, 0, 0);

	// ^string.trim[[start|both|end][;chars]]
	add_native_method("trim", Method::CT_DYNAMIC, _trim, 0, 2);

	// ^string.base64[] << encode
	// ^string:base64[encoded string] << decode
	add_native_method("base64", Method::CT_ANY, _base64, 0, 2);

	// ^string.idna[] << encode
	// ^string:idna[encoded string] << decode
	add_native_method("idna", Method::CT_ANY, _idna, 0, 1);

	// ^string.js-escape[]
	add_native_method("js-escape", Method::CT_DYNAMIC, _js_escape, 0, 0);

	// ^string:js-unescape[escaped%uXXXXstring]
	add_native_method("js-unescape", Method::CT_STATIC, _js_unescape, 1, 1);

	// ^string:unescape[js|uri;escaped;$.charset[...]]
	add_native_method("unescape", Method::CT_STATIC, _unescape, 2, 3);

	// ^string.contains[key] for hash compatibility
	add_native_method("contains", Method::CT_DYNAMIC, _contains, 1, 1);
}
