/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_request.C,v 1.206 2002/04/29 06:27:29 paf Exp $
*/

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
#include "pa_charsets.h"
#include "pa_charset.h"

/// content type of exception response, when no @MAIN:exception handler defined
const char *UNHANDLED_EXCEPTION_CONTENT_TYPE="text/plain";

/// content type of response when no $MAIN:defaults.content-type defined
const char *DEFAULT_CONTENT_TYPE="text/html";
const char *ORIGINS_CONTENT_TYPE="text/plain";

Methoded *MOP_create(Pool&);
// op.C
VHash& exception2vhash(Pool& pool, const Exception& e);

static void load_charset(const Hash::Key& akey, Hash::Val *avalue, 
										  void *) {
	charsets->load_charset(akey, static_cast<Value *>(avalue)->as_string());
}

//
Request::Request(Pool& apool,
				 Info& ainfo,
				 uchar adefault_lang,
				 bool status_allowed) : Pooled(apool),
	stack(apool),
	OP(*MOP_create(apool)),
	env(apool),
	status(apool),
	form(apool),
	math(apool),
	request(apool, *this),
	response(apool),
	cookie(apool),
	fclasses(apool),
	fdefault_lang(adefault_lang), flang(adefault_lang),
	info(ainfo),
	post_data(0), post_size(0),
	used_files(apool),
	default_content_type(0),
	mime_types(0),
	main_class(0),
	fconnection(0),
	classes_conf(apool),
	anti_endless_execute_recoursion(0),
	exception_trace(apool)
#ifdef RESOURCES_DEBUG
	, sql_connect_time(0),sql_request_time(0)
#endif
	, self(0), root(0), rcontext(0), wcontext(0)
{
	// default charsets
	pool().set_source_charset(*utf8_charset);
	pool().set_client_charset(*utf8_charset);

	// maybe expire old caches
	cache_managers->maybe_expire();
	
	/// directly used
	// operators
	OP.register_directly_used(*this);
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
	classes().put(form.get_class()->base()->name(), &form);	
	// math class
	classes().put(math.get_class()->base()->name(), &math);	
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
				   const char *root_config_filespec, bool root_config_fail_on_read_problem,
				   const char *site_config_filespec, bool site_config_fail_on_read_problem,
				   bool header_only) {

#ifdef RESOURCES_DEBUG
//measures
struct timeval mt[10];
//measure:before all
gettimeofday(&mt[0],NULL);
#endif
	try {
		char *auto_filespec=(char *)malloc(MAX_STRING);
		
		// loading root config
		if(root_config_filespec) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(root_config_filespec, 0, "root_config", 0);
			main_class=use_file(
				filespec, 
				true/*ignore class_path*/, root_config_fail_on_read_problem,
				main_class_name, main_class);
		}
		// charsets must only be specified in root config
		// so that users would not interfere
		if(main_class) {
			/* $MAIN:CHARSETS[
					$.charsetname1[/full/path/to/charset/file.cfg]
					...
				]
			*/
			if(Value *vcharsets=main_class->get_element(*charsets_name)) {
				if(Hash *charsets=vcharsets->get_hash(0))
					charsets->for_each(load_charset);
				else
					throw Exception("parser.runtime",
						0,
						"$" CHARSETS_NAME " must be hash");
			}
		}
		// configure root options
		//	until someone with less privileges have overriden them
		OP.configure_admin(*this);
		methoded_array->configure_admin(*this);

		// loading site config
		if(site_config_filespec) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(site_config_filespec, 0, "site_config", 0);
			main_class=use_file(
				filespec, 
				true/*ignore class_path*/, site_config_fail_on_read_problem,
				main_class_name, main_class);
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

					main_class=use_file(sfile_spec, 
						true/*ignore class_path*/, false/*ignore read problem*/,
						main_class_name, main_class);
				}
				after=before+1;
			}
		}

		// compile requested file
		String& spath_translated=*NEW String(pool());
		spath_translated.APPEND_TAINTED(info.path_translated, 0, "user-request", 0);
		main_class=use_file(spath_translated, 
			true/*ignore class_path*/, true/*don't ignore read problem*/,
			main_class_name, main_class);

		// configure not-root=user options
		OP.configure_user(*this);
		methoded_array->configure_user(*this);

		// $MAIN:MIME-TYPES
		if(Value *element=main_class->get_element(*mime_types_name))
			if(Table *table=element->get_table())
				mime_types=table;			

		// filling form fields
		form.fill_fields_and_tables(*this);

		// filling cookies
		cookie.fill_fields(*this);

