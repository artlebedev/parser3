/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_REQUEST_C="$Date: 2002/11/27 12:44:35 $";

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_vclass.h"
#include "pa_globals.h"
#include "pa_vint.h"
#include "pa_vmethod_frame.h"
#include "pa_types.h"
#include "pa_vtable.h"
#include "pa_vfile.h"
#include "pa_dictionary.h"
#include "pa_charset.h"
#include "pa_charsets.h"

const char *POST_PROCESS_METHOD_NAME="postprocess";
const char *UNHANDLED_EXCEPTION_METHOD_NAME="unhandled_exception";

/// content type of exception response, when no @MAIN:exception handler defined
const char *UNHANDLED_EXCEPTION_CONTENT_TYPE="text/plain";

/// content type of response when no $MAIN:defaults.content-type defined
const char *DEFAULT_CONTENT_TYPE="text/html";
const char *ORIGINS_CONTENT_TYPE="text/plain";

// op.C
VClass& VClassMAIN_create(Pool& pool);
// op.C
VHash& exception2vhash(Pool& pool, const Exception& e);

//
Request::Request(Pool& apool,
				 Info& ainfo,
				 uchar adefault_lang,
				 bool status_allowed) : Pooled(apool),
	main_method_name(apool, MAIN_METHOD_NAME),
	stack(apool),
	main_class(VClassMAIN_create(apool)),
	env(apool),
	status(apool),
	form(apool),
	mail(apool),
	math(apool),
	request(apool, *this),
	response(apool),
	cookie(apool),
	fclasses(apool),
	fdefault_lang(adefault_lang), flang(adefault_lang),
	info(ainfo),
	post_data(0), post_size(0),
	used_files(apool),
	configure_admin_done(false),
	default_content_type(0),
	mime_types(0),
	fconnection(0),
	classes_conf(apool),
	anti_endless_execute_recoursion(0),
	exception_trace(apool)
#ifdef RESOURCES_DEBUG
	, sql_connect_time(0),sql_request_time(0)
#endif
	, method_frame(0), rcontext(0), wcontext(0),
	finterrupted(false)
{
	// default charsets
	pool().set_source_charset(*utf8_charset);
	pool().set_client_charset(*utf8_charset);

	// maybe expire old caches
	cache_managers->maybe_expire();
	
	/// directly used
	// MAIN class, operators
	classes().put(main_class.name(), &main_class);
	// classes:
	// table, file, random, mail, image, ...
	methoded_array->register_directly_used(*this);

	/// methodless
	// env class
	classes().put(*NEW String(pool(), ENV_CLASS_NAME), &env);
	// status class
	if(status_allowed)
		classes().put(*NEW String(pool(), STATUS_CLASS_NAME), &status);
	// request class
	classes().put(*NEW String(pool(), REQUEST_CLASS_NAME), &request);	
	// cookie class
	classes().put(*NEW String(pool(), COOKIE_CLASS_NAME), &cookie);

	/// methoded
	// response class
	classes().put(response.get_class()->name(), &response);	

	/// bases used
	// form class
	classes().put(form.get_class()->base_class()->name(), &form);	
	// mail class
	classes().put(mail.get_class()->base_class()->name(), &mail);	
	// math class
	classes().put(math.get_class()->base_class()->name(), &math);	
}

Request::~Request() {
#ifdef XML
	// if for some strange reason xml generic errors failed to be reported, free them up
	if(const char *xml_generic_errors=xmlGenericErrors()) {
		SAPI::log(pool(), "warning: unreported xmlGenericErrors: %s", xml_generic_errors);
		free((void *)xml_generic_errors);
	}
#endif
}

Value *Request::get_self() { return method_frame?&method_frame->self():0; }

