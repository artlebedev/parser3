/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_os.h"
#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_vclass.h"
#include "pa_globals.h"
#include "pa_vint.h"
#include "pa_vmethod_frame.h"
#include "pa_types.h"
#include "pa_venv.h"
#include "pa_vmath.h"
#include "pa_vstatus.h"
#include "pa_vrequest.h"
#include "pa_vtable.h"
#include "pa_vfile.h"
#include "pa_dictionary.h"
#include "pa_charset.h"
#include "pa_charsets.h"
#include "pa_cache_managers.h"
#include "pa_http.h"
#include "pa_vmail.h"
#include "pa_vform.h"
#include "pa_vcookie.h"
#include "pa_vresponse.h"
#include "pa_vmemory.h"
#include "pa_vconsole.h"
#include "pa_vdate.h"

volatile const char * IDENT_PA_REQUEST_C="$Id: pa_request.C,v 1.415 2021/01/02 23:01:11 moko Exp $" IDENT_PA_REQUEST_H IDENT_PA_REQUEST_CHARSETS_H IDENT_PA_REQUEST_INFO_H IDENT_PA_VCONSOLE_H;

// consts

#define UNHANDLED_EXCEPTION_METHOD_NAME "unhandled_exception"

/// content type of response when no $MAIN:defaults.content-type defined
const char* DEFAULT_CONTENT_TYPE="text/html";

const uint LOOP_LIMIT=20000;
const uint EXECUTE_RECOURSION_LIMIT=1000;
const uint HTTPD_TIMEOUT=4;
const size_t FILE_SIZE_LIMIT=512*1024*1024;

// defines for globals

#define MAIN_METHOD_NAME "main"
#define AUTO_METHOD_NAME "auto"
#define USE_METHOD_NAME "use"
#define AUTOUSE_METHOD_NAME "autouse"

#define EXCEPTION_TYPE_PART_NAME "type"
#define EXCEPTION_SOURCE_PART_NAME "source"
#define EXCEPTION_COMMENT_PART_NAME "comment"

#define ORIGIN_KEY "origin"

// globals

const String main_method_name(MAIN_METHOD_NAME);
const String auto_method_name(AUTO_METHOD_NAME);
static const String use_method_name(USE_METHOD_NAME);
static const String autouse_method_name(AUTOUSE_METHOD_NAME);

const String exception_type_part_name(EXCEPTION_TYPE_PART_NAME);
const String exception_source_part_name(EXCEPTION_SOURCE_PART_NAME);
const String exception_comment_part_name(EXCEPTION_COMMENT_PART_NAME);
const String exception_handled_part_name(EXCEPTION_HANDLED_PART_NAME);

static const String origin_key(ORIGIN_KEY);

int pa_loop_limit=LOOP_LIMIT;
int pa_execute_recoursion_limit=EXECUTE_RECOURSION_LIMIT;
int pa_httpd_timeout=HTTPD_TIMEOUT;
size_t pa_file_size_limit=FILE_SIZE_LIMIT;

// defines for statics

#define MIME_TYPES_NAME "MIME-TYPES"
#define CLASS_PATH_NAME "CLASS_PATH"

#define DOWNLOAD_NAME_UPPER "DOWNLOAD"
#define BODY_NAME_UPPER "BODY"

// statics

static const String main_class_name(MAIN_CLASS_NAME);
static const String mime_types_name(MIME_TYPES_NAME);
static const String class_path_name(CLASS_PATH_NAME);

static const String charsets_name("CHARSETS");
static const String strict_vars_name("STRICT-VARS");
static const String prototype_name("OBJECT-PROTOTYPE");
static const String getter_protected_name("CLASS-GETTER-PROTECTED");
static const String locals_name("LOCALS");
static const String limits_name("LIMITS");
static const String loop_limit_name("max_loop");
static const String recoursion_limit_name("max_recoursion");
static const String file_size_limit_name("max_file_size");
static const String lock_wait_timeout_name("lock_wait_timeout");
static const String httpd_name("HTTPD");
static const String httpd_timeout_name("timeout");
static const String httpd_mode_name("mode");

static const String conf_method_name("conf");
static const String post_process_method_name("postprocess");
static const String response_body_file_name("file");

static const String download_name_upper(DOWNLOAD_NAME_UPPER);
static const String body_name_upper(BODY_NAME_UPPER);

// more static

static const String content_type_name_upper(HTTP_CONTENT_TYPE_UPPER);
static const String content_disposition_name_upper(CONTENT_DISPOSITION_UPPER);
static const String content_disposition_inline(CONTENT_DISPOSITION_INLINE);
static const String content_disposition_attachment(CONTENT_DISPOSITION_ATTACHMENT);

// defines

#define CHARSET_NAME_UPPER "CHARSET"
#define LAST_MODIFIED_NAME_UPPER "LAST-MODIFIED"

