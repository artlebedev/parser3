/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_REQUEST_C="$Date: 2009/08/30 06:05:56 $";

#include "pa_sapi.h"
#include "pa_common.h"
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

// consts

#define UNHANDLED_EXCEPTION_METHOD_NAME "unhandled_exception"

/// content type of exception response, when no @MAIN:exception handler defined
const char* UNHANDLED_EXCEPTION_CONTENT_TYPE="text/plain";

/// content type of response when no $MAIN:defaults.content-type defined
const char* DEFAULT_CONTENT_TYPE="text/html";
const char* ORIGINS_CONTENT_TYPE="text/plain";

// defines for globals

#define MAIN_METHOD_NAME "main"
#define AUTO_METHOD_NAME "auto"
#define AUTOUSE_METHOD_NAME "autouse"
#define BODY_NAME "body"
#define EXCEPTION_TYPE_PART_NAME "type"
#define EXCEPTION_SOURCE_PART_NAME "source"
#define EXCEPTION_COMMENT_PART_NAME "comment"

// globals

const String main_method_name(MAIN_METHOD_NAME);
const String auto_method_name(AUTO_METHOD_NAME);
const String autouse_method_name(AUTOUSE_METHOD_NAME);

const String body_name(BODY_NAME);
const String exception_type_part_name(EXCEPTION_TYPE_PART_NAME);
const String exception_source_part_name(EXCEPTION_SOURCE_PART_NAME);
const String exception_comment_part_name(EXCEPTION_COMMENT_PART_NAME);
const String exception_handled_part_name(EXCEPTION_HANDLED_PART_NAME);

// defines for statics

#define CHARSETS_NAME "CHARSETS"
#define MIME_TYPES_NAME "MIME-TYPES"
#define ORIGINS_MODE_NAME "ORIGINS"
#define CONF_METHOD_NAME "conf"
#define POST_PROCESS_METHOD_NAME "postprocess"
#define DOWNLOAD_NAME "download"
#define CLASS_PATH_NAME "CLASS_PATH"
#define RESPONSE_BODY_FILE_NAME "file"

// statics

static const String charsets_name(CHARSETS_NAME);
static const String main_class_name(MAIN_CLASS_NAME);
static const String mime_types_name(MIME_TYPES_NAME);
static const String origins_mode_name(ORIGINS_MODE_NAME);
static const String conf_method_name(CONF_METHOD_NAME);
static const String post_process_method_name(POST_PROCESS_METHOD_NAME);
static const String download_name(DOWNLOAD_NAME);
static const String class_path_name(CLASS_PATH_NAME);
static const String response_body_file_name(RESPONSE_BODY_FILE_NAME);

// defines

// op.C
VStateless_class& VClassMAIN_create();

//
Request::Request(SAPI_Info& asapi_info, Request_info& arequest_info, 
				 String::Language adefault_lang, bool status_allowed):
	// private
	anti_endless_execute_recoursion(0),

	// public
	method_frame(0),
	rcontext(0),
	wcontext(0),
	flang(adefault_lang),
	fconnection(0),
	finterrupted(false),
	fskip(SKIP_NOTHING),
	fin_cycle(0),

	// public
#ifdef RESOURCES_DEBUG
	sql_connect_time(0),
	sql_request_time(0),
#endif	

	// public
	request_info(arequest_info),
	sapi_info(asapi_info),
	charsets(UTF8_charset, UTF8_charset, UTF8_charset), // default charsets

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
	file_list+=String::Body("UNKNOWN");
	file_list+=String::Body("-body of process-"); // pseudo_file_no__process

	// maybe expire old caches
	cache_managers->maybe_expire();
	
	/// directly used
	// MAIN class, operators
	classes().put(main_class.name(), &main_class);
	// classes:
	// table, file, random, mail, image, ...
	methoded_array().register_directly_used(*this);

	/// methodless
	// env class
	classes().put(String::Body(ENV_CLASS_NAME), new VEnv(asapi_info));
	// status class
	if(status_allowed)
		classes().put(String::Body(STATUS_CLASS_NAME), new VStatus());
	// request class
	classes().put(String::Body(REQUEST_CLASS_NAME), new VRequest(arequest_info, charsets, form));	
	// cookie class
	classes().put(String::Body(COOKIE_CLASS_NAME), &cookie);
	// console class
	classes().put(String::Body(CONSOLE_CLASS_NAME), &console);
	/// methoded
	// response class
	classes().put(response.get_class()->name(), &response);

	/// bases used
	// form class
	classes().put(form.get_class()->base_class()->name(), &form);
	// mail class
	classes().put(mail.get_class()->base_class()->name(), &mail);
	// math class
	{
		Value& math=*new VMath;
		classes().put(math.get_class()->base_class()->name(), &math);
	}
	// memory class
	{
		Value& memory=*new VMemory;
		classes().put(memory.get_class()->base_class()->name(), &memory);
	}
}

