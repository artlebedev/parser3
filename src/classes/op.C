/** @file
	Parser: parser @b operators.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_OP_C="$Date: 2007/08/20 10:02:51 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_common.h"
#include "pa_os.h"
#include "pa_request.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"
#include "pa_vdate.h"
#include "pa_vmethod_frame.h"
#include "pa_vclass.h"
#include "pa_charset.h"

// limits

#define MAX_LOOPS 20000

// defines

#define CASE_DEFAULT_VALUE "DEFAULT"
#define PROCESS_MAIN_OPTION_NAME "main"
#define PROCESS_FILE_OPTION_NAME "file"
#define PROCESS_LINENO_OPTION_NAME "lineno"

// class

class VClassMAIN: public VClass {
public:
	VClassMAIN();
};

// defines for globals

#define CYCLE_DATA_NAME "CYCLE-DATA"

// globals

//^for & co
String cycle_data_name(CYCLE_DATA_NAME);

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


// local defines

#define CACHE_EXCEPTION_HANDLED_CACHE_NAME "cache"

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
		ULN("sql", SQL);
		ULN("js", JS);
		ULN("xml", XML);
		ULN("optimized-xml", XML|String::L_OPTIMIZE_BIT);
		ULN("html", HTML);
		ULN("optimized-html", HTML|String::L_OPTIMIZE_BIT);
		ULN("regex", REGEX);
		#undef ULN
	}
} untaint_lang_name2enum;

// methods

static void _if(Request& r, MethodParams& params) {
	bool condition=params.as_bool(0, "condition must be expression", r);
	if(condition)
		r.write_pass_lang(r.process(*params.get(1)));
	else if(params.count()>2)
		r.write_pass_lang(r.process(*params.get(2)));
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
			throw Exception(PARSER_RUNTIME,
				0,
				"no target class");

		// temporary remove language change
		Temp_lang temp_lang(r, String::L_PASS_APPENDED);
		// temporary zero @main so to maybe-replace it in processed code
		Temp_method temp_method_main(*target_class, main_method_name, 0);
		// temporary zero @auto so it wouldn't be auto-called in Request::use_buf
		Temp_method temp_method_auto(*target_class, auto_method_name, 0);

		size_t options_index=index+1;
		HashStringValue* options=0;
		if(options_index<params.count()) {
			Value& voptions=params.as_no_junction(options_index, "options must not be code");
			options=voptions.get_hash();
			if(!options)
				throw Exception(PARSER_RUNTIME,
					0,
					"options must be hash");
		}

		const String* main_alias=0;
		const String* file_alias=0;
		int line_no_alias_offset=0;
		if(options) {
			int valid_options=0;
			if(Value* vmain_alias=options->get(PROCESS_MAIN_OPTION_NAME)) {
				valid_options++;
				main_alias=&vmain_alias->as_string();
			}
			if(Value* vfile_alias=options->get(PROCESS_FILE_OPTION_NAME)) {
				valid_options++;
				file_alias=&vfile_alias->as_string();
			}
			if(Value* vline_no_alias_offset=options->get(PROCESS_LINENO_OPTION_NAME)) {
				valid_options++;
				line_no_alias_offset=vline_no_alias_offset->as_int();
			}
	
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME,
					0,
					"called with invalid option");
		}

		uint processe_file_no=file_alias?
			r.register_file(r.absolute(*file_alias))
			: pseudo_file_no__process;
		// process...{string}
		Value& vjunction=params.as_junction(index, "body must be code");
		// evaluate source to process
		const String& source=r.process_to_string(vjunction);
		r.use_buf(*target_class,
			source.cstr(String::L_UNSPECIFIED, r.connection(false)),
			main_alias,
			processe_file_no,
			line_no_alias_offset);

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
	
static void _rem(Request&, MethodParams& params) {
	params.as_junction(0, "body must be code");
}

static void _while(Request& r, MethodParams& params) {
	Temp_hash_value<const String::Body, void*> 
		cycle_data_setter(r.classes_conf, cycle_data_name, /*any not null flag*/&r);

	Value& vcondition=params.as_expression(0, "condition must be number, bool or expression");

	Value& body_code=params.as_junction(1, "body must be code");
	Value* delim_maybe_code=params.count()>2?&params[2]:0;

	// while...
	int endless_loop_count=0;
	bool need_delim=false;
	while(true) {
		if(++endless_loop_count>=MAX_LOOPS) // endless loop?
			throw Exception(PARSER_RUNTIME,
				0,
				"endless loop detected");

		bool condition=r.process_to_value(vcondition, 
				false/*don't intercept string*/).as_bool();
		if(!condition) // ...condition is true
			break;

		StringOrValue sv_processed=r.process(body_code);
		Request::Skip lskip=r.get_skip(); r.set_skip(Request::SKIP_NOTHING);
		const String* s_processed=sv_processed.get_string();
		if(delim_maybe_code && s_processed && s_processed->length()) { // delimiter set and we have body
			if(need_delim) // need delim & iteration produced string?
				r.write_pass_lang(r.process(*delim_maybe_code));
			need_delim=true;
		}
		r.write_pass_lang(sv_processed);

		if(lskip==Request::SKIP_BREAK)
			break;
	}
}