// op.C
VStateless_class& VClassMAIN_create();

//
Request::Request(SAPI_Info& asapi_info, Request_info& arequest_info, String::Language adefault_lang):
	// private
	anti_endless_execute_recoursion(0),

	// public
	allow_class_replace(false),
	method_frame(0),
	rcontext(0),
	wcontext(0),
	flang(adefault_lang),
	fconnection(0),
	fin_cycle(0),
	fskip(SKIP_NOTHING),

	// public
	request_info(arequest_info),
	sapi_info(asapi_info),
	charsets(pa_UTF8_charset, pa_UTF8_charset, pa_UTF8_charset), // default charsets

	main_class(VClassMAIN_create()),
	form(*new VForm(charsets, arequest_info)),
	mail(*new VMail),
	response(*new VResponse(arequest_info, charsets)),
	cookie(*new VCookie(charsets, arequest_info)),
	console(*new VConsole),

	// private
	configure_admin_done(false),

	// private defaults
	fdefault_lang(adefault_lang), 
	// private mime types
	mime_types(0)
{
	pa_register_thread_request(*this);

	// file_no=0 => unknown
	file_list+="UNKNOWN";
	file_list+="-body of process-"; // pseudo_file_no__process

	// maybe expire old caches
	cache_managers->maybe_expire();
	
	/// directly used
	// MAIN class, operators
	put_class(&main_class);
	// classes:
	// table, file, random, mail, image, ...
	methoded_array().register_directly_used(*this);

	/// methodless

	// env class
	put_class(new VEnv(asapi_info));
	// status class
	put_class(new VStatus());
	// request class
	put_class(new VRequest(arequest_info, charsets, form, asapi_info));
	// cookie class
	put_class(&cookie);
	// console class
	put_class(&console);

	/// methoded

	// response class
	put_class(&response);
	// form class
	put_class(&form);
	// mail class
	put_class(&mail);
	// math class
	put_class(new VMath);
	// memory class
	put_class(new VMemory);
}

Request::~Request() {
#ifdef XML
	// if for some strange reason xml generic errors failed to be reported, free them up
	if(const char* xml_generic_errors=xmlGenericErrors()) {
		SAPI::log(sapi_info, "warning: unreported xmlGenericErrors: %s", xml_generic_errors);
		pa_free((void *)xml_generic_errors);
	}
#endif
}

Value& Request::get_self() { return method_frame/*always have!*/->self(); }

VStateless_class* Request::get_class(const String& name){
	VStateless_class* result=classes().get(name);
	if(!result)
		if(const Method *method=main_class.get_element_method(autouse_method_name)){
			Value *vname=new VString(name);
			CONSTRUCTOR_FRAME_ACTION(*method, 0 /*no parent*/, main_class, {
				frame.store_params(&vname, 1);
				// we don't need the result
				call(frame);
			});
			result=classes().get(name);
		}
	return result;
}

VStateless_class& Request::get_class_ref(const String& name){
	VStateless_class* result=get_class(name);
	if(!result)
		throw Exception(PARSER_RUNTIME, &name, "class is undefined");
	return *result;
}

static void load_charset(HashStringValue::key_type akey, HashStringValue::value_type avalue, Request_charsets* charsets) {
	pa_charsets.load_charset(*charsets, akey, avalue->as_string());
}


#define CONF_OPTION(config, name, code, exception_name)		\
	if(config)						\
		if(Value* option=config->get_element(name)) {	\
			if(option->is_evaluated_expr()) {	\
				code;				\
			} else					\
				throw Exception(PARSER_RUNTIME, 0, "$MAIN:" exception_name, name.cstr()); \
		}