static void load_charset(const Hash::Key& akey, Hash::Val *avalue, void *) {
	charsets->load_charset(akey, static_cast<Value *>(avalue)->as_string());
}
void Request::configure_admin(VStateless_class& conf_class, const String *source) {
	if(configure_admin_done)
		throw Exception("parser.runtime",
		source,
		"parser already configured");
	configure_admin_done=true;
	
	// charsets must only be specified in method_frame config
	// so that users would not interfere

	/* $MAIN:CHARSETS[
			$.charsetname1[/full/path/to/charset/file.cfg]
			...
		]
	*/
	if(Value *vcharsets=conf_class.get_element(*charsets_name, conf_class, false)) {
		if(!vcharsets->is_string())
			if(Hash *charsets=vcharsets->get_hash(0))
				charsets->for_each(load_charset);
			else
				throw Exception("parser.runtime",
					0,
					"$" MAIN_CLASS_NAME ":" CHARSETS_NAME " must be hash");
	}

	// configure method_frame options
	//	until someone with less privileges have overriden them
	methoded_array->configure_admin(*this);
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
void Request::core(
				   const char *config_filespec, bool config_fail_on_read_problem,
				   bool header_only) {

#ifdef RESOURCES_DEBUG
//measures
struct timeval mt[10];
//measure:before all
gettimeofday(&mt[0],NULL);
#endif
	try {
		char *auto_filespec=(char *)malloc(MAX_STRING);
		
		// loading config
		if(config_filespec) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(config_filespec, 0, "config", 0);
			use_file(main_class,
				filespec, true/* ignore class_path */, 
				config_fail_on_read_problem, true/* file must exist if 'fail on read problem' not set */);
		}

		// loading auto.p files from document_root/.. 
		// to the one beside requested file.
		// all assigned bases from upper dir
		{
			const char *after=info.path_translated;
			size_t drlen=strlen(info.document_root);
			if(memcmp(after, info.document_root, drlen)==0) {
				after+=drlen;
				if(after[-1]=='/') 
					--after;
			}
			
			int step=0;
			while(const char *before=strchr(after, '/')) {
				String& sfile_spec=*NEW String(pool());
				if(after!=info.path_translated) {
					sfile_spec.APPEND_CLEAN(
						info.path_translated, before-info.path_translated,
						"path-translated-scanned", step++);
					sfile_spec << "/" AUTO_FILE_NAME;

					use_file(main_class,
						sfile_spec, true/* ignore class_path */, 
						true/* fail on read problem */, false/* but ignore absence, sole user */);
				}
				after=before+1;
			}
		}

		// compile requested file
		String& spath_translated=*NEW String(pool());
		spath_translated.APPEND_TAINTED(info.path_translated, 0, "user-request", 0);
		use_file(main_class,
			spath_translated, true/* ignore class_path */, 
			true/* fail on read problem*/, true/* fail on abscence */);

		// configure method_frame options if not configured yet
		if(!configure_admin_done)
			configure_admin(main_class, 0);

		// configure not-method_frame=user options
		methoded_array->configure_user(*this);

		// $MAIN:MIME-TYPES
		if(Value *element=main_class.get_element(*mime_types_name, main_class, false))
			if(Table *table=element->get_table())
				mime_types=table;			

		// filling form fields
		form.fill_fields_and_tables(*this);

		// filling cookies
		cookie.fill_fields(*this);

		// filling mail received
		mail.fill_received(*this);

#ifdef RESOURCES_DEBUG
//measure:after compile
gettimeofday(&mt[1],NULL);
#endif
		// execute @main[]
		const String *body_string=execute_virtual_method(main_class, main_method_name);
		if(!body_string)
			throw Exception("parser.runtime",
				0, 
				"'"MAIN_METHOD_NAME"' method not found");

#ifdef RESOURCES_DEBUG
		//measure:after main
gettimeofday(&mt[2],NULL);
#endif

		VString body_vstring_before_post_process(*body_string);
		VString *body_vstring_after_post_process=&body_vstring_before_post_process;
		// @postprocess
		if(Value *value=main_class.get_element(
				*NEW String(pool(), POST_PROCESS_METHOD_NAME), 
				main_class, 
				false))
			if(Junction *junction=value->get_junction())
				if(const Method *method=junction->method) {
					// preparing to pass parameters to 
					//	@postprocess[data]
					VMethodFrame frame(pool(), method->name, *junction, 0/*no parent*/);
					frame.set_self(main_class);

					frame.store_param(&body_vstring_before_post_process);
					body_vstring_after_post_process=
						NEW VString(execute_method(frame, *method));
				}

		bool lorigins_mode=origins_mode();

		const VFile *body_file=body_vstring_after_post_process->as_vfile(
			String::UL_UNSPECIFIED, lorigins_mode);

		// extract response body
		Value *body_value=static_cast<Value *>(
			response.fields().get(*body_name));
		if(body_value) // there is some $response:body
			body_file=body_value->as_vfile();
		else if(lorigins_mode)
			response.fields().put(*content_type_name, 
				NEW VString(*NEW String(pool(), ORIGINS_CONTENT_TYPE)));

#ifdef RESOURCES_DEBUG
//measure:after postprocess
gettimeofday(&mt[3],NULL);		
#endif

		// OK. write out the result
		output_result(*body_file, header_only);

#ifdef RESOURCES_DEBUG
		//measure:after output_result
gettimeofday(&mt[9],NULL);		
t[9]=mt[9].tv_sec+mt[9].tv_usec/1000000.0;

double t[10];
for(int i=0;i<10;i++)
    t[i]=mt[i].tv_sec+mt[i].tv_usec/1000000.0;
//measure:log2 compile,main,postprocess,output
SAPI::log(pool(), "rmeasure: %s,%.2f,%.2f,%.2f %.2f,%.2f %.2f", 
info.uri,
t[1]-t[0],
t[2]-t[1],
t[3]-t[2],
sql_connect_time,sql_request_time,
t[9]-t[3]
);
#endif
	} catch(const Exception& e) { // request handling problem
		// we're returning not result, but error explanation
		try {
			// log the beast
			if(const String *problem_source=e.problem_source())
				SAPI::log(pool(),
					"%s: "
#ifndef NO_STRING_ORIGIN
					ORIGIN_FILE_LINE_FORMAT": "
#endif
					"'%s' %s [%s]",
					info.uri,
#ifndef NO_STRING_ORIGIN
					problem_source->origin().file?problem_source->origin().file:"global",
					problem_source->origin().line,
#endif
					problem_source->cstr(),
					e.comment(),
					e.type()
				);
			else
				SAPI::log(pool(),
					"%s: "
					"%s [%s]",
					info.uri,
					e.comment(), e.type()
				);

			// reset language to default
			flang=fdefault_lang;
			
			// reset response
			response.fields().clear();

			// this is what we'd return in $response:body
			const String *body_string=0;

			// maybe we'd be lucky enough as to report an error
			// in a gracefull way...
			if(Value *value=main_class.get_element(
					*NEW String(pool(), UNHANDLED_EXCEPTION_METHOD_NAME), 
					main_class,
					false)) {
				if(Junction *junction=value->get_junction()) {
					if(const Method *method=junction->method) {
		 				// preparing to pass parameters to 
						//	@unhandled_exception[exception;stack]
						VMethodFrame frame(pool(), method->name, *junction, 0/*no caller*/);
						frame.set_self(main_class);

						// $exception
						frame.store_param(&exception2vhash(pool(), e));
						// $stack[^table::set{name	origin}]
						Array& stack_trace_columns=*NEW Array(pool());
						stack_trace_columns+=NEW String(pool(), "name");
						stack_trace_columns+=NEW String(pool(), "file");
						stack_trace_columns+=NEW String(pool(), "lineno");
						Table& stack_trace=*NEW Table(pool(), 0, &stack_trace_columns);
						Array_iter tracei(exception_trace);
						while(tracei.has_next()) {
							Array& row=*NEW Array(pool());

							const String *name=(const String *)tracei.next();
							row+=name; // name column
#ifndef NO_STRING_ORIGIN
							const Origin& origin=name->origin();
							if(origin.file) {
								row+=NEW String(pool(), origin.file, 0, true); // file column
								char *buf=(char *)malloc(MAX_NUMBER);
								size_t buf_size=snprintf(buf, MAX_NUMBER, "%d", 1+origin.line);
								row+=NEW String(pool(), buf, buf_size, true); // line column
							}
#endif
							stack_trace+=&row;
						}
						frame.store_param(NEW VTable(pool(), &stack_trace));

						// future $response:body=
						//   execute ^unhandled_exception[exception;stack]
						body_string=&execute_method(frame, *method);
					}
				}
			}
			
			if(!body_string) {  // couldn't report an error beautifully?
				// doing that ugly

				// make up result: $origin $source $comment $type $code
				char *buf=(char *)malloc(MAX_STRING);
				size_t printed=0;
				if(const String *problem_source=e.problem_source()) {
#ifndef NO_STRING_ORIGIN
					const Origin& origin=problem_source->origin();
					if(origin.file)
						printed+=snprintf(buf+printed, MAX_STRING-printed, 
							ORIGIN_FILE_LINE_FORMAT": ", 
							origin.file, 1+origin.line
						);
#endif
					printed+=snprintf(buf+printed, MAX_STRING-printed, "'%s' ", 
						problem_source->cstr());
				}
				if(const char *comment=e.comment(true))
					printed+=snprintf(buf+printed, MAX_STRING-printed, "%s", comment);
				if(const char *type=e.type(true)) 
					printed+=snprintf(buf+printed, MAX_STRING-printed, "  type: %s",  type);

				// future $response:content-type
				response.fields().put(*content_type_name, 
					NEW VString(*NEW String(pool(), UNHANDLED_EXCEPTION_CONTENT_TYPE)));
				// future $response:body
				body_string=NEW String(pool(), buf);
			}

			VString body_vstring(*body_string);
			const VFile *body_file=body_vstring.as_vfile();

			// ERROR. write it out
			output_result(*body_file, header_only);
		} catch(const Exception& ) {
			/*re*/throw;
		}
	}
}

