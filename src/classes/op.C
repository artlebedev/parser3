/** @file
	Parser: parser @b operators.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_OP_C="$Date: 2003/10/02 07:26:46 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"
#include "pa_vdate.h"
#include "pa_vmethod_frame.h"
#include "pa_vclass.h"

// limits

#define MAX_LOOPS 10000

// defines

#define CASE_DEFAULT_VALUE "DEFAULT"

// class

class VClassMAIN: public VClass {
public:
	VClassMAIN();
};

// defines for statics

#define SWITCH_DATA_NAME "SWITCH-DATA"
#define CACHE_DATA_NAME "CACHE-DATA"
#define EXCEPTION_VAR_NAME "exception"

// statics

//^switch ^case
static const String switch_data_name(SWITCH_DATA_NAME);
//^cache
static const String cache_data_name(CACHE_DATA_NAME);

static const String exception_var_name(EXCEPTION_VAR_NAME);

// helpers

class Untaint_lang_name2enum: public Hash<const String::Body, String::Language> {
public:
	Untaint_lang_name2enum() {
		#define ULN(name, LANG) \
			put(String::Body(name), (value_type)(String::L_##LANG));
		ULN("as-is", AS_IS);
		ULN("optimized-as-is", AS_IS|String::L_OPTIMIZE_BIT);
		ULN("file-spec", FILE_SPEC);
		ULN("http-header", HTTP_HEADER);
		ULN("mail-header", MAIL_HEADER);
		ULN("uri", URI);
		ULN("table", TABLE);
		ULN("sql", SQL);
		ULN("js", JS);
		ULN("xml", XML);
		ULN("optimized-xml", XML|String::L_OPTIMIZE_BIT);
		ULN("html", HTML);
		ULN("optimized-html", HTML|String::L_OPTIMIZE_BIT);
		#undef ULN
	}
} untaint_lang_name2enum;

// methods

static void _if(Request& r, MethodParams& params) {
	Value& condition_code=params.as_junction(0, "condition must be expression");

	bool condition=r.process_to_value(condition_code, 
		/*0/*no name* /,*/
		false/*don't intercept string*/).as_bool();
	if(condition)
		r.write_pass_lang(r.process(params.as_junction(1, "'then' parameter must be code")));
	else if(params.count()>2)
		r.write_pass_lang(r.process(params.as_junction(2, "'else' parameter must be code")));
}

static void _untaint(Request& r, MethodParams& params) {

	String::Language lang;
	if(params.count()==1)
		lang=String::L_AS_IS; // mark as simply 'tainted'. useful in html from sql 
	else {
		const String& lang_name=params.as_string(0, "lang must be string");
		lang=untaint_lang_name2enum.get(lang_name);
		if(!lang)
			throw Exception(0,
				&lang_name,
				"invalid taint language");
	}

	{
		Value& vbody=params.as_junction(params.count()-1, "body must be code");
		
		Temp_lang temp_lang(r, lang); // set temporarily specified ^untaint[language;
		r.write_pass_lang(r.process(vbody)); // process marking tainted with that lang
	}
}

static void _taint(Request& r, MethodParams& params) {
	String::Language lang;
	if(params.count()==1)
		lang=String::L_TAINTED; // mark as simply 'tainted'. useful in table:set
	else {
		const String& lang_name=params.as_string(0, "lang must be string");
		lang=untaint_lang_name2enum.get(lang_name);
		if(!lang)
			throw Exception(0,
				&lang_name,
				"invalid taint language");
	}

	{
		Value& vbody=params.as_no_junction(params.count()-1, "body must not be code");
		
		String result;
		result.append(
			vbody.as_string(),  // process marking tainted with that lang
			lang, true);  // force result language to specified
		r.write_pass_lang(result);
	}
}