void Request::configure_admin(VStateless_class& conf_class) {
	if(configure_admin_done)
		throw Exception(PARSER_RUNTIME, 0, "parser already configured");
	configure_admin_done=true;
	
	// charsets must only be specified in method_frame config
	// so that users would not interfere

	/* $MAIN:CHARSETS[
			$.charsetname1[/full/path/to/charset/file.cfg]
			...
		]
	*/
	if(Value* vcharsets=conf_class.get_element(charsets_name)) {
		if(!vcharsets->is_string()) {
			if(HashStringValue* charsets=vcharsets->get_hash())
				charsets->for_each<Request_charsets*>(load_charset, &this->charsets);
			else
				throw Exception(PARSER_RUNTIME, 0, "$MAIN:%s must be hash", charsets_name.cstr());
		}
	}

#ifdef STRICT_VARS
	VVoid::strict_vars=false;
	if(Value* strict_vars=conf_class.get_element(strict_vars_name)) {
		if(strict_vars->is_bool())
			VVoid::strict_vars=strict_vars->as_bool();
		else
			throw Exception(PARSER_RUNTIME, 0, "$MAIN:%s must be bool", strict_vars_name.cstr());
	}
#endif

#ifdef OBJECT_PROTOTYPE
	VClass::prototype=true;
	if(Value* prototype=conf_class.get_element(prototype_name)) {
		if(prototype->is_bool())
			VClass::prototype=prototype->as_bool();
		else
			throw Exception(PARSER_RUNTIME, 0, "$MAIN:%s must be bool", prototype_name.cstr());
	}
#endif

#ifdef CLASS_GETTER_UNPROTECTED
	VClass::getter_protected=true;
	if(Value* getter_protected=conf_class.get_element(getter_protected_name)) {
		if(getter_protected->is_bool())
			VClass::getter_protected=getter_protected->as_bool();
		else
			throw Exception(PARSER_RUNTIME, 0, "$MAIN:%s must be bool", getter_protected_name.cstr());
	}
#endif

	VStateless_class::gall_vars_local=false;
	if(Value* locals=conf_class.get_element(locals_name)) {
		if(locals->is_bool()){
			VStateless_class::gall_vars_local=locals->as_bool();
			main_class.set_all_vars_local();
		} else
			throw Exception(PARSER_RUNTIME, 0, "$MAIN:%s must be bool", locals_name.cstr());
	}

	Value* limits=conf_class.get_element(limits_name);

	pa_loop_limit=LOOP_LIMIT;
	CONF_OPTION(limits, loop_limit_name, {
		pa_loop_limit=option->as_int();
		if(pa_loop_limit==0) pa_loop_limit=INT_MAX;
	}, "LIMITS.%s must be int");

	pa_execute_recoursion_limit=EXECUTE_RECOURSION_LIMIT;
	CONF_OPTION(limits, recoursion_limit_name, {
		pa_execute_recoursion_limit=option->as_int();
		if(pa_execute_recoursion_limit==0) pa_execute_recoursion_limit=INT_MAX;
	}, "LIMITS.%s must be int");

	pa_file_size_limit=FILE_SIZE_LIMIT;
	CONF_OPTION(limits, file_size_limit_name, {
		double limit=option->as_double();
		if(limit >= (double)SSIZE_MAX)
			throw Exception(PARSER_RUNTIME, 0, "$MAIN:LIMITS.%s must be less then %.15g", file_size_limit_name.cstr(), (double)SSIZE_MAX);
		pa_file_size_limit=(size_t)limit;
		if(pa_file_size_limit==0)
			pa_file_size_limit=SSIZE_MAX;
	}, "LIMITS.%s must be number");

	pa_lock_attempts=PA_LOCK_ATTEMPTS;
	CONF_OPTION(limits, lock_wait_timeout_name, {
		double limit=option->as_double();
		if(limit >= 3600*24)
			throw Exception(PARSER_RUNTIME, 0, "$MAIN:LIMITS.%s must be less then %d", lock_wait_timeout_name.cstr(), 3600*24);
		pa_lock_attempts=(unsigned int)(limit*2)+1;
	}, "LIMITS.%s must be number");

	Value* httpd=conf_class.get_element(httpd_name);

	pa_httpd_timeout=HTTPD_TIMEOUT;
	CONF_OPTION(httpd, httpd_timeout_name, {
		pa_httpd_timeout=option->as_int();
		if(pa_httpd_timeout==0) pa_httpd_timeout=INT_MAX;
	}, "HTTPD.%s must be int");

	if(httpd)
		if(Value* option=httpd->get_element( httpd_mode_name)) {
			if(option->get_junction())
				throw Exception(PARSER_RUNTIME, 0, "$MAIN:HTTPD:mode must be string");
			HTTPD_Server::set_mode(option->as_string());
		}

	// configure method_frame options
	//	until someone with less privileges have overriden them
	methoded_array().configure_admin(*this);
}

const char* Request::get_exception_cstr(const Exception& e, Request::Exception_details& details) {

#define PA_URI_FORMAT "%s: "
#define PA_COMMENT_TYPE_FORMAT "%s [%s]"

#define PA_ORIGIN_FILE_POS_FORMAT "%s(%d:%d): "
#define PA_SOURCE_FORMAT "'%s' "

#define PA_ORIGIN_FILE_POS_VALUE file_list[details.origin.file_no].cstr(), 1+details.origin.line, 1+details.origin.col,
#define PA_SOURCE_VALUE details.problem_source->cstr(),

#define EXCEPTION_CSTR(f1,v1,f2,v2) \
			snprintf(result, MAX_STRING, \
				PA_URI_FORMAT \
				f1 f2 \
				PA_COMMENT_TYPE_FORMAT, \
				request_info.uri, \
				v1 v2 \
				e.comment(), e.type() \
			);

	char* result=new(PointerFreeGC) char[MAX_STRING];

	if(details.problem_source) { // do we know the guy?
		if(details.origin.file_no) // do whe know where he came from?
			EXCEPTION_CSTR(PA_ORIGIN_FILE_POS_FORMAT, PA_ORIGIN_FILE_POS_VALUE, PA_SOURCE_FORMAT, PA_SOURCE_VALUE)
		else
			EXCEPTION_CSTR(PA_SOURCE_FORMAT, PA_SOURCE_VALUE,,)
	} else {
		if(details.origin.file_no) // do whe know where he came from?
			EXCEPTION_CSTR(PA_ORIGIN_FILE_POS_FORMAT, PA_ORIGIN_FILE_POS_VALUE,,)
		else
			EXCEPTION_CSTR(,,,)
	}

	return result;
}