VStateless_class *Request::use_file(VStateless_class& aclass,
									const String& file_name, bool ignore_class_path, 
									bool fail_on_read_problem, bool fail_on_file_absence) {
	// cyclic dependence check
	if(used_files.get(file_name))
		return 0;
	used_files.put(file_name, (Hash::Val *)true);

	const String *file_spec;
	if(ignore_class_path) // ignore_class_path?
		file_spec=&file_name;
	else if(file_name.first_char()=='/') //absolute path, no need to scan MAIN:CLASS_PATH?
		file_spec=&absolute(file_name);
	else {
		file_spec=0;
		if(Value *element=main_class.get_element(*class_path_name, main_class, false)) {
			if(element->is_string()) {
				file_spec=file_readable(absolute(element->as_string()), file_name); // found at class_path?
			} else if(Table *table=element->get_table()) {
				int size=table->size();
				for(int i=size; i--; ) {
					const String& path=*static_cast<Array *>(table->get(i))->get_string(0);
					if(file_spec=file_readable(absolute(path), file_name))
						break; // found along class_path
				}
			} else
				throw Exception("parser.runtime",
					0,
					"$" CLASS_PATH_NAME " must be string or table");
			if(!file_spec)
				throw Exception("parser.runtime",
					&file_name,
					"not found along " MAIN_CLASS_NAME ":" CLASS_PATH_NAME);
		}
		if(!file_spec)
			throw Exception("parser.runtime",
				&file_name,
				"usage failed - no $" MAIN_CLASS_NAME  ":" CLASS_PATH_NAME " were specified");
	}

	if(fail_on_read_problem && !fail_on_file_absence) // ignore file absence if asked for
		if(!entry_exists(*file_spec))
			return 0;

	char *source=file_read_text(pool(), *file_spec, fail_on_read_problem);
	if(!source)
		return 0;

	return use_buf(aclass, source, *file_spec, file_spec->cstr());
}


