/** @file
	Parser: parser @b operators.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

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

volatile const char * IDENT_OP_C="$Id: op.C,v 1.260 2020/12/17 16:50:53 moko Exp $";

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


// local defines

#define CACHE_EXCEPTION_HANDLED_CACHE_NAME "cache"

// helpers

class Untaint_lang_name2enum: public HashString<String::Language> {
public:
	Untaint_lang_name2enum() {
		#define ULN(name, LANG) put(name, (value_type)(String::L_##LANG));
		ULN("clean", CLEAN);
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
		ULN("parser-code", PARSER_CODE);
		ULN("json", JSON);
		#undef ULN
	}
} untaint_lang_name2enum;

// methods

static void _if(Request& r, MethodParams& params) {
	size_t max_param=params.count()-1;
	size_t i=0;
	do {
		bool condition=params.as_bool(i, "condition must be expression", r);
		if(condition) {
			r.process_write(params[i+1]);
			return;
		}
		i+=2;
	} while (i < max_param);

	if(i == max_param)
		r.process_write(params[i]);
}

String::Language get_untaint_lang(const String& lang_name){
	String::Language lang=untaint_lang_name2enum.get(lang_name);
	if(!lang)
		throw Exception(PARSER_RUNTIME,	&lang_name, "invalid taint language");
	return lang;
}

static void _untaint(Request& r, MethodParams& params) {
	String::Language lang;
	if(params.count()==1)
		lang=String::L_AS_IS; // mark as simply 'as-is'. useful in html from sql
	else
		lang=get_untaint_lang(params.as_string(0, "lang must be string"));

	Value& vbody=params.as_junction(params.count()-1, "body must be code");
	Value& result=r.process(vbody);

	if(const String* string=result.get_string()){
		String &untainted=*new String();
		string->append_to(untainted, lang); // mark all tainted to specified language
		r.write(untainted);
	} else
		r.write(result); // this is not normal, just backward compatibility
}

static void _taint(Request& r, MethodParams& params) {
	String::Language lang;
	if(params.count()==1)
		lang=String::L_TAINTED; // mark as simply 'tainted'. useful in table:create
	else
		lang=get_untaint_lang(params.as_string(0, "lang must be string"));

	{
		Value& vbody=params.as_no_junction(params.count()-1, "body must not be code");
		
		String result(vbody.as_string(), lang); // force result language to specified
		r.write(result);
	}
}

static void _apply_taint(Request& r, MethodParams& params) {
	String::Language lang=params.count()==1 ? String::L_AS_IS : get_untaint_lang(params.as_string(0, "lang must be string"));
	const String &sbody=params.as_string(params.count()-1, "body must be string");
	String::Body result_body=sbody.cstr_to_string_body_untaint(lang, r.connection(false), &r.charsets);
	r.write(*new String(result_body, String::L_AS_IS));
}

static void _process(Request& r, MethodParams& params) {
	Method* main_method;

	size_t index=0;
	Value* target_self;
	Value& maybe_target_self=params[index];
	if(maybe_target_self.get_string() || maybe_target_self.get_junction())
		target_self=&r.get_method_frame()->caller()->self();
	else {
		target_self=&maybe_target_self;
		if(params.count()==1)
			throw Exception(PARSER_RUNTIME, 0, "no body specified");
		index++;
	}

	{
		VStateless_class *target_class=target_self->get_class();
		if(!target_class)
			throw Exception(PARSER_RUNTIME, 0, "no target class");

		// temporary zero @main so to maybe-replace it in processed code
		Temp_method temp_method_main(*target_class, main_method_name, 0);

		const String* main_alias=0;
		const String* file_alias=0;
		int line_no_alias_offset=0;
		bool allow_class_replace=false;

		size_t options_index=index+1;
		if(options_index<params.count())
			if(HashStringValue* options=params.as_hash(options_index)) {

				int valid_options=0;
				for(HashStringValue::Iterator i(*options); i; i.next() ){

					String::Body key=i.key();
					Value* value=i.value();

					if(key == "main") {
						valid_options++;
						main_alias=&value->as_string();
					} else if(key == "file") {
						valid_options++;
						file_alias=&value->as_string();
					} else if(key == "lineno") {
						valid_options++;
						line_no_alias_offset=value->as_int();
					} else if(key == "replace") {
						valid_options++;
						allow_class_replace=r.process(*value).as_bool();
					}
				}

				if(valid_options!=options->count())
					throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			}

		uint processe_file_no=file_alias ? r.register_file(r.full_disk_path(*file_alias)) : pseudo_file_no__process;
		// process...{string}
		Value& vjunction=params.as_junction(index, "body must be code");
		// evaluate source to process
		const String& source=r.process_to_string(vjunction);

		Temp_class_replace class_replace(r, allow_class_replace);

		r.use_buf(*target_class, source.untaint_cstr(String::L_PARSER_CODE, r.connection(false)), main_alias, processe_file_no, line_no_alias_offset);

		// main_method
		main_method=target_class->get_method(main_method_name);
	}
	// after restoring current-request-lang
	// maybe-execute @main[]
	if(main_method) {
		METHOD_FRAME_ACTION(*main_method, r.get_method_frame()->caller(), *target_self, {
			frame.empty_params();
			r.call(frame);
			r.write(frame.result());
		});
	}
}
	
static void _rem(Request&, MethodParams& params) {
	params.as_junction(0, "body must be code");
}

static void _while(Request& r, MethodParams& params) {
	InCycle temp(r);

	Value& vcondition=params.as_expression(0, "condition must be number, bool or expression");

	Value& body_code=params.as_junction(1, "body must be code");
	Value* delim_maybe_code=params.count()>2?&params[2]:0;

	// while...
	int endless_loop_count=0;
	if(delim_maybe_code){ // delimiter set
		bool need_delim=false;
		while(true) {
			if(++endless_loop_count>=pa_loop_limit) // endless loop?
				throw Exception(PARSER_RUNTIME, 0, "endless loop detected");

			if(!r.process(vcondition).as_bool())
				break;

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
		while(true) {
			if(++endless_loop_count>=pa_loop_limit) // endless loop?
				throw Exception(PARSER_RUNTIME, 0, "endless loop detected");

			if(!r.process(vcondition).as_bool())
				break;

			r.process_write(body_code);

			if(r.check_skip_break())
				break;
		}
	}
}

static void _use(Request& r, MethodParams& params) {
	Value& vfile=params.as_no_junction(0, FILE_NAME_MUST_NOT_BE_CODE);

	const String* use_origin=0;
	bool allow_class_replace=false;
	bool load_auto_p=false;

	if(params.count()==2)
		if(HashStringValue* options=params.as_hash(1)) {
			int valid_options=0;
			for(HashStringValue::Iterator i(*options); i; i.next() ){

				String::Body key=i.key();
				Value* value=i.value();

				if(key == "origin") {
					valid_options++;
					use_origin=&value->as_string();
				}

				if(key == "replace") {
					valid_options++;
					allow_class_replace=r.process(*value).as_bool();
				}

				if(key == "main") {
					valid_options++;
					load_auto_p=r.process(*value).as_bool();
				}

				if(valid_options!=options->count())
					throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
			}
		}

	if(!use_origin)
		if(VMethodFrame* caller=r.get_method_frame()->caller())
			use_origin=r.get_method_filespec(&caller->method);

	Temp_class_replace class_replace(r, allow_class_replace);

	r.use_file(vfile.as_string(), use_origin, load_auto_p);
}

static void set_skip(Request& r, Request::Skip askip) {
	if(!r.get_in_cycle())
		throw Exception(askip==Request::SKIP_BREAK ? "parser.break" : "parser.continue", 0, "without cycle");
	r.set_skip(askip);
}

static void _break(Request& r, MethodParams& params) {
	if(!params.count() || params.as_bool(0, "condition must be expression", r)) set_skip(r, Request::SKIP_BREAK);
}

static void _continue(Request& r, MethodParams& params) {
	if(!params.count() || params.as_bool(0, "condition must be expression", r)) set_skip(r, Request::SKIP_CONTINUE);
}

static void _return(Request& r, MethodParams& params) {
	VMethodFrame& caller=*r.get_method_frame()->caller();
	if(params.count())
		r.put_element(caller, Symbols::RESULT_SYMBOL, &r.process(params[0]));
	r.set_skip_return(caller);
}

static void _for(Request& r, MethodParams& params) {
	InCycle temp(r);

	const String& var_name=params.as_string(0, "var name must be string");
	int from=params.as_int(1, "from must be int", r);
	int to=params.as_int(2, "to must be int", r);
	Value& body_code=params.as_junction(3, "body must be code");
	Value* delim_maybe_code=params.count()>4?&params[4]:0;

	if(to-from>=pa_loop_limit) // too long loop?
		throw Exception(PARSER_RUNTIME, 0, "endless loop detected");

	VInt* vint=new VInt(0);

	VMethodFrame& caller=*r.get_method_frame()->caller();
	r.put_element(caller, var_name, vint);
	if(delim_maybe_code){ // delimiter set 
		bool need_delim=false;

		for(int i=from; i<=to; i++) {
			vint->set_int(i);

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
		for(int i=from; i<=to; i++) {
			vint->set_int(i);
 
			r.process_write(body_code);

			if(r.check_skip_break())
				break;
		}
	}
}

static void _eval(Request& r, MethodParams& params) {
	Value& expr=params.as_junction(0, "need expression");
	// evaluate expresion
	Value& value_result=r.process(expr).as_expr_result();
	if(params.count()>1) {
		const String& fmt=params.as_string(1, "fmt must be string").trim();
		if(fmt.is_empty()){
			r.write(value_result);
		} else {
			r.write(String(format(value_result.as_double(), fmt.cstrm())));
		}
	} else
		r.write(value_result);
}

static void _connect(Request& r, MethodParams& params) {
	Value& url=params.as_no_junction(0, "url must not be code");
	Value& body_code=params.as_junction(1, "body must be code");

	Table* protocol2driver_and_client=0;
	if(Value* sql=r.main_class.get_element(String(MAIN_SQL_NAME))) {
		if(Value* element=sql->get_element(String(MAIN_SQL_DRIVERS_NAME))) {
			protocol2driver_and_client=element->get_table();
		}
	}

	// connect
	SQL_Connection* connection=SQL_driver_manager->get_connection(url.as_string(), 
		protocol2driver_and_client,
		r.charsets.source().NAME().cstr(),
		r.request_info.document_root);

	Temp_connection temp_connection(r, connection);

	// execute body
	try {
		r.process_write(body_code);
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
	const String* searching_string;
	double searching_double;
	bool searching_bool;
	Value* found;
	Value* _default;
public:
	Switch_data(Request& ar, Value& asearching): 
		r(ar)
	{
		if(asearching.is_string() || asearching.is_void()){
			searching_string=&asearching.as_string();
			searching_double=0;
			searching_bool=false;
		} else {
			searching_string=0;
			searching_double=asearching.as_double();
			searching_bool=asearching.is_bool();
		}
	}
};
#endif
static void _switch(Request& r, MethodParams& params) {
	Switch_data* data=new Switch_data(r, r.process(params[0]));
	Temp_hash_value<HashString<void*>, void*> switch_data_setter(&r.classes_conf, switch_data_name, data);

	Value& cases_code=params.as_junction(1, "switch cases must be code");
	// execution of found ^case[...]{code} must be in context of ^switch[...]{code}
	// because of stacked WWrapper used there as wcontext
	r.process(cases_code);
	if(Value* selected_code=data->found? data->found: data->_default)
		r.process_write(*selected_code);
}

static void _case(Request& r, MethodParams& params) {
	Switch_data* data=static_cast<Switch_data*>(r.classes_conf.get(switch_data_name));
	if(!data)
		throw Exception(PARSER_RUNTIME,
			0,
			"without switch");

	if(data->found) // matches already was found
		return;

	int count=params.count();
	Value* code=&params.as_expression(--count, "case result must be code");

#ifdef USE_DESTRUCTORS
	Junction *j=code->get_junction();
	if (j){
		code=new VJunction(j->self,j->method,j->method_frame,j->rcontext,j->wcontext,j->code);
		if (j->wcontext) j->wcontext->attach_junction((VJunction *)code);
	}
#endif
	
	for(int i=0; i<count; i++){
		Value& value=r.process(params[i]);

		if(value.is_string() && value.as_string() == CASE_DEFAULT_VALUE){
			data->_default=code;
			continue;
		}

		bool matches;
		if(data->searching_string)
			matches=(*data->searching_string) == value.as_string();
		else if(data->searching_bool || value.is_bool())
			matches=(data->searching_double != 0) == value.as_bool();
		else
			matches=data->searching_double == value.as_double();

		if(matches){
			data->found=code;
			break;
		}
	}
}
#ifndef DOXYGEN
struct Try_catch_result {
	ValueRef processed_code;
	const String* exception_should_be_handled;

	Try_catch_result(): exception_should_be_handled(0) {}
};

/// Auto-object used for temporary changing Request::skip.
class Temp_skip {
	Request& frequest;
	Request::Skip saved_skip;
public:
	Temp_skip(Request& arequest) : frequest(arequest), saved_skip(arequest.get_skip()) {
		arequest.set_skip(Request::SKIP_NOTHING);
	}
	~Temp_skip() {
		if(frequest.get_skip() == Request::SKIP_NOTHING)
			frequest.set_skip(saved_skip);
	}
};
#endif

/// used by ^try and ^cache, @returns $exception.handled[string] if any
template<class I>
static Try_catch_result try_catch(Request& r, Value& body_code(Request&, I), I info, Value* catch_code, bool could_be_handled_by_caller=false) {
	Try_catch_result result;

	// minor bug: context not restored if only finally code is present, see #1062
	if(!catch_code) {
		result.processed_code=body_code(r, info);
		return result;
	}

	// taking snapshot of try-context
	Request_context_saver try_context(r);
	try {
		result.processed_code=body_code(r, info);
	} catch(const Exception& e) {
		Request_context_saver throw_context(r); // remembering exception stack trace

		Request::Exception_details details=r.get_details(e);

		try_context.restore(); // restoring try-context for code after try and catch-code

		{
			Temp_value_element temp(r, *catch_code->get_junction()->method_frame, exception_var_name, &details.vhash);
			Temp_skip temp_skip(r);
			result.processed_code=r.process(*catch_code);
		}
		
		// retriving $exception.handled
		Value* vhandled=details.vhash.hash().get(exception_handled_part_name);

		bool bhandled=false;
		if(vhandled) {
			if(vhandled->is_string()) { // not simple $exception.handled(1/0)?
				if(bhandled=could_be_handled_by_caller) { // and we can possibly handle it
					result.exception_should_be_handled=vhandled->get_string(); // considering 'recovered' and let the caller recover
				}
			} else
				bhandled=vhandled->as_bool();
		}

		if(!bhandled){
			throw_context.restore(); // restoring exception stack trace creared by try_context.restore()
			rethrow;
		}
	}

	return result;
}

static Value& process_try_body_code(Request& r, Value* body_code) {
	return r.process(*body_code);
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



static Value& process_cache_body_code(Request& r, Value* body_code) {
	return r.process(*body_code);
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
				"$" EXCEPTION_VAR_NAME "." EXCEPTION_HANDLED_PART_NAME " value must be "
				"either boolean or string '" CACHE_EXCEPTION_HANDLED_CACHE_NAME "'");
	} else
		info.processed_code=&((Value &)result.processed_code).as_string();

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
		false/*block == don't wait till other thread release lock*/,
		false/*dun throw exception if lock failed*/) ? info.processed_code: 0;

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

	File_read_result file=file_read_binary(file_spec, false /*fail_on_read_problem*/);
	if(file.success && file.length /*ignore reads which are empty due to non-unary open+lockEX conflict with lockSH*/) {
		
		Data_string_serialized_prolog& prolog = *reinterpret_cast<Data_string_serialized_prolog *>(file.str);
		
		String* body=new String;
		if(file.length>=sizeof(Data_string_serialized_prolog) && prolog.version==DATA_STRING_SERIALIZED_VERSION) {
			if(body->deserialize(sizeof(Data_string_serialized_prolog),  file.str, file.length)) {
				result.body=body;
				result.expired=prolog.expires <= now;
			}
		}
	}

	return result;
}

