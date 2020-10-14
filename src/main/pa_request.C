/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
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
#include "pa_vmail.h"
#include "pa_vform.h"
#include "pa_vcookie.h"
#include "pa_vresponse.h"
#include "pa_vmemory.h"
#include "pa_vconsole.h"
#include "pa_vdate.h"

volatile const char * IDENT_PA_REQUEST_C="$Id: pa_request.C,v 1.381 2020/10/14 00:07:42 moko Exp $" IDENT_PA_REQUEST_H IDENT_PA_REQUEST_CHARSETS_H IDENT_PA_REQUEST_INFO_H IDENT_PA_VCONSOLE_H;

// consts

#define UNHANDLED_EXCEPTION_METHOD_NAME "unhandled_exception"

/// content type of response when no $MAIN:defaults.content-type defined
const char* DEFAULT_CONTENT_TYPE="text/html";

const uint LOOP_LIMIT=20000;
const uint EXECUTE_RECOURSION_LIMIT=1000;
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
size_t pa_file_size_limit=FILE_SIZE_LIMIT;

// defines for statics

#define CHARSETS_NAME "CHARSETS"
#define MIME_TYPES_NAME "MIME-TYPES"
#define STRICT_VARS_NAME "STRICT-VARS"
#define PROTOTYPE_NAME "OBJECT-PROTOTYPE"
#define LIMITS_NAME "LIMITS"
#define LOOP_LIMIT_NAME "max_loop"
#define RECOURSION_LIMIT_NAME "max_recoursion"
#define FILE_SIZE_LIMIT_NAME "max_file_size"
#define LOCK_WAIT_TIMEOUT_NAME "lock_wait_timeout"
#define CONF_METHOD_NAME "conf"
#define POST_PROCESS_METHOD_NAME "postprocess"
#define CLASS_PATH_NAME "CLASS_PATH"
#define RESPONSE_BODY_FILE_NAME "file"

#define DOWNLOAD_NAME_UPPER "DOWNLOAD"
#define BODY_NAME_UPPER "BODY"

// statics

static const String charsets_name(CHARSETS_NAME);
static const String main_class_name(MAIN_CLASS_NAME);
static const String mime_types_name(MIME_TYPES_NAME);
static const String strict_vars_name(STRICT_VARS_NAME);
static const String prototype_name(PROTOTYPE_NAME);
static const String limits_name(LIMITS_NAME);
static const String loop_limit_name(LOOP_LIMIT_NAME);
static const String recoursion_limit_name(RECOURSION_LIMIT_NAME);
static const String file_size_limit_name(FILE_SIZE_LIMIT_NAME);
static const String lock_wait_timeout_name(LOCK_WAIT_TIMEOUT_NAME);

static const String conf_method_name(CONF_METHOD_NAME);
static const String post_process_method_name(POST_PROCESS_METHOD_NAME);
static const String class_path_name(CLASS_PATH_NAME);
static const String response_body_file_name(RESPONSE_BODY_FILE_NAME);

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
Request::Request(SAPI_Info& asapi_info, Request_info& arequest_info, 
				 String::Language adefault_lang):
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

static void load_charset(HashStringValue::key_type akey, HashStringValue::value_type avalue, Request_charsets* charsets) {
	pa_charsets.load_charset(*charsets, akey, avalue->as_string());
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
				throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":" CHARSETS_NAME " must be hash");
		}
	}

#ifdef STRICT_VARS
	VVoid::strict_vars=false;
	if(Value* strict_vars=conf_class.get_element(strict_vars_name)) {
		if(strict_vars->is_bool())
			VVoid::strict_vars=strict_vars->as_bool();
		else
			throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":" STRICT_VARS_NAME " must be bool");
	}
#endif

#ifdef OBJECT_PROTOTYPE
	VClass::prototype=true;
	if(Value* prototype=conf_class.get_element(prototype_name)) {
		if(prototype->is_bool())
			VClass::prototype=prototype->as_bool();
		else
			throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":" PROTOTYPE_NAME " must be bool");
	}