Table &Request::Exception_trace::table(Request &r){
	// $stack[^table::create{name	file	lineno	colno}]
	Table::columns_type stack_trace_columns(new ArrayString);
	*stack_trace_columns+=new String("name");
	*stack_trace_columns+=new String("file");
	*stack_trace_columns+=new String("lineno");
	*stack_trace_columns+=new String("colno");
	Table& stack_trace=*new Table(stack_trace_columns);

	if(!is_empty()/*signed!*/)
		for(size_t i=bottom_index(); i<top_index(); i++) {
			Trace trace=get(i);
			Table::element_type row(new ArrayString);

			*row+=trace.name(); // name column
			Operation::Origin origin=trace.origin();
			if(origin.file_no) {
				*row+=new String(r.file_list[origin.file_no], String::L_TAINTED); // 'file' column
				*row+=new String(String::Body::Format(1+origin.line), String::L_CLEAN); // 'lineno' column
				*row+=new String(String::Body::Format(1+origin.col), String::L_CLEAN); // 'colno' column
			}
			stack_trace+=row;
		}

	return stack_trace;
}

void Request::configure() {
	// configure admin options if not configured yet
	if(!configure_admin_done)
		configure_admin(main_class);

	// configure not-admin=user options
	methoded_array().configure_user(*this);

	// $MAIN:MIME-TYPES
	if(Value* element=main_class.get_element(mime_types_name))
		if(Table *table=element->get_table())
			mime_types=table;
}

/**
	load MAIN class, execute @main.
	MAIN class consists of all the auto.p files we'd manage to find
	plus
	the file user requested us to process
	all located classes become children of one another,
	composing class we name 'MAIN'

	@test log stack trace

*/
void Request::core(const char* config_filespec, bool header_only, const String &amain_method_name, const String* amain_class_name) {
	VFile* body_file=NULL;
	bool as_attachment=false;

	try {
		// loading config
		if(config_filespec)
			use_file_directly(*new String(config_filespec));

		// filling mail received
		mail.fill_received(*this);

		try {
			// compile requested file
			if(request_info.path_translated)
				use_file_directly(*new String(request_info.path_translated, String::L_TAINTED), true, true /* load auto.p files */);
			configure();
		} catch(...) {
			configure(); // configure anyway, useful in @unhandled_exception [say, if they would want to mail by SMTP something]
			rethrow;
		}

		VStateless_class& main = amain_class_name ? get_class_ref(*amain_class_name) : main_class;
		// execute @main[]
		const String* body_string=amain_method_name.is_empty() ? &String::Empty : execute_method(main, amain_method_name);
		if(!body_string)
			throw Exception(PARSER_RUNTIME, &amain_method_name, "method not found in class %s", main.type());

		// extract response body
		Value* body_value=response.fields().get(download_name_upper); // $response:download?
		as_attachment=body_value!=0;
		if(!body_value)
			body_value=response.fields().get(body_name_upper); // $response:body
		if(!body_value)
			body_value=new VString(*body_string); // just result of ^main[]

		// @postprocess
		if(const Method *method=main_class.get_method(post_process_method_name)) {
			METHOD_FRAME_ACTION(*method, 0 /*no parent*/, main_class, {
				frame.store_params(&body_value, 1);
				call(frame);
				body_value=&frame.result();
			});
		}

		body_file=body_value->as_vfile(flang, &charsets);

	} catch(const Exception& e) { // request handling problem
		try {
			// we're returning not result, but error explanation

			Request::Exception_details details=get_details(e);
			const char* exception_cstr=get_exception_cstr(e, details);

			// reset language to default
			flang=fdefault_lang;
			// reset response
			response.fields().clear();

			// this is what we'd return in $response:body
			const String* body_string=0;

			// maybe we'd be lucky enough as to report an error in a gracefull way...
			if(const Method *method=main_class.get_method(*new String(UNHANDLED_EXCEPTION_METHOD_NAME))) {
				// preparing parameters to @unhandled_exception[exception;stack]

				Table& stack_trace=exception_trace.table(*this);
				exception_trace.clear(); // forget all about previous life, in case there would be error inside of this method, error handled would not be mislead by old stack contents (see extract_origin)

				Value *params[]={&details.vhash, new VTable(&stack_trace)};
				METHOD_FRAME_ACTION(*method, 0 /*no caller*/, main_class, {
					frame.store_params(params, 2);
					call(frame);
					body_string=&frame.result().as_string();
				});
			}

			// conditionally log it
			Value* vhandled=details.vhash.hash().get(exception_handled_part_name);
			if(!vhandled || !vhandled->as_bool()) {
				SAPI::log(sapi_info, "%s", exception_cstr);
			}

			if(body_string) {  // could report an error beautifully?
				VString body_vstring(*body_string);

				body_file=body_vstring.as_vfile(flang, &charsets);
				as_attachment=false;
			} else {
				// doing that ugly
				SAPI::send_error(sapi_info, exception_cstr, !strcmp(e.type(), "file.missing") ? "404" : "500");
				return;
			}

		} catch(const Exception& e) { // exception in unhandled exception
			Request::Exception_details details=get_details(e);
			// unconditionally log the beast in exception handler
			throw Exception(0, 0, "Unhandled exception in %s", get_exception_cstr(e, details));
		}
	}

	// write out the result outside of try as network exceptions should not be handled by parser code.
	output_result(body_file, header_only, as_attachment);
}