static void _process(Request& r, MethodParams& params) {
	Method* main_method;

	size_t index=0;
	Value* target_self;
	Value& maybe_target_self=params[index];
	if(maybe_target_self.get_string() || maybe_target_self.get_junction())
		target_self=&r.get_method_frame()->caller()->self();
	else {
		target_self=&maybe_target_self;  index++;
	}

	{
		VStateless_class *target_class=target_self->get_last_derived_class();
		if(!target_class)
			throw Exception("parser.runtime",
				0,
				"no target class");

		// temporary remove language change
		Temp_lang temp_lang(r, String::L_PASS_APPENDED);
		// temporary zero @main so to maybe-replace it in processed code
		Temp_method temp_method_main(*target_class, main_method_name, 0);
		// temporary zero @auto so it wouldn't be auto-called in Request::use_buf
		Temp_method temp_method_auto(*target_class, auto_method_name, 0);

		size_t main_alias_index=index+1;
		const String* main_alias=0;
		if(main_alias_index<params.count())
			main_alias=&params.as_string(main_alias_index, "main alias must be string");

		if(const String* file_name=params[index].get_string()) { // ^process...[file]
			r.use_file(*target_class, r.absolute(*file_name), main_alias, true/*ignore_class_path*/);
		} else { // process...{string}
			Value& vjunction=params.as_junction(index, "body must be code");
			// evaluate source to process
			const String& source=r.process_to_string(vjunction);
			r.use_buf(*target_class,
				source.cstr(String::L_UNSPECIFIED, r.connection(false)),
				main_alias,
				pseudo_file_no__process);		
		}

		// main_method
		main_method=target_class->get_method(main_method_name);
	}
	// after restoring current-request-lang
	// maybe-execute @main[]
	if(main_method) {
		// temporarily set method_frame's self to target_self
		Temp_method_frame_self tmfs(*r.get_method_frame(), *target_self);
		// execute!	
		r.execute(*main_method->parser_code);
	}
}
	
static void _rem(Request& r, MethodParams& params) {
	params.as_junction(0, "body must be code");
}

static void _while(Request& r, MethodParams& params) {
	Value& vcondition=params.as_junction(0, "condition must be expression");
	Value& body=params.as_junction(1, "body must be code");

	// while...
	int endless_loop_count=0;
	while(true) {
		if(++endless_loop_count>=MAX_LOOPS) // endless loop?
			throw Exception("parser.runtime",
				0,
				"endless loop detected");

		bool condition=r.process_to_value(vcondition, 
				/*0/*no name* /,*/
				false/*don't intercept string*/).as_bool();
		if(!condition) // ...condition is true
			break;

		// write processed body
		r.write_pass_lang(r.process(body));
	}
}

static void _use(Request& r, MethodParams& params) {
	Value& vfile=params.as_no_junction(0, "file name must not be code");
	r.use_file(r.main_class, vfile.as_string());
}

static void _for(Request& r, MethodParams& params) {
	const String& var_name=params.as_string(0, "var name must be string");
	int from=params.as_int(1, "from must be int", r);
	int to=params.as_int(2, "to must be int", r);
	Value&  body_code=params.as_junction(3, "body must be code");
	Value* delim_maybe_code=params.count()>4?&params[4]:0;

	if(to-from>=MAX_LOOPS) // too long loop?
		throw Exception("parser.runtime",
			0,
			"endless loop detected");

	bool need_delim=false;
	VInt* vint=new VInt(0);
	r.get_method_frame()->caller()->put_element(var_name, vint, false);
	for(int i=from; i<=to; i++) {
		vint->set_int(i);

		StringOrValue sv_processed=r.process(body_code);
		const String* s_processed=sv_processed.get_string();
		if(delim_maybe_code && s_processed && s_processed->length()) { // delimiter set and we have body
			if(need_delim) // need delim & iteration produced string?
				r.write_pass_lang(r.process(*delim_maybe_code));
			need_delim=true;
		}
		r.write_pass_lang(sv_processed);
	}
}

static void _eval(Request& r, MethodParams& params) {
	Value& expr=params.as_junction(0, "need expression");
	// evaluate expresion
	Value& value_result=r.process_to_value(expr, 
		/*0/*no name YET* /,*/
		true/*don't intercept string*/).as_expr_result();
	if(params.count()>1) {
		Value& fmt=params.as_no_junction(1, "fmt must not be code");
		r.write_no_lang(String(format(value_result.as_double(), fmt.as_string().cstrm())));
	} else
		r.write_no_lang(value_result);
}

static void _connect(Request& r, MethodParams& params) {
#ifdef RESOURCES_DEBUG
struct timeval mt[2];
#endif
	Value& url=params.as_no_junction(0, "url must not be code");
	Value& body_code=params.as_junction(1, "body must be code");

	Table* protocol2driver_and_client=0;
	if(Value* sql=r.main_class.get_element(String(MAIN_SQL_NAME), r.main_class, false)) {
		if(Value* element=sql->get_element(String(MAIN_SQL_DRIVERS_NAME), *sql, false)) {
			protocol2driver_and_client=element->get_table();
		}
	}

#ifdef RESOURCES_DEBUG
//measure:before
gettimeofday(&mt[0],NULL);
#endif
	// connect
	SQL_Connection* connection=SQL_driver_manager.get_connection(url.as_string(), 
		protocol2driver_and_client);

#ifdef RESOURCES_DEBUG
//measure:after connect
gettimeofday(&mt[1],NULL);

double t[2];
for(int i=0;i<2;i++)
    t[i]=mt[i].tv_sec+mt[i].tv_usec/1000000.0;

r.sql_connect_time+=t[1]-t[0];
#endif
	Temp_connection temp_connection(r, connection);
	// execute body
	try {
		r.write_assign_lang(r.process(body_code));
		connection->commit();
		connection->close();
	} catch(...) { // process problem
		connection->rollback();
		connection->close();
		rethrow;
	}
}