#endif

	Value* limits=conf_class.get_element(limits_name);

	pa_loop_limit=LOOP_LIMIT;
	if(limits)
		if(Value* loop_limit=limits->get_element(loop_limit_name)) {
			if(loop_limit->is_evaluated_expr()) {
				pa_loop_limit=loop_limit->as_int();
				if(pa_loop_limit==0) pa_loop_limit=INT_MAX;
			} else
				throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":LIMITS." LOOP_LIMIT_NAME " must be int");
		}

	pa_execute_recoursion_limit=EXECUTE_RECOURSION_LIMIT;
	if(limits)
		if(Value* recoursion_limit=limits->get_element(recoursion_limit_name)) {
			if(recoursion_limit->is_evaluated_expr()) {
				pa_execute_recoursion_limit=recoursion_limit->as_int();
				if(pa_execute_recoursion_limit==0) pa_execute_recoursion_limit=INT_MAX;
			} else
				throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":LIMITS." RECOURSION_LIMIT_NAME " must be int");
		}

	pa_file_size_limit=FILE_SIZE_LIMIT;
	if(limits)
		if(Value* file_size_limit=limits->get_element(file_size_limit_name)) {
			if(file_size_limit->is_evaluated_expr()) {
				double limit=file_size_limit->as_double();
				if(limit >= (double)SSIZE_MAX)
					throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":LIMITS." FILE_SIZE_LIMIT_NAME " must be less then %.15g", (double)SSIZE_MAX);
				pa_file_size_limit=(size_t)limit;
				if(pa_file_size_limit==0) pa_file_size_limit=SSIZE_MAX;
			} else
				throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":LIMITS." FILE_SIZE_LIMIT_NAME " must be number");
		}

	pa_lock_attempts=PA_LOCK_ATTEMPTS;
	if(limits)
		if(Value* lock_wait_timeout=limits->get_element(lock_wait_timeout_name)) {
			if(lock_wait_timeout->is_evaluated_expr()) {
				double limit=lock_wait_timeout->as_double();
				if(limit >= 3600*24)
					throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":LIMITS." LOCK_WAIT_TIMEOUT_NAME " must be less then %d", 3600*24);
				pa_lock_attempts=(unsigned int)(limit*2)+1;
			} else
				throw Exception(PARSER_RUNTIME, 0, "$" MAIN_CLASS_NAME ":LIMITS." LOCK_WAIT_TIMEOUT_NAME " must be number");
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
void Request::core(const char* config_filespec, bool config_fail_on_read_problem, bool header_only) {
	try {
		// loading config
		if(config_filespec) {
			const String& filespec=*new String(config_filespec);
			use_file_directly(main_class, filespec, config_fail_on_read_problem, true /*file must exist if 'fail on read problem' not set*/);
		}

		// filling mail received
		mail.fill_received(*this);

		// loading auto.p files from document_root/.. 
		// to the one beside requested file.
		// all assigned bases from upper dir
		{
			const char* after=request_info.path_translated;
			size_t drlen=strlen(request_info.document_root);
			if(memcmp(after, request_info.document_root, drlen)==0) {
				after+=drlen;
				if(after[-1]=='/') 
					--after;
			}
			
			while(const char* before=strchr(after, '/')) {
				String& sfile_spec=*new String;
				if(after!=request_info.path_translated) {
					sfile_spec.append_strdup(request_info.path_translated, before-request_info.path_translated, String::L_CLEAN);
					sfile_spec << "/" AUTO_FILE_NAME;

					use_file_directly(main_class, sfile_spec, true /*fail on read problem*/, false /*but ignore absence, sole user*/);
				}
				for(after=before+1;*after=='/';after++);
			}
		}

		try {
			// compile requested file
			String& spath_translated=*new String;
			spath_translated.append_help_length(request_info.path_translated, 0, String::L_TAINTED);
			use_file_directly(main_class, spath_translated);

			configure();
		} catch(...) {
			configure(); // configure anyway, useful in @unhandled_exception [say, if they would want to mail by SMTP something]
			rethrow;
		}

		// execute @main[]
		const String* body_string=execute_virtual_method(main_class, main_method_name);
		if(!body_string)
			throw Exception(PARSER_RUNTIME, 0, "'" MAIN_METHOD_NAME "' method not found");

		// extract response body
		Value* body_value=response.fields().get(download_name_upper); // $response:download?
		bool as_attachment=body_value!=0;
		if(!body_value)
			body_value=response.fields().get(body_name_upper); // $response:body
		if(!body_value)
			body_value=new VString(*body_string); // just result of ^main[]

		// @postprocess
		if(Value* value=main_class.get_element(post_process_method_name))
			if(Junction* junction=value->get_junction())
				if(const Method *method=junction->method) {
					// preparing to pass parameters to 
					//	@postprocess[data]
					METHOD_FRAME_ACTION(*method, 0 /*no parent*/, main_class, {
						frame.store_params(&body_value, 1);
						call(frame);
						body_value=&frame.result();
					});
				}

		VFile* body_file=body_value->as_vfile(flang, &charsets);

		// OK. write out the result
		output_result(body_file, header_only, as_attachment);

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

		// maybe we'd be lucky enough as to report an error
		// in a gracefull way...
		if(Value* value=main_class.get_element(*new String(UNHANDLED_EXCEPTION_METHOD_NAME))) {
			if(Junction* junction=value->get_junction()) {
				if(const Method *method=junction->method) {
					// preparing to pass parameters to 
					//	@unhandled_exception[exception;stack]

					// $stack[^table::create{name	file	lineno	colno}]
					Table::columns_type stack_trace_columns(new ArrayString);
					*stack_trace_columns+=new String("name");
					*stack_trace_columns+=new String("file");
					*stack_trace_columns+=new String("lineno");
					*stack_trace_columns+=new String("colno");
					Table& stack_trace=*new Table(stack_trace_columns);
					if(!exception_trace.is_empty()/*signed!*/) 
						for(size_t i=exception_trace.bottom_index(); i<exception_trace.top_index(); i++) {
							Trace trace=exception_trace.get(i);
							Table::element_type row(new ArrayString);

							*row+=trace.name(); // name column
							Operation::Origin origin=trace.origin();
							if(origin.file_no) {
								*row+=new String(file_list[origin.file_no], String::L_TAINTED); // 'file' column
								*row+=new String(String::Body::Format(1+origin.line), String::L_CLEAN); // 'lineno' column
								*row+=new String(String::Body::Format(1+origin.col), String::L_CLEAN); // 'colno' column
							}
							stack_trace+=row;
						}

					// future $response:body=
					//   execute ^unhandled_exception[exception;stack]
					exception_trace.clear(); // forget all about previous life, in case there would be error inside of this method, error handled  would not be mislead by old stack contents (see extract_origin)

					Value *params[]={&details.vhash, new VTable(&stack_trace)};
					METHOD_FRAME_ACTION(*method, 0 /*no caller*/, main_class, {
						frame.store_params(params, 2);
						call(frame);
						body_string=&frame.result().as_string();
					});
				}
			}
		}
		
		// conditionally log it
		Value* vhandled=details.vhash.hash().get(exception_handled_part_name);
		if(!vhandled || !vhandled->as_bool()) {
			SAPI::log(sapi_info, "%s", exception_cstr);
		}

		if(body_string) {  // could report an error beautifully?
			VString body_vstring(*body_string);
			VFile* body_file=body_vstring.as_vfile(flang, &charsets);
			// write it out the error
			output_result(body_file, header_only, false);
		} else {
			// doing that ugly
			SAPI::send_error(sapi_info, exception_cstr, !strcmp(e.type(), "file.missing") ? "404" : "500");
		}

		} catch(const Exception& e) { // exception in unhandled exception
			Request::Exception_details details=get_details(e);
			const char* exception_cstr=get_exception_cstr(e, details);
			// unconditionally log the beast in exception handler
			throw Exception(0, 0, "Unhandled exception in %s", exception_cstr);
		}
	}
}