uint Request::register_file(String::Body file_spec) {
	file_list+=file_spec;
	return file_list.count()-1;
}

void Request::use_file_directly(const String& file_spec, bool fail_on_file_absence, bool with_auto_p) {
	// cyclic dependence check
	if(used_files.get(file_spec))
		return;
	used_files.put(file_spec, true);

	if(!fail_on_file_absence && !entry_exists(file_spec)) // ignore file absence if asked for
		return;

	if(with_auto_p) {
		// loading auto.p files from document_root/.. 
		// to the one beside requested file.
		// all assigned bases from upper dir
		const char* target=file_spec.cstr();

		// all relative paths are calculated from main document
		request_info.path_translated=target;

		const char* after=target;
		size_t drlen=strlen(request_info.document_root);
		if(memcmp(after, request_info.document_root, drlen)==0) {
			after+=drlen;
			if(after[-1]=='/') 
				--after;
		}

		while(const char* before=strchr(after, '/')) {
			String& sfile_spec=*new String;
			if(after!=target) {
				sfile_spec.append_strdup(target, before-target, String::L_CLEAN);
				sfile_spec << "/" AUTO_FILE_NAME;

				use_file_directly(sfile_spec, false /*ignore absence, sole user*/);
			}
			for(after=before+1;*after=='/';after++);
		}
	}

	if(const char* source=file_read_text(charsets, file_spec, true))
		use_buf(main_class, source, 0, register_file(file_spec));
}


void Request::use_file(const String& file_name, const String* use_filespec/*absolute*/, bool with_auto_p) {
	if(file_name.is_empty())
		throw Exception(PARSER_RUNTIME, 0, "usage failed - no filename was specified");

	const String* filespec=0;

	if(file_name.first_char()=='/') //absolute path? [no need to scan MAIN:CLASS_PATH]
		filespec=&full_disk_path(file_name);
	else if(use_filespec){ // search in current dir first
		size_t last_slash_pos=use_filespec->strrpbrk("/");
		if(last_slash_pos!=STRING_NOT_FOUND)
			filespec=file_exist(use_filespec->mid(0, last_slash_pos), file_name); // found in current dir?
	}

	if(!filespec){
		// prevent multiple scan CLASS_PATH for searching one file
		if(searched_along_class_path.get(file_name))
			return;
		searched_along_class_path.put(file_name, true);
		if(Value* element=main_class.get_element(class_path_name)) {
			if(element->is_string()) {
				filespec=file_exist(full_disk_path(element->as_string()), file_name); // found at class_path?
			} else if(Table *table=element->get_table()) {
				for(size_t i=table->count(); i--; ) {
					const String& path=*(*table->get(i))[0];
					if(filespec=file_exist(full_disk_path(path), file_name))
						break; // found along class_path
				}
			} else
				throw Exception(PARSER_RUNTIME, 0, "$" CLASS_PATH_NAME " must be string or table");
			if(!filespec)
				throw Exception(PARSER_RUNTIME, &file_name, "not found along $MAIN:" CLASS_PATH_NAME);
		} else 
			throw Exception(PARSER_RUNTIME, &file_name, "usage failed - no $MAIN:" CLASS_PATH_NAME " were specified");
	}

	use_file_directly(*filespec, true, with_auto_p);
}

void Request::use_file(const String& file_name, const String* use_filespec/*absolute*/, Operation::Origin origin) {
	static String use("USE");
	try {
		static VHash* voptions=new VHash();
		if(const Method *method=main_class.get_method(use_method_name)){
			Value *params[]={new VString(file_name), voptions};
			voptions->hash().put(origin_key, new VString(*use_filespec));

			CONSTRUCTOR_FRAME_ACTION(*method, 0 /*no parent*/, main_class, {
				frame.store_params(params, 2);
				// we don't need the result
				call(frame);
			});
		}
	} catch (...) {
		exception_trace.push(Trace(&use, origin));
		rethrow;
	}
}