static time_t as_expires(Request& r, MethodParams& params, int index, time_t now) {
	time_t result;
	if(Value* vdate=params[index].as(VDATE_TYPE))
		result=(time_t)(static_cast<VDate*>(vdate)->get_time());
	else
		result=now+(time_t)params.as_double(index, "lifespan must be date or number", r);
	
	return result;
}

static const String& as_file_spec(Request& r, MethodParams& params, int index) {
	return r.full_disk_path(params.as_string(index, "filespec must be string"));
}

static void _cache(Request& r, MethodParams& params) {
	if(params.count()==0) {
		// ^cache[] -- return current expiration time
		Cache_scope* scope=static_cast<Cache_scope*>(r.classes_conf.get(cache_data_name));
		if(!scope)
			throw Exception(PARSER_RUNTIME, 0, "expire-time get without cache");
		r.write(*new VDate((pa_time_t)scope->expires));
		return;
	}

	time_t now=time(0);

	if(params.count()==1) {
		// ^cache[filename] ^cache(seconds) ^cache[expires date]
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
	const String& file_spec=as_file_spec(r, params, 0);

	Cache_scope scope={as_expires(r, params, 1, now), 0};

	Temp_hash_value<HashString<void*>, void*> cache_scope_setter(&r.classes_conf, cache_data_name, &scope);
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
   				r.write(*cached.body);
   				// happy with it
   				return;
   			}
		}

		// no cached info or it's already expired

		// trying to process it under lock and store result in file
		const String* processed_body=locked_process_and_cache_put(r, body_code, catch_code, scope, file_spec);
		if(processed_body){
			// write it out 
			r.write(*processed_body);
			// happy with it
			return;
		} else {
			// we fail while get exclusive lock. nvm, we just execute body_code a bit later
		}
	} else { 
		// instructed not to cache; forget cached copy
		cache_delete(file_spec);
	}
	
	// process without caching
	if(catch_code){
		Try_catch_result result=try_catch(r, process_try_body_code, &body_code, catch_code);
		r.write(result.processed_code);
	} else {
		r.write(r.process_to_string(body_code));
	}
}