Request::~Request() {
#ifdef XML
	// if for some strange reason xml generic errors failed to be reported, free them up
	if(const char* xml_generic_errors=xmlGenericErrors()) {
		SAPI::log(sapi_info, "warning: unreported xmlGenericErrors: %s", xml_generic_errors);
		free((void *)xml_generic_errors);
	}
#endif
}

Value& Request::get_self() { return method_frame/*always have!*/->self(); }

Value* Request::get_class(const String& name){
	Value* result=classes().get(name);
	if(!result){
		if(Value* value=main_class.get_element(autouse_method_name))
			if(Junction* junction=value->get_junction())
				if(const Method *method=junction->method) {
					Value *vname=new VString(name);
					VMethodFrame frame(*junction, 0/*no parent*/);

					frame.set_self(main_class);
					frame.store_params(&vname, 1);
					// we don't need the result
					execute_method(frame, *method);

					result=classes().get(name);
				}
	}
	return result;
}

static void load_charset(HashStringValue::key_type akey, 
			 HashStringValue::value_type avalue, 
			 Request_charsets* charsets) {
	const String::Body NAME=String(akey, String::L_CLEAN).change_case(charsets->source(), String::CC_UPPER);
	::charsets.load_charset(*charsets, NAME, avalue->as_string());
}
void Request::configure_admin(VStateless_class& conf_class) {
	if(configure_admin_done)
		throw Exception(PARSER_RUNTIME,
		0,
		"parser already configured");
	configure_admin_done=true;
	
	// charsets must only be specified in method_frame config
	// so that users would not interfere

	/* $MAIN:CHARSETS[
			$.charsetname1[/full/path/to/charset/file.cfg]
			...
		]
	*/
	if(Value* vcharsets=conf_class.get_element(charsets_name)) {
		if(!vcharsets->is_string())
			if(HashStringValue* charsets=vcharsets->get_hash())
				charsets->for_each<Request_charsets*>(load_charset, &this->charsets);
			else
				throw Exception(PARSER_RUNTIME,
					0,
					"$" MAIN_CLASS_NAME ":" CHARSETS_NAME " must be hash");
	}

	// configure method_frame options
	//	until someone with less privileges have overriden them
	methoded_array().configure_admin(*this);
}