void Request::use_buf(VStateless_class& aclass, const char* source, const String* main_alias, uint file_no, int line_no_offset) {
	// temporary zero @conf to avoid it second execution
	Temp_method temp_method_conf(aclass, conf_method_name, 0);
	// temporary zero @auto to avoid it second execution
	Temp_method temp_method_auto(aclass, auto_method_name, 0);

	// compile loaded classes
	ArrayClass& cclasses=compile(&aclass, source, main_alias, file_no, line_no_offset);

	VString* vfilespec=
		new VString(*new String(file_list[file_no], String::L_TAINTED));

	for(size_t i=0; i<cclasses.count(); i++){
		VStateless_class& cclass=*cclasses.get(i);

		// locate and execute possible @conf[] static
		if(execute_method_if_exists(cclass, conf_method_name, vfilespec))
			configure_admin(cclass/*, executed.method->name*/);

		// locate and execute possible @auto[] static
		execute_method_if_exists(cclass, auto_method_name, vfilespec);

		cclass.enable_default_setter();
	}
}

const String& Request::relative(const char* apath, const String& relative_name) {
	char *hpath=pa_strdup(apath);
	String& result=*new String;
	if(rsplit(hpath, '/')) // if something/splitted
		result << hpath << "/";
	result << relative_name;
	return result;
}

const String& Request::full_disk_path(const String& relative_name) {
	if(relative_name.first_char()=='/') {
		String& result=*new String(pa_strdup(request_info.document_root));
		result << relative_name;
		return result;
	}
	if(relative_name.pos("://")!=STRING_NOT_FOUND // something like "http://xxx"
#ifdef WIN32
		|| relative_name.pos(":")==1  // DRIVE:
		|| relative_name.starts_with("\\\\") // UNC1
		|| relative_name.starts_with("//") // UNC2
#endif
		)
		return relative_name;

	return relative(request_info.path_translated ? request_info.path_translated : request_info.document_root, relative_name);
}

#ifndef DOXYGEN
class Add_header_attribute_info
{
public:
	Add_header_attribute_info(Request& ar) : r(ar), add_last_modified(true) {}
	Request& r;
	bool add_last_modified;
};
#endif
static void add_header_attribute(HashStringValue::key_type name, HashStringValue::value_type value, Add_header_attribute_info* info) {
	if(name==BODY_NAME_UPPER || name==DOWNLOAD_NAME_UPPER || name==CHARSET_NAME_UPPER)
		return;
	
	if(name==LAST_MODIFIED_NAME_UPPER)
		info->add_last_modified=false;

	const char* aname=String(name, String::L_URI).untaint_and_transcode_cstr(String::L_URI, &info->r.charsets);

	SAPI::add_header_attribute(info->r.sapi_info,
			aname,
			attributed_meaning_to_string(*value, String::L_URI, false).untaint_and_transcode_cstr(String::L_URI, &info->r.charsets)
		);
}

static void output_sole_piece(Request& r, bool header_only, VFile& body_file, Value* body_file_content_type) {
	// transcode text body when "text/*" or simple result
	String::C output(body_file.value_ptr(), body_file.value_size());
	if(!body_file_content_type/*vstring.as_vfile*/ || body_file_content_type->as_string().pos("text/")==0)
		output=Charset::transcode(output, r.charsets.source(), r.charsets.client());

	// prepare header: Content-Length
	SAPI::add_header_attribute(r.sapi_info, HTTP_CONTENT_LENGTH, format(output.length, "%u"));

	// send header
	SAPI::send_header(r.sapi_info);
	
	// send body
	if(!header_only)
		SAPI::send_body(r.sapi_info, output.str, output.length);
}

#ifndef DOXYGEN
struct Range
{
	uint64_t start;
	uint64_t end;
};
#endif

#define UNSET ((uint64_t)-1)

static void parse_range(const String* s, Array<Range> &ar) {
	const char *p = s->cstr();
	if(s->starts_with("bytes="))
		p += 6;
	Range r;
	while(*p){
		r.start = UNSET;
		r.end = UNSET;

		while(*p==' ' || *p=='\t') p++;

		if(*p >= '0' && *p <= '9'){
			const char *s=p;
			while(*p>='0' && *p<='9') p++;
			r.start = pa_atoul(pa_strdup(s,p-s));
		}

		while(*p==' ' || *p=='\t') p++;

		if(*p++ != '-') break;

		while(*p==' ' || *p=='\t') p++;

		if(*p >= '0' && *p <= '9'){
			const char *s=p;
			while(*p>='0' && *p<='9') p++;
			r.end = pa_atoul(pa_strdup(s,p-s));
		}

		while(*p==' ' || *p=='\t') p++;

		if(*p)
			if(*p++ != ',') break;

		ar += r;
	}
}