#ifdef RESOURCES_DEBUG
//measure:after compile
gettimeofday(&mt[1],NULL);
#endif
		// execute @main[]
		const String *body_string=execute_virtual_method(*main_class, *main_method_name);
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
		if(Value *value=main_class->get_element(*post_process_method_name))
			if(Junction *junction=value->get_junction())
				if(const Method *method=junction->method) {
					// preparing to pass parameters to 
					//	@postprocess[data]
					VMethodFrame frame(pool(), method->name, *junction);
					frame.set_self(*main_class);

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
		if(body_value) // there is some $response.body
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

			if(main_class) { // we've managed to end up with some main_class
				// maybe we'd be lucky enough as to report an error
				// in a gracefull way...
				if(Value *value=main_class->get_element(*unhandled_exception_method_name))
					if(Junction *junction=value->get_junction())
						if(const Method *method=junction->method) {
		 					// preparing to pass parameters to 
							//	@unhandled_exception[exception;stack]
							VMethodFrame frame(pool(), method->name, *junction);
							frame.set_self(*main_class);

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
							//   execute ^exception[origin;source;comment;type;code;stack]
							body_string=&execute_method(frame, *method);
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

VStateless_class *Request::use_file(const String& file_name, 
									bool ignore_class_path, bool fail_on_read_problem,
									const String *name, 
									VStateless_class *base_class) {
	// cyclic dependence check
	if(used_files.get(file_name))
		return base_class;
	used_files.put(file_name, (Hash::Val *)true);

	const String *file_spec;
	if(ignore_class_path) // ignore_class_path?
		file_spec=&file_name;
	else if(file_name.first_char()=='/') //absolute path, no need to scan MAIN:CLASS_PATH?
		file_spec=&absolute(file_name);
	else {
		file_spec=0;
		if(main_class)
			if(Value *element=main_class->get_element(*class_path_name)) {
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

	char *source=file_read_text(pool(), *file_spec, fail_on_read_problem);
	if(!source)
		return base_class;

	return use_buf(source, file_spec->cstr(), 0/*new class*/, name, base_class);
}

VStateless_class *Request::use_buf(const char *source, const char *file,
								   VStateless_class *aclass, const String *name, 
								   VStateless_class *base_class) {
	// compile loaded class
	VStateless_class& cclass=COMPILE(source, aclass, name, base_class, file);

	// locate and execute possible @auto[] static method
	execute_nonvirtual_method(cclass, *auto_method_name, false /*no result needed*/);
	return &cclass;
}

const String& Request::relative(const char *apath, const String& relative_name) {
	int lpath_buf_size=strlen(apath)+1;
    char *lpath=(char *)malloc(lpath_buf_size);
	memcpy(lpath, apath, lpath_buf_size);
    if(!rsplit(lpath, '/'))
		strcpy(lpath, ".");
	String& result=*NEW String(pool(), lpath);
    result << "/" << relative_name;
    return result;
}

const String& Request::absolute(const String& relative_name) {
	char *relative_name_cstr=relative_name.cstr();
	if(relative_name_cstr[0]=='/') {
		String& result=*NEW String(pool(), info.document_root);
		result << relative_name;
		return result;
	} else 
		return relative(info.path_translated, relative_name);
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
		attributed_meaning_to_string(lmeaning, String::UL_HTTP_HEADER).cstr());
}
void Request::output_result(const VFile& body_file, bool header_only) {
	// header: cookies
	cookie.output_result();
	
	// set content-type
	if(String *body_file_content_type=static_cast<String *>(
		body_file.fields().get(*vfile_mime_type_name))) {
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
			vhash.hash(0).put(*content_disposition_filename_name, vfile_name);
			response.fields().put(*content_disposition_name, &vhash);
		}

	// prepare header: $response:fields without :body
	response.fields().for_each(add_header_attribute, this);

	// transcode
	const void *client_body;
	size_t client_content_length;
	Charset::transcode(pool(),
		pool().get_source_charset(), body_file.value_ptr(), body_file.value_size(),
		pool().get_client_charset(), client_body, client_content_length
	);

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
			if(mime_types->locate(0, sext))
				if(const String *result=mime_types->item(1))
					return *result;
				else
					throw Exception("parser.runtime",
						mime_types->origin_string(),
						"MIME-TYPE table column elements must not be empty");
		}
	return *NEW String(pool(), "application/octet-stream");
}