uint Request::register_file(String::Body file_spec) {
	file_list+=file_spec;
	return file_list.count()-1;
}

void Request::use_file_directly(VStateless_class& aclass, const String& file_spec, bool fail_on_read_problem, bool fail_on_file_absence) {
	// cyclic dependence check
	if(used_files.get(file_spec))
		return;
	used_files.put(file_spec, true);

	if(fail_on_read_problem && !fail_on_file_absence) // ignore file absence if asked for
		if(!entry_exists(file_spec))
			return;

	if(const char* source=file_read_text(charsets, file_spec, fail_on_read_problem))
		use_buf(aclass, source, 0, register_file(file_spec));
}


void Request::use_file(VStateless_class& aclass, const String& file_name, const String* use_filespec/*absolute*/) {
	if(file_name.is_empty())
		throw Exception(PARSER_RUNTIME, 0, "usage failed - no filename was specified");

	const String* filespec=0;

	if(file_name.first_char()=='/') //absolute path? [no need to scan MAIN:CLASS_PATH]
		filespec=&absolute(file_name);
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
				filespec=file_exist(absolute(element->as_string()), file_name); // found at class_path?
			} else if(Table *table=element->get_table()) {
				for(size_t i=table->count(); i--; ) {
					const String& path=*(*table->get(i))[0];
					if(filespec=file_exist(absolute(path), file_name))
						break; // found along class_path
				}
			} else
				throw Exception(PARSER_RUNTIME, 0, "$" CLASS_PATH_NAME " must be string or table");
			if(!filespec)
				throw Exception(PARSER_RUNTIME, &file_name, "not found along $" MAIN_CLASS_NAME ":" CLASS_PATH_NAME);
		} else 
			throw Exception(PARSER_RUNTIME, &file_name, "usage failed - no $" MAIN_CLASS_NAME  ":" CLASS_PATH_NAME " were specified");
	}

	use_file_directly(aclass, *filespec);
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
		Execute_nonvirtual_method_result executed=execute_nonvirtual_method(cclass, conf_method_name, vfilespec, false/*no string result needed*/);
		if(executed.method)
			configure_admin(cclass/*, executed.method->name*/);

		// locate and execute possible @auto[] static
		execute_nonvirtual_method(cclass, auto_method_name, vfilespec, false/*no result needed*/);

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