static void _use(Request& r, MethodParams& params) {
	Value& vfile=params.as_no_junction(0, FILE_NAME_MUST_NOT_BE_CODE);
	r.use_file(r.main_class, vfile.as_string());
}

static void set_skip(Request& r, Request::Skip askip) {
	void* data=r.classes_conf.get(cycle_data_name);
	if(!data)
		throw Exception(PARSER_RUNTIME,
			0,
			"without cycle");

	r.set_skip(askip);
}

static void _break(Request& r, MethodParams&) {
	set_skip(r, Request::SKIP_BREAK);
}

static void _continue(Request& r, MethodParams&) {
	set_skip(r, Request::SKIP_CONTINUE);
}

static void _for(Request& r, MethodParams& params) {
	Temp_hash_value<const String::Body, void*> 
		cycle_data_setter(r.classes_conf, cycle_data_name, /*any not null flag*/&r);

	const String& var_name=params.as_string(0, "var name must be string");
	int from=params.as_int(1, "from must be int", r);
	int to=params.as_int(2, "to must be int", r);
	Value&  body_code=params.as_junction(3, "body must be code");
	Value* delim_maybe_code=params.count()>4?&params[4]:0;

	if(to-from>=MAX_LOOPS) // too long loop?
		throw Exception(PARSER_RUNTIME,
			0,
			"endless loop detected");

	bool need_delim=false;
	VInt* vint=new VInt(0);

	VMethodFrame& caller=*r.get_method_frame()->caller();
	caller.put_element(caller, var_name, vint, false);
	for(int i=from; i<=to; i++) {
		vint->set_int(i);

		StringOrValue sv_processed=r.process(body_code);
		Request::Skip lskip=r.get_skip(); r.set_skip(Request::SKIP_NOTHING);
		const String* s_processed=sv_processed.get_string();
		if(delim_maybe_code && s_processed && s_processed->length()) { // delimiter set and we have body
			if(need_delim) // need delim & iteration produced string?
				r.write_pass_lang(r.process(*delim_maybe_code));
			need_delim=true;
		}
		r.write_pass_lang(sv_processed);

		if(lskip==Request::SKIP_BREAK)
			break;
	}
}

static void _eval(Request& r, MethodParams& params) {
	Value& expr=params.as_junction(0, "need expression");
	// evaluate expresion
	Value& value_result=r.process_to_value(expr, 
		false/*don't intercept string*/).as_expr_result();
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
	SQL_Connection* connection=SQL_driver_manager->get_connection(url.as_string(), 
		protocol2driver_and_client,
		r.charsets.source().NAME().cstr());

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
	Temp_hash_value<const String::Body, void*> 
		switch_data_setter(r.classes_conf, switch_data_name, data);

	Value& cases_code=params.as_junction(1, "switch cases must be code");
	// execution of found ^case[...]{code} must be in context of ^switch[...]{code}
	// because of stacked WWrapper used there as wcontext
	r.process(cases_code, true/*intercept_string*/);
	if(Value* selected_code=data->found? data->found: data->_default)
		r.write_pass_lang(r.process(*selected_code));
}