static void _try_operator(Request& r, MethodParams& params) {
	Value& body_code=params.as_junction(0, "body_code must be code");
	Value& catch_code=params.as_junction(1, "catch_code must be code");
	Value* finally_code=(params.count()==3) ? &params.as_junction(2, "finally_code must be code") : 0;

	Try_catch_result result;

	try{
		// process try and catch code
		result=try_catch(r, process_try_body_code, &body_code, &catch_code);
	} catch(...){
		// process finally code but ignore the result
		if(finally_code){
			Temp_skip temp(r);
			/* Value &finally_result= */ r.process(*finally_code);
		}
		rethrow;
	}

	// process finally code
	if(finally_code){
		Temp_skip temp(r);
		Value& finally_result=r.process(*finally_code);

		// no exception in try/catch or finally, writing processed body_code or catch_code
		r.write(result.processed_code);

		// write out processed finally code
		r.write(finally_result);
	} else {
		// no exception in try/catch, writing processed body_code or catch_code
		r.write(result.processed_code);
	}

}

static void _throw_operator(Request&, MethodParams& params) {
	if(params.count()==1 && !params[0].is_string()) {
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

			Exception e(type, source, 0);
			e.add_comment(comment); // to avoid MAX_LENGTH limit
			throw e;
		} else
			throw Exception(PARSER_RUNTIME, 0, "one-param version has hash or string param");
	} else {
		const char* type=params.as_string(0, "type must be string").cstr();
		const String* source=params.count()>1? &params.as_string(1, "source must be string"):0;
		const char* comment=params.count()>2? params.as_string(2, "comment must be string").cstr():0;
		Exception e(type, source, 0);
		e.add_comment(comment); // to avoid MAX_LENGTH limit
		throw e;
	}
 }