#ifndef DOXYGEN
class Switch_data: public PA_Object {
public:
	Request& r;
	Value& searching;
	Value* found;
	Value* _default;
public:
	Switch_data(Request& ar, Value& asearching): 
	  r(ar), searching(asearching) {}
};
#endif
static void _switch(Request& r, MethodParams& params) {
	Switch_data* data=new Switch_data(r, r.process_to_value(params[0]));
	Temp_hash_value<const String::Body, PA_Object*> 
		switch_data_setter(r.classes_conf, switch_data_name, data);

	Value& cases_code=params.as_junction(1, "switch cases must be code");
	// execution of found ^case[...]{code} must be in context of ^switch[...]{code}
	// because of stacked WWrapper used there as wcontext
	r.process(cases_code, true/*intercept_string*/);
	if(Value* selected_code=data->found? data->found: data->_default) {
		// setting code context, would execute in ^switch[...]{>>context<<}
		//selected_code->get_junction()->change_context(cases_code.get_junction());
		r.write_pass_lang(r.process(*selected_code));
	}
}

static void _case(Request& r, MethodParams& params) {
	Switch_data* data=static_cast<Switch_data*>(r.classes_conf.get(switch_data_name));
	if(!data)
		throw Exception("parser.runtime",
			0,
			"without switch");

	int count=params.count();
	Value& code=params.as_junction(--count, "case result must be code");
	
	// killing context for safety, would execute in ^switch[...]{>>context<<}
	// reason: context is stacked, and it would become invalid afterwards
	//code->get_junction()->change_context(0);

	for(int i=0; i<count; i++) {
		Value& value=r.process_to_value(params[i]);

		if(value.as_string() == CASE_DEFAULT_VALUE) {
			data->_default=&code;
			break;
		}

		bool matches;
		if(data->searching.is_string())
			matches=data->searching.as_string() == value.as_string();
		else
			matches=data->searching.as_double() == value.as_double();

		if(matches) {
			if(data->found)
				throw Exception("parser.runtime",
					0,
					"duplicate found");

			data->found=&code;
			break;
		}
	}
}

// cache--

// consts

const int DATA_STRING_SERIALIZED_VERSION=0x0005;

// helper types

#ifndef DOXYGEN
struct Data_string_serialized_prolog {
	int version;
	time_t expires;
};
#endif

void cache_delete(const String& file_spec) {
	file_delete(file_spec, false/*fail_on_read_problem*/);
}