VStateless_class *Request::use_buf(VStateless_class& aclass,
								   const char *source, 
								   const String& filespec, const char *filespec_cstr) {
	// temporary zero @conf so to maybe-replace it in compiled code
	Temp_method temp_method_conf(aclass, *conf_method_name, 0);
	// temporary zero @auto so to maybe-replace it in compiled code
	Temp_method temp_method_auto(aclass, *auto_method_name, 0);

	// compile loaded class
	VStateless_class& cclass=COMPILE(aclass, source, filespec_cstr);

	// locate and execute possible @conf[] static
	VString *vfilespec=NEW VString(filespec);
	const Method *method_called;
	execute_nonvirtual_method(cclass, 
		*conf_method_name, vfilespec,
		0/*no result needed*/, &method_called);
	if(method_called)
		configure_admin(cclass, &method_called->name);

	// locate and execute possible @auto[] static
	execute_nonvirtual_method(cclass, 
		*auto_method_name, vfilespec,
		0/*no result needed*/);
	return &cclass;
}

const String& Request::relative(const char *apath, const String& relative_name) {
	int hpath_buf_size=strlen(apath)+1;
    char *hpath=(char *)malloc(hpath_buf_size);
	memcpy(hpath, apath, hpath_buf_size);
	String& result=*NEW String(pool());
    if(rsplit(hpath, '/')) // if something/splitted
		result << hpath << "/";
    result << relative_name;
    return result;
}