const String& Request::absolute(const String& relative_name) {
	if(relative_name.first_char()=='/') {
		String& result=*new String(pa_strdup(request_info.document_root));
		result << relative_name;
		return result;
	} else 
		if(relative_name.pos("://")!=STRING_NOT_FOUND // something like "http://xxx"
#ifdef WIN32
			|| relative_name.pos(":")==1  // DRIVE:
			|| relative_name.starts_with("\\\\") // UNC1
			|| relative_name.starts_with("//") // UNC2
#endif
			)
			return relative_name;
		else
			return relative(request_info.path_translated, relative_name);
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
	size_t start;
	size_t end;
};
#endif
static void parse_range(const String* s, Array<Range> &ar) {
	const char *p = s->cstr();
	if(s->starts_with("bytes="))
		p += 6;
	Range r;
	while(*p){
		r.start = (size_t)-1;
		r.end = (size_t)-1;
		if(*p >= '0' && *p <= '9'){
			r.start = atol(p);
			while(*p>='0' && *p<='9') ++p;
		}
		if(*p++ != '-') break;
		if(*p >= '0' && *p <= '9'){
			r.end = atol(p);
			while(*p>='0' && *p<='9') ++p;
		}
		if(*p == ',') ++p;
		ar += r;
	}
}

static void output_pieces(Request& r, bool header_only, const String& filename, size_t content_length, Value& date, bool add_last_modified) {
	SAPI::add_header_attribute(r.sapi_info, "accept-ranges", "bytes");

	const size_t BUFSIZE = 128*0x400;
	char buf[BUFSIZE];
	const char *range = SAPI::Env::get(r.sapi_info, "HTTP_RANGE");
	size_t offset=0;
	size_t part_length=content_length;
	if(range){
		Array<Range> ar;
		parse_range(new String(range), ar);
		size_t count = ar.count();
		if(count == 1){
			Range &rg = ar.get_ref(0);
			if(rg.start == (size_t)-1 && rg.end == (size_t)-1){
				SAPI::add_header_attribute(r.sapi_info, HTTP_STATUS, "416 Requested Range Not Satisfiable");
				return;
			}
			if(rg.start == (size_t)-1 && rg.end != (size_t)-1){
				rg.start = content_length - rg.end;
				rg.end = content_length;
				offset += rg.start;
				part_length = rg.end-rg.start;
			}else if(rg.start != (size_t)-1 && rg.end == (size_t)-1){
				rg.end = content_length-1;
				offset += rg.start;
				part_length -= rg.start;
			}
			if(part_length == 0){
				SAPI::add_header_attribute(r.sapi_info, HTTP_STATUS, "204 No Content");
				return;
			}
			SAPI::add_header_attribute(r.sapi_info, HTTP_STATUS, "206 Partial Content");
			snprintf(buf, BUFSIZE, "bytes %u-%u/%u", rg.start, rg.end, content_length);
			SAPI::add_header_attribute(r.sapi_info, "content-range", buf);
		}else if(count != 0){
			SAPI::add_header_attribute(r.sapi_info, HTTP_STATUS, "501 Not Implemented");
			return;
		}
	}


	SAPI::add_header_attribute(r.sapi_info, HTTP_CONTENT_LENGTH, format(part_length, "%u"));

	if(add_last_modified)
		SAPI::add_header_attribute(r.sapi_info, "last-modified", attributed_meaning_to_string(date, String::L_AS_IS, true).cstr());

	SAPI::send_header(r.sapi_info);

	const String& filespec=r.absolute(filename);

	size_t sent = 0;
	if(!header_only){
		size_t to_read = 0;
		size_t size = 0;
		do{
			to_read = part_length < BUFSIZE ? part_length : BUFSIZE;
			File_read_result read_result=file_read_binary(filespec, true /*fail on problem*/, buf, offset, to_read);
			to_read=read_result.length;
			if(to_read == 0)
				break;
			offset += to_read;

			size = SAPI::send_body(r.sapi_info, read_result.str, to_read);
			sent += size;
			if(size != to_read)
				break;
			part_length -= to_read;
		}while(part_length);
	}
}