const char* Request::get_exception_cstr(
										const Exception& e,
										Request::Exception_details& details) {
#define PA_URI_FORMAT "%s: "
#define PA_ORIGIN_FILE_POS_FORMAT "%s(%d:%d): "
#define PA_SOURCE_FORMAT "'%s' "
#define PA_COMMENT_TYPE_FORMAT "%s [%s]"
	char* result=new(PointerFreeGC) char[MAX_STRING];

	if(details.problem_source) { // do we know the guy?
		if(details.trace) { // do whe know where he came from?
			Operation::Origin origin=details.trace.origin();
			snprintf(result, MAX_STRING, 
				PA_URI_FORMAT 
				PA_ORIGIN_FILE_POS_FORMAT 
				PA_SOURCE_FORMAT PA_COMMENT_TYPE_FORMAT,
				request_info.uri,
				file_list[origin.file_no].cstr(), 1+origin.line, 1+origin.col,
				details.problem_source->cstr(),
				e.comment(), e.type()
			);
		} else
			snprintf(result, MAX_STRING, 
				PA_URI_FORMAT 
				PA_SOURCE_FORMAT PA_COMMENT_TYPE_FORMAT,
				request_info.uri,
				details.problem_source->cstr(),
				e.comment(), e.type()
			);
	} else
		snprintf(result, MAX_STRING, 
			PA_URI_FORMAT 
			PA_COMMENT_TYPE_FORMAT,
			request_info.uri,
			e.comment(), e.type()
		);

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
#ifdef RESOURCES_DEBUG
//measures
struct timeval mt[10];
//measure:before all
gettimeofday(&mt[0],NULL);
#endif
	try {
		// filling mail received
		mail.fill_received(*this);

		// loading config
		if(config_filespec) {
			const String& filespec=*new String(config_filespec);
			use_file(main_class,
				filespec, 0 /*main_alias*/, 
				true /*ignore class_path*/, 
				config_fail_on_read_problem, 
				true /*file must exist if 'fail on read problem' not set*/);
		}

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
					sfile_spec.append_strdup(
						request_info.path_translated, before-request_info.path_translated,
						String::L_CLEAN);
					sfile_spec << "/" AUTO_FILE_NAME;

					use_file(main_class,
						sfile_spec, 
						0 /*main_alias*/,
						true /*ignore class_path*/, 
						true /*fail on read problem*/, 
						false /*but ignore absence, sole user*/);
				}
				after=before+1;
			}
		}

		try {
			// compile requested file
			String& spath_translated=*new String;
			spath_translated.append_help_length(request_info.path_translated, 0, String::L_TAINTED);
			use_file(main_class,
				spath_translated, 
				0 /*main_alias*/,
				true /*ignore class_path*/);

			configure();
		} catch(...) {
			configure(); // configure anyway, useful in @unhandled_exception [say, if they would want to mail by SMTP something]
			rethrow;
		}

#ifdef RESOURCES_DEBUG
//measure:after compile
gettimeofday(&mt[1],NULL);
#endif
		// execute @main[]
		const String* body_string=execute_virtual_method(main_class, main_method_name);
		if(!body_string)
			throw Exception(PARSER_RUNTIME,
				0,
				"'"MAIN_METHOD_NAME"' method not found");

#ifdef RESOURCES_DEBUG
		//measure:after main
gettimeofday(&mt[2],NULL);
#endif

		// extract response body
		Value* body_value=response.fields().get(download_name); // $response:download?
		bool as_attachment=body_value!=0;
		if(!body_value)
			body_value=response.fields().get(body_name); // $response:body
		if(!body_value)
			body_value=new VString(*body_string); // just result of ^main[]

		// @postprocess
		if(Value* value=main_class.get_element(post_process_method_name))
			if(Junction* junction=value->get_junction())
				if(const Method *method=junction->method) {
					// preparing to pass parameters to 
					//	@postprocess[data]
					VMethodFrame frame(/*method->name, */ *junction, 0/*no parent*/);
					frame.set_self(main_class);

					frame.store_params(&body_value, 1);
					body_value=&execute_method(frame, *method).as_value();
				}

		VFile* body_file=body_value->as_vfile(flang, &charsets);

#ifdef RESOURCES_DEBUG
//measure:after postprocess
gettimeofday(&mt[3],NULL);		
#endif

		// OK. write out the result
		output_result(body_file, header_only, as_attachment);

#ifdef RESOURCES_DEBUG
		//measure:after output_result
gettimeofday(&mt[9],NULL);		
t[9]=mt[9].tv_sec+mt[9].tv_usec/1000000.0;

double t[10];
for(int i=0;i<10;i++)
    t[i]=mt[i].tv_sec+mt[i].tv_usec/1000000.0;