const String& Request::absolute(const String& relative_name) {
	if(relative_name.first_char()=='/') {
		String& result=*NEW String(pool(), info.document_root);
		result << relative_name;
		return result;
	} else 
		return relative_name.pos("://")<0? relative(info.path_translated, relative_name)
			:relative_name; // something like "http://xxx"
}

static void add_header_attribute(const Hash::Key& aattribute, Hash::Val *ameaning, 
								 void *info) {
	Request& r=*static_cast<Request *>(info);
	if(aattribute==BODY_NAME
		|| aattribute==CHARSET_NAME)
		return;

	Value& lmeaning=*static_cast<Value *>(ameaning);
	Pool& pool=lmeaning.pool();

	SAPI::add_header_attribute(pool,
		aattribute.cstr(), 
		attributed_meaning_to_string(lmeaning, String::UL_HTTP_HEADER).cstr(String::UL_UNSPECIFIED));
}
void Request::output_result(const VFile& body_file, bool header_only) {
	// header: cookies
	cookie.output_result();
	
	Value *body_file_content_type;
	// set content-type
	if(body_file_content_type=static_cast<Value *>(
		body_file.fields().get(*content_type_name))) {
		// body file content type
		response.fields().put(*content_type_name, body_file_content_type);
	} else {
		// default content type
		response.fields().put_dont_replace(*content_type_name, 
			default_content_type?default_content_type
			:NEW VString(*NEW String(pool(), DEFAULT_CONTENT_TYPE)));
	}

	// content-disposition
	if(VString *vfile_name=static_cast<VString *>(body_file.fields().get(*name_name)))
		if(vfile_name->string()!=NONAME_DAT) {
			VHash& vhash=*NEW VHash(pool());
			Hash& hash=vhash.hash(0);
			hash.put(*value_name, NEW VString(*content_disposition_value));
			hash.put(*content_disposition_filename_name, vfile_name);
			response.fields().put(*content_disposition_name, &vhash);
		}

	// prepare header: $response:fields without :body
	response.fields().for_each(add_header_attribute, this);

	const void *client_body;
	size_t client_content_length;
	// transcode text body when "text/*" or simple result
	if(body_file_content_type)
		if(Hash *hash=body_file_content_type->get_hash(0))
			body_file_content_type=static_cast<Value *>(hash->get(*value_name));
	if(!body_file_content_type/*vstring.as_vfile*/ || body_file_content_type->as_string().pos("text/")==0) {
		Charset::transcode(pool(),
			pool().get_source_charset(), body_file.value_ptr(), body_file.value_size(),
			pool().get_client_charset(), client_body, client_content_length
		);
	} else {
		client_body=body_file.value_ptr();
		client_content_length=body_file.value_size();
	}

	// prepare header: content-length
	char content_length_cstr[MAX_NUMBER];
	snprintf(content_length_cstr, MAX_NUMBER, "%u", client_content_length);
	SAPI::add_header_attribute(pool(), "content-length", content_length_cstr);

	// send header
	SAPI::send_header(pool());
	
	// send body
	if(!header_only)
		SAPI::send_body(pool(), client_body, client_content_length);
}

const String& Request::mime_type_of(const char *user_file_name_cstr) {
	if(mime_types)
		if(const char *cext=strrchr(user_file_name_cstr, '.')) {
			String sext(pool(), ++cext);
			if(mime_types->locate(0, sext.change_case(pool(), String::CC_LOWER)))
				if(const String *result=mime_types->item(1))
					return *result;
				else
					throw Exception("parser.runtime",
						mime_types->origin_string(),
						MIME_TYPES_NAME  " table column elements must not be empty");
		}
	return *NEW String(pool(), "application/octet-stream");
}

bool Request::origins_mode() {
	return main_class.get_element(*origins_mode_name, main_class, false)!=0;  // $ORIGINS mode
}