struct Send_range_action_info {
	Request *r;
	uint64_t offset;
	uint64_t part_length;
};

static void send_range(struct stat& /*finfo*/, int f, const String& /*file_spec*/, void *context){
	Send_range_action_info &info = *(Send_range_action_info*)context;

	SAPI::send_header(info.r->sapi_info);
	pa_lseek(f, info.offset, SEEK_SET);

	const size_t BUFSIZE = 128*0x400;
	char buf[BUFSIZE];
	do{
		size_t to_read = info.part_length < BUFSIZE ? (size_t)info.part_length : BUFSIZE;
		size_t to_write = file_block_read(f, buf, to_read);

		if(to_write == 0)
			break;

		size_t size = SAPI::send_body(info.r->sapi_info, buf, to_write);
		if(size != to_write)
			break;

		info.part_length -= to_write;
	} while (info.part_length);
}

static void output_pieces(Request& r, bool header_only, const String& filename, uint64_t content_length, Value& date, bool add_last_modified) {
	SAPI::add_header_attribute(r.sapi_info, "accept-ranges", "bytes");

	const char *range = SAPI::Env::get(r.sapi_info, "HTTP_RANGE");
	uint64_t offset=0;
	uint64_t part_length=content_length;

	if(range && content_length){
		Array<Range> ar;
		parse_range(new String(range), ar);
		int count = ar.count();
		if(count == 1){
			Range &rg = ar.get_ref(0);

			if(rg.start == UNSET && rg.end == UNSET)
				return SAPI::send_error(r.sapi_info, "", "416");

			if(rg.start == UNSET && rg.end != UNSET){
				if(rg.end > content_length)
					rg.end = content_length;
				rg.start = content_length - rg.end;
				rg.end =  content_length-1;
			} else if(rg.start != UNSET && rg.end == UNSET){
				if(rg.start >= content_length)
					return SAPI::send_error(r.sapi_info, "", "416");
				rg.end = content_length-1;
			} else {
				if(rg.start >= content_length || rg.start > rg.end)
					return SAPI::send_error(r.sapi_info, "", "416");
				if(rg.end >= content_length)
					rg.end = content_length-1;
			}

			offset = rg.start;
			part_length = rg.end-rg.start+1;

			char buf[MAX_STRING];
			snprintf(buf, MAX_STRING, "bytes %.15g-%.15g/%.15g", (double)rg.start, (double)rg.end, (double)content_length);
			SAPI::add_header_attribute(r.sapi_info, HTTP_STATUS, "206");
			SAPI::add_header_attribute(r.sapi_info, "content-range", buf);
		} else {
			return SAPI::send_error(r.sapi_info, count ? "Multiple ranges are not supported" : "Invalid range", count ? "501" : "400");
		}
	}

	SAPI::add_header_attribute(r.sapi_info, HTTP_CONTENT_LENGTH, format((double)part_length, "%.15g"));

	if(add_last_modified)
		SAPI::add_header_attribute(r.sapi_info, "last-modified", attributed_meaning_to_string(date, String::L_AS_IS, true).cstr());

	if(header_only){
		SAPI::send_header(r.sapi_info);
	} else {
		Send_range_action_info info = { &r, offset, part_length};
		file_read_action_under_lock(r.full_disk_path(filename), "send", send_range, &info);
	}
}

void Request::output_result(VFile* body_file, bool header_only, bool as_attachment) {
	// header: cookies
	cookie.output_result(sapi_info);
	
	// $.file and $.name (when body is real vfile)
	Value* vfile = body_file->fields().get(response_body_file_name);
	Value* vname = body_file->fields().get(name_name);

	const String* sfile = vfile ? &vfile->as_string() : NULL;
	const String* sname = vname ? &vname->as_string() : NULL;

	if(sname && *sname == NONAME_DAT)
		sname = NULL;

	// Content-Disposition, use $.name[<empty>] to avoid
	const String* disposition_name = sname ? sname->is_empty() ? NULL : sname : sfile;
	if(disposition_name) {
		VHash& hash=*new VHash();
		hash.hash().put(value_name, new VString(as_attachment ? content_disposition_attachment : content_disposition_inline));
		hash.hash().put(content_disposition_filename_name, new VString(*new String(*disposition_name, String::L_HTTP_HEADER)));
		response.fields().put(content_disposition_name_upper, &hash);
	}

	// _file_ content-type might be specified
	Value* body_file_content_type = body_file->fields().get(content_type_name);
	if(!body_file_content_type){
		const String* scontent_name = sname && !sname->is_empty() ? sname : sfile;
		if(scontent_name)
			body_file_content_type = new VString(mime_type_of(scontent_name->cstr()));
	}

	// set Content-Type
	if(body_file_content_type) {
		// body file content type
		response.fields().put(content_type_name_upper, body_file_content_type);
	} else {
		// default content type
		response.fields().put_dont_replace(content_type_name_upper, new VString(*new String(DEFAULT_CONTENT_TYPE)));
	}

	// prepare header: $response:fields without :body, :download and :charset
	Add_header_attribute_info info(*this);
	response.fields().for_each<Add_header_attribute_info*>(add_header_attribute, &info);

	if(sfile) {
		// $response:[download|body][$.file[filespec]] -- optput specified file
		uint64_t content_length=0;
		time_t atime=0, mtime=0, ctime=0;
		file_stat(full_disk_path(*sfile), content_length, atime, mtime, ctime);

		VDate* vdate=0;
		if(Value* v=body_file->fields().get("mdate")) {
			if(Value* vdatep=v->as(VDATE_TYPE))
				vdate=static_cast<VDate*>(vdatep);
			else 
				throw Exception(PARSER_RUNTIME, 0, "mdate must be a date");
		}
		if(!vdate)
			vdate=new VDate((pa_time_t)mtime);

		output_pieces(*this, header_only, *sfile, content_length, *vdate, info.add_last_modified);
	} else {
		if(body_file_content_type)
			if(HashStringValue *hash=body_file_content_type->get_hash())
				body_file_content_type=hash->get(value_name);

		output_sole_piece(*this, header_only, *body_file, body_file_content_type);
	}
}