static void _sleep_operator(Request& r, MethodParams& params) {
	double seconds=params.as_double(0, "seconds must be double", r);
	if(seconds>0)
		pa_sleep((int)trunc(seconds), (int)trunc((seconds-trunc(seconds))*1000000));
 }

#if defined(WIN32) && defined(_DEBUG) && !defined(_WIN64)
#	define PA_BPT
static void _bpt(Request&, MethodParams&) {
	_asm int 3;
}
#endif

// constructor

VClassMAIN::VClassMAIN(): VClass(MAIN_CLASS_NAME) {

#ifdef PA_BPT
	// ^bpt[]
	add_native_method("bpt", Method::CT_ANY, _bpt, 0, 0);
#endif

	// ^if(condition){code-when-true}
	// ^if(condition){code-when-true}{code-when-false}
	// ^if(condition){code-when-true} (another condition){code-when-true} ... {code-when-false}
	add_native_method("if", Method::CT_ANY, _if, 2, 10000, Method::CO_WITHOUT_FRAME);

	// ^untaint[as-is|uri|sql|js|html|html-typo|regex|parser-code]{code}
	add_native_method("untaint", Method::CT_ANY, _untaint, 1, 2, Method::CO_WITHOUT_FRAME);

	// ^taint[as-is|uri|sql|js|html|html-typo|regex|parser-code]{code}
	add_native_method("taint", Method::CT_ANY, _taint, 1, 2, Method::CO_WITHOUT_FRAME);

	// ^apply-taint[untaint lang][string]
	add_native_method("apply-taint", Method::CT_ANY, _apply_taint, 1, 2, Method::CO_WITHOUT_FRAME);

	// ^process{code}
	// ^process[context]{code}[options hash]
	add_native_method("process", Method::CT_ANY, _process, 1, 3);

	// ^rem{code}
	add_native_method("rem", Method::CT_ANY, _rem, 1, 10000, Method::CO_WITHOUT_FRAME);

	// ^while(condition){code}
	add_native_method("while", Method::CT_ANY, _while, 2, 3, Method::CO_WITHOUT_FRAME);

	// ^use[file[;options hash]]
	add_native_method("use", Method::CT_ANY, _use, 1, 2);

	// ^break[]
	// ^break(condition)
	add_native_method("break", Method::CT_ANY, _break, 0, 1, Method::CO_WITHOUT_FRAME);

	// ^continue[]
	// ^continue(condition)
	add_native_method("continue", Method::CT_ANY, _continue, 0, 1, Method::CO_WITHOUT_FRAME);

	// ^return[]
	// ^return[result]
	add_native_method("return", Method::CT_ANY, _return, 0, 1);

	// ^for[i](from-number;to-number-inclusive){code}[delim]
	add_native_method("for", Method::CT_ANY, _for, 3+1, 3+1+1);

	// ^eval(expr)
	// ^eval(expr)[format]
	add_native_method("eval", Method::CT_ANY, _eval, 1, 2, Method::CO_WITHOUT_FRAME);

	// ^connect[protocol://user:pass@host[:port]/database]{code with ^sql-s}
	add_native_method("connect", Method::CT_ANY, _connect, 2, 2);


	// ^cache[file_spec](time){code}[{catch code}] time=0 no cache
	// ^cache[file_spec] delete cache
	// ^cache[] get current expiration time
	add_native_method("cache", Method::CT_ANY, _cache, 0, 4);
	
	// switch

	// ^switch[value]{cases}
	add_native_method("switch", Method::CT_ANY, _switch, 2, 2, Method::CO_WITHOUT_FRAME);

	// ^case[value]{code}
	add_native_method("case", Method::CT_ANY, _case, 2, 10000, Method::CO_WITHOUT_FRAME);

	// try-catch

	// ^try{code}{catch code}
	add_native_method("try", Method::CT_ANY, _try_operator, 2, 3, Method::CO_WITHOUT_FRAME);
	// ^throw[$exception hash]
	// ^throw[type;source;comment]
	add_native_method("throw", Method::CT_ANY, _throw_operator, 1, 3);

	add_native_method("sleep", Method::CT_ANY, _sleep_operator, 1, 1);
}

// constructor & configurator

VStateless_class& VClassMAIN_create() {
	return *new VClassMAIN;
}