void Request::output_result(VFile* body_file, bool header_only, bool as_attachment) {
	// header: cookies
	cookie.output_result(sapi_info);
	
	// _file_ content-type might be specified
	Value* body_file_content_type=body_file->fields().get(content_type_name);

	// Content-Disposition
	Value* vfile_name=body_file->fields().get(name_name);
	if(!vfile_name) {
		vfile_name=body_file->fields().get(response_body_file_name);
		if(vfile_name) {
			char* name_cstr=vfile_name->as_string().cstrm();
			if(char *after_slash=rsplit(name_cstr, '\\'))
				name_cstr=after_slash;
			if(char *after_slash=rsplit(name_cstr, '/'))
				name_cstr=after_slash;
			vfile_name=new VString(*new String(name_cstr));
		}
	}
	if(vfile_name) {
		const String& sfile_name=vfile_name->as_string();
		if(sfile_name!=NONAME_DAT) {
			VHash& hash=*new VHash();
			HashStringValue &h=hash.hash();
			h.put(value_name, new VString( as_attachment ? content_disposition_attachment : content_disposition_inline ));
			h.put(content_disposition_filename_name, new VString(*new String(sfile_name, String::L_HTTP_HEADER)));

			response.fields().put(content_disposition_name_upper, &hash);

			if(!body_file_content_type)
				body_file_content_type=new VString(mime_type_of(sfile_name.cstr()));
		}
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

	if(Value* vresponse_body_file=body_file->fields().get(response_body_file_name)) {
		// $response:[download|body][$.file[filespec]] -- optput specified file
		const String& sresponse_body_file=vresponse_body_file->as_string();
		uint64_t content_length=0;
		time_t atime=0, mtime=0, ctime=0;
		file_stat(absolute(sresponse_body_file), content_length, atime, mtime, ctime);

		VDate* vdate=0;
		if(Value* v=body_file->fields().get("mdate")) {
			if(Value* vdatep=v->as(VDATE_TYPE))
				vdate=static_cast<VDate*>(vdatep);
			else 
				throw Exception(PARSER_RUNTIME, 0, "mdate must be a date");
		}
		if(!vdate)
			vdate=new VDate((pa_time_t)mtime);

		output_pieces(*this, header_only, sresponse_body_file, (size_t)content_length, *vdate, info.add_last_modified);
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
					throw Exception(PARSER_RUNTIME, 0, MIME_TYPES_NAME  " table column elements must not be empty");
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