#ifndef DOXYGEN
class Cache_data: public PA_Object {
public:
	time_t expires;
};
struct Locked_process_and_cache_put_action_info {
	Request *r;
	Cache_data *data;
	Value* body_code; const String* evaluated_body;
};
#endif
/* @todo maybe network order worth spending some effort?
	don't bothering myself with network byte order,
	am not planning to be able to move resulting file across platforms
*/
static void locked_process_and_cache_put_action(int f, void *context) {
	Locked_process_and_cache_put_action_info& info=
		*static_cast<Locked_process_and_cache_put_action_info *>(context);

	// body->process 
	info.evaluated_body=&info.r->process_to_string(*info.body_code);

	// expiration time not spoiled by ^cache(0) or something?
	if(info.data->expires > time(0)) {
		// string -serialize> buffer
		String::Cm serialized=info.evaluated_body->serialize(
			sizeof(Data_string_serialized_prolog));
		Data_string_serialized_prolog& prolog=
			*reinterpret_cast<Data_string_serialized_prolog *>(serialized.str);
		prolog.version=DATA_STRING_SERIALIZED_VERSION;
		prolog.expires=info.data->expires;
		
		// buffer -write> file
		write(f, serialized.str, serialized.length);
	} else // expired!
		info.data->expires=0; // flag it so that could be easily checked by caller
}
const String* locked_process_and_cache_put(Request& r, 
					   Value& body_code,
					   Cache_data& data,
					   const String& file_spec) {
	Locked_process_and_cache_put_action_info info={0};
	info.r=&r;
	info.data=&data;
	info.body_code=&body_code;

	const String* result=file_write_action_under_lock(
		file_spec, 
		"cache_put", locked_process_and_cache_put_action, &info,
		false/*as_text*/,
		false/*do_append*/,
		false/*block*/,
		false/*fail on lock problem*/) ? info.evaluated_body: 0;
	time_t now=time(0);
	if(data.expires<=now)
		cache_delete(file_spec);
	return result;
}
const String* cache_get(Request_charsets& charsets, const String& file_spec, time_t now) {
	File_read_result file=file_read(charsets, file_spec, 
			   false/*as_text*/, 
			   0, //no params
			   false/*fail_on_read_problem*/);
	if(file.success && file.length/* ignore reads which are empty due to 
			non-unary open+lockEX conflict with lockSH */) {
			
		Data_string_serialized_prolog& prolog=
			*reinterpret_cast<Data_string_serialized_prolog *>(file.str);

		String* result=new String;
		if(
			file.length>=sizeof(Data_string_serialized_prolog)
			&& prolog.version==DATA_STRING_SERIALIZED_VERSION
			&& prolog.expires > now
			&& result->deserialize(sizeof(Data_string_serialized_prolog),  file.str, file.length))
			return result;
	}

	return 0;
}
static time_t as_expires(Request& r, MethodParams& params, 
						int index, time_t now) {
	time_t result;
	if(Value* vdate=params[index].as(VDATE_TYPE, false))
		result=static_cast<VDate*>(vdate)->get_time();
	else
		result=now+(time_t)params.as_double(index, "lifespan must be date or number", r);
	
	return result;
}
static const String& as_file_spec(Request& r, MethodParams& params, int index) {
	return r.absolute(params.as_string(index, "filespec must be string"));
}
static void _cache(Request& r, MethodParams& params) {
	time_t now=time(0);

	// ^cache[filename] ^cache(seconds) ^cache[expires date]
	if(params.count()==1) {
		if(params[0].is_string()) { // filename?
			cache_delete(as_file_spec(r, params, 0));
			return;
		}

		// secods|expires date
		Cache_data* data=static_cast<Cache_data*>(r.classes_conf.get(cache_data_name));
		if(!data)
			throw Exception("parser.runtime",
				0,
				"expire-time reducing instruction without cache");
		
		time_t expires=as_expires(r, params, 0, now);
		if(expires < data->expires)
			data->expires=expires;

		return;
	}
	
	// file_spec, expires, body code
	const String& file_spec=r.absolute(params.as_string(0, "filespec must be string"));

	Cache_data* data=new Cache_data;
	Temp_hash_value<const String::Body, PA_Object*> 
		cache_data_setter(r.classes_conf, cache_data_name, data);
	data->expires=as_expires(r, params, 1, now);
	Value& body_code=params.as_junction(2, "body must be code");

	if(data->expires>now) { // valid 'expires' specified? try cached copy...
		// hence we don't hope to have unary create/lockEX
		// we need some plan to live in a life like that, so... 
		// worst races plan:
		// A        B
		// open
		//          |open
		// lockSH
		//          |nonblocking-lockEX fails
		// unlockSH
		// close, cache_get returns 0
		// open
		// nonblocking-lockEX succeeds; process, write, close
		//          |retry1: open
		// ...
		//          |lockSH succeeds; ...

		for(int retry=0; retry<2; retry++) {
			if(const String* cached_body=cache_get(r.charsets, file_spec, now)) { // have cached copy?
				// write it out 
				r.write_assign_lang(*cached_body);
				// happy with it
				return;
			}

			// non-blocked lock; process; cache it
			if(const String* processed_body=
				locked_process_and_cache_put(r, body_code, *data, file_spec)) {
				// write it out 
				r.write_assign_lang(*processed_body);
				// happy with it
				return;
			} else { // somebody writing result right now
				pa_sleep(0, 500000); // waiting half a second
				retry=0; // prolonging our wait, than could cache_get it, without processing body_code
			}
		}
		throw Exception(0,
			&file_spec,
			"locking problem");
	} else { 
		// instructed not to cache; forget cached copy
		cache_delete(file_spec);
		// process
		const String& processed_body=r.process_to_string(body_code);
		// write it out 
		r.write_assign_lang(processed_body);
		// happy with it
		return;
	}
	// never reached
}



// also used in pa_request.C to pass param to @unhandled_exception