static void _case(Request& r, MethodParams& params) {
	Switch_data* data=static_cast<Switch_data*>(r.classes_conf.get(switch_data_name));
	if(!data)
		throw Exception(PARSER_RUNTIME,
			0,
			"without switch");

	int count=params.count();
	Value& code=params.as_junction(--count, "case result must be code");
	
	Value& searching=data->searching;
	bool we_are_searching_string_or_void=searching.is_string() || searching.is_void();
	for(int i=0; i<count; i++) {
		Value& value=r.process_to_value(params[i]);

		if(value.is_string() && value.as_string() == CASE_DEFAULT_VALUE) {
			data->_default=&code;
			break;
		}

		bool matches;
		if(we_are_searching_string_or_void)
			matches=searching.as_string() == value.as_string();
		else
			matches=searching.as_double() == value.as_double();

		if(matches) {
			if(data->found)
				throw Exception(PARSER_RUNTIME,
					0,
					"duplicate found");

			data->found=&code;
			break;
		}
	}
}
#ifndef DOXYGEN
struct Try_catch_result {
	StringOrValue processed_code;
	const String* exception_should_be_handled;

	Try_catch_result(): exception_should_be_handled(0) {}
};
#endif

/// used by ^try and ^cache, @returns $exception.handled[string] if any
template<class I>
static Try_catch_result try_catch(Request& r, 
		  StringOrValue body_code(Request&, I), I info, 
		  Value* catch_code, bool could_be_handled_by_caller=false) 
{
	Try_catch_result result;
	if(!catch_code) {
		result.processed_code=body_code(r, info);
		return result;
	}

	// taking snapshot of try-context
	Request_context_saver try_context(r);
	try {
		result.processed_code=body_code(r, info);
	} catch(const Exception& e) {
		Request_context_saver throw_context(r); // taking snapshot of throw-context [stack trace contains error]
		Request::Exception_details details=r.get_details(e);
		try_context.restore(); // restoring try-context to perform catch-code

		Junction* junction=catch_code->get_junction();
		Value* method_frame=junction->method_frame;
		Value* saved_exception_var_value=method_frame->get_element(exception_var_name, *method_frame, false);
		VMethodFrame& frame=*junction->method_frame;
		frame.put_element(frame, exception_var_name, &details.vhash, false);
		result.processed_code=r.process(*catch_code);
		
		// retriving $exception.handled, restoring $exception var
		Value* vhandled=details.vhash.hash().get(exception_handled_part_name);
		frame.put_element(frame, exception_var_name, saved_exception_var_value, false);

		bool bhandled=false;
		if(vhandled) {
			if(vhandled->is_string()) { // not simple $exception.handled(1/0)?
				if(could_be_handled_by_caller) { // and we can possibly handle it
					result.exception_should_be_handled=vhandled->get_string(); // considering 'recovered' and let the caller recover
					return result;
				}
				
				bhandled=false;
			} else
				bhandled=vhandled->as_bool();		
		}

		if(!bhandled) {
			throw_context.restore(); // restoring throw-context [exception were not handled]
			rethrow;
		}
	}
	return result;
}

// cache--

// consts

const int DATA_STRING_SERIALIZED_VERSION=0x0006;

// helper types

#ifndef DOXYGEN
struct Data_string_serialized_prolog {
	int version;
	time_t expires;
};
#endif

void cache_delete(const String& file_spec) {
	file_delete(file_spec, false/*fail_on_problem*/);
}

#ifndef DOXYGEN
struct Cache_scope {
public:
	time_t expires;
	const String* body_from_disk;
};
struct Locked_process_and_cache_put_action_info {
	Request *r;
	Cache_scope *scope;
	Value* body_code;
	Value* catch_code; 
	const String* processed_code;
};
#endif



static StringOrValue process_cache_body_code(Request& r, Value* body_code) {
	return StringOrValue(r.process_to_string(*body_code));
}