//measure:log2 compile,main,postprocess,output
SAPI::log("rmeasure: %s,%.2f,%.2f,%.2f %.2f,%.2f %.2f", 
request_info.uri,
t[1]-t[0],
t[2]-t[1],
t[3]-t[2],
sql_connect_time,sql_request_time,
t[9]-t[3]
);
#endif
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
					VMethodFrame frame(/*method->name, */ *junction, 0/*no caller*/);
					frame.set_self(main_class);

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

					Value *params[]={&details.vhash, new VTable(&stack_trace)};
					frame.store_params(params, 2);

					// future $response:body=
					//   execute ^unhandled_exception[exception;stack]
					exception_trace.clear(); // forget all about previous life, in case there would be error inside of this method, error handled  would not be mislead by old stack contents (see extract_origin)
					body_string=&execute_method(frame, *method).as_string();
				}
			}
		}
		
		if(!body_string) {  // couldn't report an error beautifully?
			// doing that ugly

			// future $response:content-type
			response.fields().remove(content_type_name);
			response.fields().put(http_content_type, 
				new VString(*new String(UNHANDLED_EXCEPTION_CONTENT_TYPE)));
			// future $response:body
			body_string=new String(exception_cstr);
		}

		VString body_vstring(*body_string);
		VFile* body_file=body_vstring.as_vfile(flang, &charsets);

		// conditionally log it
		Value* vhandled=details.vhash.hash().get(exception_handled_part_name);
		if(!vhandled || !vhandled->as_bool()) {
			SAPI::log(sapi_info, "%s", exception_cstr);
		}

		// ERROR. write it out
		output_result(body_file, header_only, false);

		} catch(const Exception& e) { // exception in unhandled exception
			Request::Exception_details details=get_details(e);
			const char* exception_cstr=get_exception_cstr(e, details);
			// unconditionally log the beast
			SAPI::log(sapi_info, "%s", exception_cstr);

			throw Exception(0,
				0,
				"in %s", 
					exception_cstr);
		}
	}
}

uint Request::register_file(String::Body file_spec) {
	file_list+=file_spec;
	return file_list.count()-1;
}

void Request::use_file(VStateless_class& aclass,
		       const String& file_name, const String* main_alias, 
		       bool ignore_class_path, 
		       bool fail_on_read_problem, 
		       bool fail_on_file_absence) {
	// cyclic dependence check
	if(used_files.get(file_name))
		return;
	used_files.put(file_name, true);

	const String* file_spec;
	if(ignore_class_path) // ignore_class_path?
		file_spec=&file_name;
	else if(file_name.first_char()=='/') //absolute path? [no need to scan MAIN:CLASS_PATH]
		file_spec=&absolute(file_name);
	else {
		file_spec=0;
		if(Value* element=main_class.get_element(class_path_name)) {
			if(element->is_string()) {
				file_spec=file_exist(absolute(element->as_string()), file_name); // found at class_path?
			} else if(Table *table=element->get_table()) {
				int size=table->count();
				for(int i=size; i--; ) {
					const String& path=*(*table->get(i))[0];
					if((file_spec=file_exist(absolute(path), file_name)))
						break; // found along class_path
				}
			} else
				throw Exception(PARSER_RUNTIME,
					0,
					"$" CLASS_PATH_NAME " must be string or table");
			if(!file_spec)
				throw Exception(PARSER_RUNTIME,
					&file_name,
					"not found along " MAIN_CLASS_NAME ":" CLASS_PATH_NAME);
		}
		if(!file_spec)
			throw Exception(PARSER_RUNTIME,
				&file_name,
				"usage failed - no $" MAIN_CLASS_NAME  ":" CLASS_PATH_NAME " were specified");
	}

	if(fail_on_read_problem && !fail_on_file_absence) // ignore file absence if asked for
		if(!entry_exists(*file_spec))
			return;

	if(const char* source=file_read_text(charsets, *file_spec, fail_on_read_problem))
		use_buf(aclass, source, main_alias, register_file(*file_spec));
}


void Request::use_buf(VStateless_class& aclass,
		      const char* source, const String* main_alias, 
		      uint file_no,
			  int line_no_offset) {
	// temporary zero @conf so to maybe-replace it in compiled code
	Temp_method temp_method_conf(aclass, conf_method_name, 0);
	// temporary zero @auto so to maybe-replace it in compiled code
	Temp_method temp_method_auto(aclass, auto_method_name, 0);

	// compile loaded classes
	ArrayClass& cclasses=compile(&aclass, source, main_alias, file_no, line_no_offset);

	VString* vfilespec=
		new VString(*new String(file_list[file_no], String::L_TAINTED));

	for(size_t i=0; i<cclasses.count(); i++){
		VStateless_class& cclass=*cclasses.get(i);

		// locate and execute possible @conf[] static
		Execute_nonvirtual_method_result executed=execute_nonvirtual_method(cclass, 
			conf_method_name, vfilespec,
			false/*no string result needed*/);
		if(executed.method)
			configure_admin(cclass/*, executed.method->name*/);

		// locate and execute possible @auto[] static
		execute_nonvirtual_method(cclass, 
			auto_method_name, vfilespec,
			false/*no result needed*/);
	}
}