static void _try_operator(Request& r, MethodParams& params) {
	Value& body_code=params.as_junction(0, "body_code must be code");
	Value& catch_code=params.as_junction(1, "catch_code must be code");

	StringOrValue result;
	// taking snapshot of try-context
	Request_context_saver try_context(r);
	try {
		result=r.process(body_code);
	} catch(const Exception& e) {
		Request::Exception_details details=r.get_details(e);

		Request_context_saver throw_context(r); // taking snapshot of throw-context [stack trace contains error]
		try_context.restore(); // restoring try-context to perform catch-code

		Junction* junction=catch_code.get_junction();
		Value* method_frame=junction->method_frame;
		Value* saved_exception_var_value=method_frame->get_element(exception_var_name, *method_frame, false);
		junction->method_frame->put_element(exception_var_name, &details.vhash, false);
		result=r.process(catch_code);
		bool handled=false;
		if(Value* value=
			details.vhash.hash().get(exception_handled_part_name))
			handled=value->as_bool();		
		junction->method_frame->put_element(exception_var_name, saved_exception_var_value, false);

		if(!handled) {
			throw_context.restore(); // restoring throw-context [exception were not handled]
			rethrow;
		}
	}
	// write out result
	r.write_pass_lang(result);
}

static void _throw_operator(Request& r, MethodParams& params) {
	if(params.count()==1) {
		if(HashStringValue *hash=params[0].get_hash()) {
			const char* type=0;
			if(Value* value=hash->get(exception_type_part_name))
				type=value->as_string().cstr();
			const String* source=0;
			if(Value* value=hash->get(exception_source_part_name))
				source=&value->as_string();
			const char* comment;
			if(Value* value=hash->get(exception_comment_part_name))
				comment=value->as_string().cstr();

			throw Exception(type,
				source?source:0,
				"%s", comment?comment:"");
		} else
			throw Exception("parser.runtime",
				0,
				"one-param version has hash param");
	} else {
		const char* type=params.as_string(0, "type must be string").cstr();
		const String& source=params.as_string(1, "source must be string");
		const char* comment=params.count()>2? params.as_string(2, "comment must be string").cstr()
			:0;
		throw Exception(type, &source, "%s", comment?comment:"");
	}
 }

#if defined(WIN32) && defined(_DEBUG)
#	define PA_BPT
static void _bpt(Request& r, MethodParams& params) {
	_asm int 3;
}
#endif

// constructor

VClassMAIN::VClassMAIN(): VClass() {
	set_name(*new String(MAIN_CLASS_NAME));

#ifdef PA_BPT
	// ^bpt[]
	add_native_method("bpt", Method::CT_ANY, _bpt, 0, 0);
#endif

	// ^if(condition){code-when-true}
	// ^if(condition){code-when-true}{code-when-false}
	add_native_method("if", Method::CT_ANY, _if, 2, 3);

	// ^untaint[as-is|uri|sql|js|html|html-typo]{code}
	add_native_method("untaint", Method::CT_ANY, _untaint, 1, 2);

	// ^taint[as-is|uri|sql|js|html|html-typo]{code}
	add_native_method("taint", Method::CT_ANY, _taint, 1, 2);

	// ^process[code]
	add_native_method("process", Method::CT_ANY, _process, 1, 2);

	// ^rem{code}
	add_native_method("rem", Method::CT_ANY, _rem, 1, 10000);

	// ^while(condition){code}
	add_native_method("while", Method::CT_ANY, _while, 2, 2);

	// ^use[file]
	add_native_method("use", Method::CT_ANY, _use, 1, 1);

	// ^for[i](from-number;to-number-inclusive){code}[delim]
	add_native_method("for", Method::CT_ANY, _for, 3+1, 3+1+1);

	// ^eval(expr)
	// ^eval(expr)[format]
	add_native_method("eval", Method::CT_ANY, _eval, 1, 2);

	// ^connect[protocol://user:pass@host[:port]/database]{code with ^sql-s}
	add_native_method("connect", Method::CT_ANY, _connect, 2, 2);


	// ^cache[file_spec] delete cache
	// ^cache[file_spec](time){code} time=0 no cache
	add_native_method("cache", Method::CT_ANY, _cache, 1, 3);
	
	// switch

	// ^switch[value]{cases}
	add_native_method("switch", Method::CT_ANY, _switch, 2, 2);

	// ^case[value]{code}
	add_native_method("case", Method::CT_ANY, _case, 2, 10000);

	// try-catch

	// ^try{code}{catch code}
	add_native_method("try", Method::CT_ANY, _try_operator, 2, 2);
	// ^throw[$exception hash]
	// ^throw[type;source;comment]
	add_native_method("throw", Method::CT_ANY, _throw_operator, 1, 3);

}

// constructor & configurator

VStateless_class& VClassMAIN_create() {
	return *new VClassMAIN;
}