const String& Request::mime_type_of(const String* file_name) {
	return mime_type_of(file_name?file_name->taint_cstr(String::L_FILE_SPEC):0);
}

const String& Request::mime_type_of(const char* user_file_name_cstr) {
	if(mime_types)
		if(const char* cext=strrchr(user_file_name_cstr, '.')) {
			String sext(++cext);
			Table::Action_options options;
			if(mime_types->locate(0, sext.change_case(charsets.source(), String::CC_LOWER), options)) {
				if(const String* result=mime_types->item(1))
					return *result;
				else
					throw Exception(PARSER_RUNTIME, 0, MIME_TYPES_NAME " table column elements must not be empty");
			}
		}

	return *new String("application/octet-stream");
}

const String* Request::get_used_filespec(uint file_no){
	if(file_no < file_list.count())
		return new String(file_list[file_no], String::L_TAINTED);
	return 0;
}

#ifdef XML
xmlChar* Request::transcode(const String& s) {
	return charsets.source().transcode(s);
}

xmlChar* Request::transcode(const String::Body s) {
	return charsets.source().transcode(s);
}

const String& Request::transcode(const xmlChar* s) {
	return charsets.source().transcode(s);
}
#endif

Request::Exception_details Request::get_details(const Exception& e) {
	const String* problem_source=e.problem_source();
	VHash& vhash=*new VHash;  HashStringValue& hash=vhash.hash();
	Operation::Origin origin={0, 0, 0};

	if(!exception_trace.is_empty()) {
		Trace bottom=exception_trace.bottom_value();
		origin=bottom.origin();
		if(!problem_source) { // we don't know who trigged the bug
			problem_source=bottom.name(); // we usually know source of next-from-throw-point exception did that
			exception_trace.set_bottom_index(exception_trace.bottom_index()+1);
		} else if (bottom.name()==problem_source) { // it is that same guy?
			exception_trace.set_bottom_index(exception_trace.bottom_index()+1); // throw away that trace
		} else {
			// stack top contains not us, leaving intact to help ^throw
		}
	}

	// $.type
	if(const char* type=e.type(true))
		hash.put(exception_type_part_name, new VString(*new String(type)));

	// $.source
	if(problem_source)
		hash.put(exception_source_part_name, new VString(*new String(*problem_source, String::L_TAINTED)));

	// $.file $.lineno $.colno
	if(origin.file_no){
		hash.put("file", new VString(*new String(file_list[origin.file_no], String::L_TAINTED)));
		hash.put("lineno", new VInt(1+origin.line));
		hash.put("colno", new VInt(1+origin.col));
	}

	// $.comment
	if(const char* comment=e.comment(true))
		hash.put(exception_comment_part_name, new VString(*new String(comment, String::L_TAINTED)));

	// $.handled(0)
	hash.put(exception_handled_part_name, &VBool::get(false));

	return Request::Exception_details(origin, problem_source, vhash);
}

Temp_value_element::Temp_value_element(Request& arequest, Value& awhere, const String& aname, Value* awhat) :
	frequest(arequest),
	fwhere(awhere),
	fname(aname),
	saved(awhere.get_element(aname))
{
	Junction* junction;
	if(saved && (junction=saved->get_junction()) && junction->is_getter)
		saved=0;
	frequest.put_element(fwhere, aname, awhat);
}

Temp_value_element::~Temp_value_element() {
	frequest.put_element(fwhere, fname, saved ? saved : VVoid::get());
}