const String& Request::relative(const char* apath, const String& relative_name) {
	char *hpath=strdup(apath);
	String& result=*new String;
	if(rsplit(hpath, '/')) // if something/splitted
		result << hpath << "/";
	result << relative_name;
	return result;
}

const String& Request::absolute(const String& relative_name) {
	if(relative_name.first_char()=='/') {
		String& result=*new String(strdup(request_info.document_root));
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
static void add_header_attribute(
				 HashStringValue::key_type name, 
				 HashStringValue::value_type value, 
				 Add_header_attribute_info* info) {
	if(name==BODY_NAME
		|| name==DOWNLOAD_NAME
		|| name==CHARSET_NAME)
		return;
	
	const char* aname=String(name, String::L_URI).transcode_and_untaint_cstr(String::L_URI, &info->r.charsets);

	SAPI::add_header_attribute(info->r.sapi_info,
			aname, 
			attributed_meaning_to_string(*value, String::L_URI, false).transcode_and_untaint_cstr(String::L_URI, &info->r.charsets)
		);

	if(strcasecmp(aname, "last-modified")==0)
		info->add_last_modified=false;
}

static void output_sole_piece(Request& r,
							  bool header_only, 
							  VFile& body_file,
							  Value* body_file_content_type) {
	// transcode text body when "text/*" or simple result
	String::C output(body_file.value_ptr(), body_file.value_size());
	if(!body_file_content_type/*vstring.as_vfile*/ || body_file_content_type->as_string().pos("text/")==0)
		output=Charset::transcode(output, 
			r.charsets.source(), 
			r.charsets.client());

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
static void parse_range(const String* s, Array<Range> &ar)
{
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

static void output_pieces(Request& r,
						  bool header_only, 
						  const String& filename,
						  size_t content_length,
						  Value& date,
						  bool add_last_modified) 
{
	SAPI::add_header_attribute(r.sapi_info, "Accept-Ranges", "bytes");

	const size_t BUFSIZE = 10*0x400;
	char buf[BUFSIZE];
	const char *range = SAPI::get_env(r.sapi_info, "HTTP_RANGE");
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
			SAPI::add_header_attribute(r.sapi_info, "Content-Range", buf);
		}else if(count != 0){
			SAPI::add_header_attribute(r.sapi_info, HTTP_STATUS, "501 Not Implemented");
			return;
		}
	}


	SAPI::add_header_attribute(r.sapi_info, HTTP_CONTENT_LENGTH, format(part_length, "%u"));

	if(add_last_modified){
		const String &s = attributed_meaning_to_string(date, String::L_AS_IS, true);
		SAPI::add_header_attribute(r.sapi_info, "Last-Modified", s.cstr());
	}
	SAPI::send_header(r.sapi_info);

	const String& filespec=r.absolute(filename);

	size_t sent = 0;
	if(!header_only){
		size_t to_read = 0;
		size_t size = 0;
		do{
			to_read = part_length<BUFSIZE?part_length:BUFSIZE;
			File_read_result read_result=file_read(r.charsets, filespec, 
				false /*as text*/,  0/*params*/, true/*fail on problem*/,
				buf, offset, to_read);
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
	
	// may be specified
	Value* body_file_content_type=body_file->fields().get(content_type_name);

	// content-disposition
	Value* vfile_name=body_file->fields().get(name_name);
	if(!vfile_name) {
		vfile_name=body_file->fields().get(response_body_file_name);
		if(vfile_name)
		{
			const String& sfile_name=vfile_name->as_string();

			char* name_cstr=sfile_name.cstrm();
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

			h.put(content_disposition_filename_name, vfile_name);
			response.fields().put(content_disposition, &hash);

			if(!body_file_content_type)
				body_file_content_type=new VString(mime_type_of(sfile_name.cstr()));
		}
	}

	// set Content-Type
	response.fields().put(http_content_type,
		body_file_content_type
			? body_file_content_type
			: response.fields().get(content_type_name)
				? response.fields().get(content_type_name)
				: new VString(*new String(DEFAULT_CONTENT_TYPE)));

	response.fields().remove(content_type_name);

	// prepare header: $response:fields without :body
	Add_header_attribute_info info(*this);
	response.fields().for_each<Add_header_attribute_info*>(add_header_attribute, &info);

	if(body_file_content_type)
		if(HashStringValue *hash=body_file_content_type->get_hash())
			body_file_content_type=hash->get(value_name);

	if(Value* vresponse_body_file=body_file->fields().get(response_body_file_name)) {
		const String& sresponse_body_file=vresponse_body_file->as_string();
		size_t content_length=0;
		time_t atime=0, mtime=0, ctime=0;
		file_stat(absolute(sresponse_body_file),
			content_length,
			atime, mtime, ctime);

		VDate* vdate=0;
		if(Value* v=body_file->fields().get("mdate")) {
			if(Value* vdatep=v->as(VDATE_TYPE))
				vdate=static_cast<VDate*>(vdatep);
			else 
				throw Exception(PARSER_RUNTIME, 0, "mdate must be a date");
		}
		if(!vdate)
			vdate=new VDate(mtime);

		output_pieces(*this, header_only, 
			sresponse_body_file,
			content_length,
			*vdate,
			info.add_last_modified);
	} else {
		output_sole_piece(*this, header_only, 
			*body_file, body_file_content_type);
	}
}

const String& Request::mime_type_of(const char* user_file_name_cstr) {
	if(mime_types)
		if(const char* cext=strrchr(user_file_name_cstr, '.')) {
			String sext(++cext);
			Table::Action_options options;
			if(mime_types->locate(0, sext.change_case(charsets.source(), String::CC_LOWER), options))
					if(const String* result=mime_types->item(1))
						return *result;
				else
					throw Exception(PARSER_RUNTIME,
						0,
						MIME_TYPES_NAME  " table column elements must not be empty");
		}

	return *new String("application/octet-stream");
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


const Request::Trace Request::Exception_trace::extract_origin(const String*& problem_source) {
	Trace result;
	if(!is_empty()) {
		result=bottom_value();
		if(!problem_source) { // we don't know who trigged the bug?
			problem_source=result.name(); // consider the stack-top-guy (we usually know source of next-from-throw-point exception) did that
			fbottom++;
		} else if(result.name()==problem_source) // it is that same guy?
			fbottom++; // throw away that trace
		else {
			// stack top contains not us, 
			//	leaving result intact
			//	it would help ^throw
		}
	}

	return result;
}

Request::Exception_details Request::get_details(const Exception& e) {
	const String* problem_source=e.problem_source();
	Trace trace=exception_trace.extract_origin(problem_source);
	
	VHash& vhash=*new VHash;  HashStringValue& hash=vhash.hash();

	// $.type
	if(const char* type=e.type(true))
		hash.put(exception_type_part_name, new VString(*new String(type)));

	// $.source
	if(problem_source) {
		String& source=*new String(*problem_source, String::L_TAINTED);
		hash.put(exception_source_part_name, new VString(source));
	}

	// $.file lineno colno
	if(trace) {
		const Operation::Origin origin=trace.origin();
		hash.put(String::Body("file"),
			new VString(*new String(file_list[origin.file_no], String::L_TAINTED)));
		hash.put(String::Body("lineno"), new VInt(1+origin.line));
		hash.put(String::Body("colno"), new VInt(1+origin.col));
	}

	// $.comment
	if(const char* comment=e.comment(true))
		hash.put(exception_comment_part_name, 
			new VString(*new String(comment, String::L_TAINTED)));

	// $.handled(0)
	hash.put(exception_handled_part_name, &VBool::get(false));

	return Request::Exception_details(trace, problem_source, vhash);
}