/* @todo maybe network order worth spending some effort?
	don't bothering myself with network byte order,
	am not planning to be able to move resulting file across platforms
*/
static void locked_process_and_cache_put_action(int f, void *context) {
	Locked_process_and_cache_put_action_info& info=
		*static_cast<Locked_process_and_cache_put_action_info *>(context);

	const String* body_from_disk=info.scope->body_from_disk;
	// body->process 
	Try_catch_result result=try_catch(*info.r, 
		process_cache_body_code, info.body_code,
		info.catch_code, body_from_disk!=0 /*we have something old=we can handle=recover later*/);

	if(result.exception_should_be_handled) {
		if(*result.exception_should_be_handled==CACHE_EXCEPTION_HANDLED_CACHE_NAME) {
			assert(body_from_disk);
			info.processed_code=body_from_disk;
		} else
			throw Exception(PARSER_RUNTIME,
				result.exception_should_be_handled,
				"$"EXCEPTION_VAR_NAME"."EXCEPTION_HANDLED_PART_NAME" value must be "
				"either boolean or string '"CACHE_EXCEPTION_HANDLED_CACHE_NAME"'");
	} else
		info.processed_code=&result.processed_code.as_string();

	// expiration time not spoiled by ^cache(0) or something?
	if(info.scope->expires > time(0)) {
		// string -serialize> buffer
		String::Cm serialized=info.processed_code->serialize(
			sizeof(Data_string_serialized_prolog));
		Data_string_serialized_prolog& prolog=
			*reinterpret_cast<Data_string_serialized_prolog *>(serialized.str);
		prolog.version=DATA_STRING_SERIALIZED_VERSION;
		prolog.expires=info.scope->expires;
		
		// buffer -write> file
		write(f, serialized.str, serialized.length);
	} else // expired!
		info.scope->expires=0; // flag it so that could be easily checked by caller
}
const String* locked_process_and_cache_put(Request& r, 
					   Value& body_code,
					   Value* catch_code,
					   Cache_scope& scope,
					   const String& file_spec) 
{
	Locked_process_and_cache_put_action_info info={&r, &scope, &body_code, catch_code, 0};

	const String* result=file_write_action_under_lock(
		file_spec, 
		"cache_put",
		locked_process_and_cache_put_action,
		&info,
		false/*as_text*/,
		false/*do_append*/,
		false/*block == don't wait till other thread release lock*/) ? info.processed_code: 0;

	time_t now=time(0);
	if(scope.expires<=now)
		cache_delete(file_spec);
	return result;
}
#ifndef DOXYGEN
struct Cache_get_result {
	const String* body;
	bool expired;
};
#endif
static Cache_get_result cache_get(Request_charsets& charsets, const String& file_spec, time_t now) {
	Cache_get_result result={0, false};

	File_read_result file=file_read(charsets, file_spec, 
			   false/*as_text*/, 
			   0, //no params
			   false/*fail_on_read_problem*/);
	if(file.success && file.length/* ignore reads which are empty due to 
			non-unary open+lockEX conflict with lockSH */) {
			
		Data_string_serialized_prolog& prolog=
			*reinterpret_cast<Data_string_serialized_prolog *>(file.str);

		String* body=new String;
		if(
			file.length>=sizeof(Data_string_serialized_prolog)
			&& prolog.version==DATA_STRING_SERIALIZED_VERSION) {
			if(body->deserialize(sizeof(Data_string_serialized_prolog),  file.str, file.length)) {
				result.body=body;
				result.expired=prolog.expires <= now;
			}
		}
	}

	return result;
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
	if(params.count()==0)
	{
		// return current expiration time
		Cache_scope* scope=static_cast<Cache_scope*>(r.classes_conf.get(cache_data_name));
		if(!scope)
			throw Exception(PARSER_RUNTIME,
				0,
				"expire-time get without cache");
		r.write_no_lang(*new VDate(scope->expires));
		return;
	}

	time_t now=time(0);

	// ^cache[filename] ^cache(seconds) ^cache[expires date]
	if(params.count()==1) {
		if(params[0].is_string()) { // filename?
			cache_delete(as_file_spec(r, params, 0));
			return;
		}

		// secods|expires date
		Cache_scope* scope=static_cast<Cache_scope*>(r.classes_conf.get(cache_data_name));
		if(!scope)
			throw Exception(PARSER_RUNTIME,
				0,
				"expire-time reducing instruction without cache");
		
		time_t expires=as_expires(r, params, 0, now);
		if(expires < scope->expires)
			scope->expires=expires;

		return;
	} else if(params.count()<3)
		throw Exception(PARSER_RUNTIME,
			0,
			"invalid number of parameters"); 
	
	// file_spec, expires, body code
	const String& file_spec=r.absolute(params.as_string(0, "filespec must be string"));

	Cache_scope scope={as_expires(r, params, 1, now), 0};

	Temp_hash_value<const String::Body, void*> 
		cache_scope_setter(r.classes_conf, cache_data_name, &scope);
	Value& body_code=params.as_junction(2, "body_code must be code");
	Value* catch_code=0;
	if(params.count()>3)
		catch_code=&params.as_junction(3, "catch_code must be code");

	if(scope.expires>now) {
		Cache_get_result cached=cache_get(r.charsets, file_spec, now);

		if(cached.body) { // have cached copy
		if(cached.expired) {
			scope.body_from_disk=cached.body; // storing for user to retrive it with ^cache[]
		} else {
			// and it's not expired yet write it out 
   				r.write_assign_lang(*cached.body);
   				// happy with it
   				return;
   			}
		}

		// no cached info or it's already expired
		try {
			// try to process and store in file
   			const String* processed_body=locked_process_and_cache_put(r, body_code, catch_code, scope, file_spec);
   			// write it out 
   			r.write_assign_lang(*processed_body);
   			// happy with it
   			return;
		} catch(...) {
			// we fail during get exclusive lock
			// nvm we just process it a bit later 
   		}
	} else { 
		// instructed not to cache; forget cached copy
		cache_delete(file_spec);
	}
	
	// process without cacheing
	const String& processed_body=r.process_to_string(body_code);
	// write it out 
	r.write_assign_lang(processed_body);
}

static StringOrValue process_try_body_code(Request& r, Value* body_code) {
	return r.process(*body_code);
}
static void _try_operator(Request& r, MethodParams& params) {
	Value& body_code=params.as_junction(0, "body_code must be code");
	Value& catch_code=params.as_junction(1, "catch_code must be code");

	Try_catch_result result=try_catch(r, 
		process_try_body_code, &body_code,
		&catch_code);
	
	if(result.exception_should_be_handled)
		throw Exception(PARSER_RUNTIME,
			result.exception_should_be_handled,
			"catch block must set $exception.handled to some boolean value, not string");

	// write out processed body_code or catch_code
	r.write_pass_lang(result.processed_code);
}

static void _throw_operator(Request&, MethodParams& params) {
	if(params.count()==1) {
		if(HashStringValue *hash=params[0].get_hash()) {
			const char* type=0;
			if(Value* value=hash->get(exception_type_part_name))
				type=value->as_string().cstr();
			const String* source=0;
			if(Value* value=hash->get(exception_source_part_name))
				source=&value->as_string();
			const char* comment=0;
			if(Value* value=hash->get(exception_comment_part_name))
				comment=value->as_string().cstr();

			throw Exception(type,
				source?source:0,
				"%s", comment?comment:"");
		} else
			throw Exception(PARSER_RUNTIME,
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

static void _sleep_operator(Request& r, MethodParams& params) {
	double seconds=params.as_double(0, "seconds must be double", r);
	pa_sleep((int)trunc(seconds), (int)trunc(seconds*1000));
 }

#if defined(WIN32) && defined(_DEBUG)
#	define PA_BPT
static void _bpt(Request&, MethodParams&) {
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
	add_native_method("process", Method::CT_ANY, _process, 1, 3);

	// ^rem{code}
	add_native_method("rem", Method::CT_ANY, _rem, 1, 10000);

	// ^while(condition){code}
	add_native_method("while", Method::CT_ANY, _while, 2, 3);

	// ^use[file]
	add_native_method("use", Method::CT_ANY, _use, 1, 1);

	// ^break[]
	add_native_method("break", Method::CT_ANY, _break, 0, 0);
	// ^continue[]
	add_native_method("continue", Method::CT_ANY, _continue, 0, 0);
	// ^for[i](from-number;to-number-inclusive){code}[delim]
	add_native_method("for", Method::CT_ANY, _for, 3+1, 3+1+1);

	// ^eval(expr)
	// ^eval(expr)[format]
	add_native_method("eval", Method::CT_ANY, _eval, 1, 2);

	// ^connect[protocol://user:pass@host[:port]/database]{code with ^sql-s}
	add_native_method("connect", Method::CT_ANY, _connect, 2, 2);


	// ^cache[file_spec](time){code}[{catch code}] time=0 no cache
	// ^cache[file_spec] delete cache
	// ^cache[] get current expiration time
	add_native_method("cache", Method::CT_ANY, _cache, 0, 4);
	
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

	add_native_method("sleep", Method::CT_ANY, _sleep_operator, 1, 1);
}

// constructor & configurator

VStateless_class& VClassMAIN_create() {
	return *new VClassMAIN;
}
