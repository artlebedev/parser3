/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.88 2001/03/29 15:00:22 paf Exp $
*/

#include "pa_config_includes.h"

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_vclass.h"
#include "_root.h"
#include "_table.h"
#include "_file.h"
#include "pa_globals.h"
#include "pa_vint.h"
#include "pa_vmframe.h"
#include "pa_types.h"
#include "pa_vtable.h"

/// $limits.post_max_size default 10M
const size_t MAX_POST_SIZE_DEFAULT=10*0x400*400;

/// content type of exception response, when no @MAIN:exception handler defined
const char *UNHANDLED_EXCEPTION_CONTENT_TYPE="text/plain";

/// content type of response when no $MAIN:defaults.content-type defined
const char *DEFAULT_CONTENT_TYPE="text/html";

//
Request::Request(Pool& apool,
				 Info& ainfo,
				 String::Untaint_lang adefault_lang) : Pooled(apool),
	stack(apool),
	ROOT(apool),
	env(apool),
	form(apool),
	request(apool, *this),
	response(apool),
	cookie(apool),
	fclasses(apool),
	fdefault_lang(adefault_lang), flang(adefault_lang),
	info(ainfo),
	used_files(apool),
	default_content_type(0),
	mime_types(0)
{
	// root superclass, 
	//   parent of all classes, 
	//   operators holder
	initialize_root_class(pool(), ROOT);
	classes().put(*root_class_name, &ROOT);
	// table class
	classes().put(*table_class_name, table_class);	
	// file class
	classes().put(*file_class_name, file_class);	
	// env class
	classes().put(*env_class_name, &env);
	// form class
	classes().put(*form_class_name, &form);	
	// request class
	classes().put(*request_class_name, &request);	
	// response class
	classes().put(*response_class_name, &response);	
	// cookie class
	classes().put(*cookie_class_name, &cookie);
}

static void add_header_attribute(const Hash::Key& aattribute, Hash::Val *ameaning, 
								 void *info) {
	String *attribute_to_exclude=static_cast<String *>(info);
	if(aattribute==*attribute_to_exclude)
		return;

	Value& lmeaning=*static_cast<Value *>(ameaning);
	Pool& pool=lmeaning.pool();

	String attribute(pool);
	SAPI::add_header_attribute(pool,
		attribute.append(aattribute, String::UL_HEADER, true).cstr(), 
		attributed_meaning_to_string(lmeaning).cstr());
}

/**
	load MAIN class, execute @main.
	MAIN class consists of all the auto.p files we'd manage to find
	plus
	the file user requested us to process
	all located classes become children of one another,
	composing class we name 'MAIN'
*/
void Request::core(const char *root_auto_path, bool root_auto_fail,
				   const char *site_auto_path, bool site_auto_fail,
				   bool header_only) {
	VStateless_class *main_class=0;
	bool need_rethrow=false;  Exception rethrow_me;
	TRY {
		char *auto_filespec=(char *)malloc(MAX_STRING);
		
		// loading root auto.p 
		if(root_auto_path) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(root_auto_path, 0, "root_auto", 0);
			filespec.APPEND_CONST("/" AUTO_FILE_NAME);
			main_class=use_file(
				filespec, root_auto_fail,
				main_class_name, main_class);
		}

		// $MAIN:LIMITS hash used here,
		//	until someone with less privileges have overriden them
		{
			Value *limits=main_class?main_class->get_element(*limits_name):0;
			// $limits.post_max_size default 10M
			Value *element=limits?limits->get_element(*post_max_size_name):0;
			size_t value=element?(size_t)element->as_double():0;
			size_t post_max_size=value?value:MAX_POST_SIZE_DEFAULT;
			
			form.fill_fields(*this, post_max_size);
		}

		// filling cookies
		cookie.fill_fields(*this);

		// loading site auto.p
		if(site_auto_path) {
			String& filespec=*NEW String(pool());
			filespec.APPEND_CLEAN(site_auto_path, 0, "site_auto", 0);
			filespec.APPEND_CONST("/" AUTO_FILE_NAME);
			main_class=use_file(
				filespec, site_auto_fail,
				main_class_name, main_class);
		}

		// loading auto.p files from document_root/.. 
		// to the one beside requested file.
		// all assigned bases from upper dir
		{
			/* /document/root */
			char *file_spec=
				(char *)malloc(strlen(info.path_translated)+strlen(AUTO_FILE_NAME)+1);
			size_t document_root_size=strlen(info.document_root);
			if(info.document_root[document_root_size-1]=='/')
				document_root_size--;
			memcpy(file_spec, info.document_root, document_root_size);

			/* /requested/file.html */
			char *branches=(char *)malloc(strlen(info.path_translated)+1);
			strcpy(branches, info.path_translated+document_root_size);
			char *next=branches;
			char *append_here=file_spec+document_root_size;

			size_t slash_auto_p_size=strlen("/" AUTO_FILE_NAME);
			while(true) {
				char *step=lsplit(&next, '/');
				if(next) { // not 'file.html' part
					size_t step_size=strlen(step);
					memcpy(append_here, step, step_size);
					append_here+=step_size;
					memcpy(append_here, "/" AUTO_FILE_NAME, slash_auto_p_size+1);
					append_here++/* / */;

					String& sfile_spec=*NEW String(pool());
					sfile_spec.APPEND_CLEAN(file_spec, 0, "scanned", 0);
					main_class=use_file(sfile_spec, false/*ignore read problem*/,
						main_class_name, main_class);
				} else
					break;
			}
		}

		// compiling requested file
		String& spath_translated=*NEW String(pool());
		spath_translated.APPEND_CLEAN(info.path_translated, 0, "user-request", 0);
		main_class=use_file(spath_translated, true/*don't ignore read problem*/,
			main_class_name, main_class);

		// $MAIN:DEFAULTS
		Value *defaults=main_class->get_element(*defaults_name);
		// value must be allocated on request's pool for that pool used on
		// meaning constructing @see attributed_meaning_to_string
		default_content_type=defaults?defaults->get_element(*content_type_name):0;
		if(Value *element=main_class->get_element(*html_typo_name))
			if(Table *table=element->get_table())
				pool().set_tag(table);

		// $MAIN:MIME-TYPES
		if(Value *element=main_class->get_element(*mime_types_name))
			if(Table *table=element->get_table())
				mime_types=table;			

		// execute @main[]
		const String *body_string=execute_method(*main_class, *main_method_name);
		if(!body_string)
			THROW(0,0,
			0, 
			"'"MAIN_METHOD_NAME"' method not found");

		// post-process
		// todo

		// extract response body
		Value *body_value=static_cast<Value *>(
			response.fields().get(*body_name));
		if(body_value) // there is some $request.body
			body_string=&body_value->as_string();// TODO: IMAGE&FILE

		// OK. write out the result
		output_result(*body_string, header_only);
	} 
	CATCH(e) { // request handling problem
		// we're returning not result, but error explanation
		TRY {
			// log the beast
			const String *problem_source=e.problem_source();
			if(problem_source)
				SAPI::log(pool(),
#ifndef NO_STRING_ORIGIN
					"%s(%d): "
#endif
					"'%s' %s [%s %s]",
#ifndef NO_STRING_ORIGIN
					problem_source->origin().file?problem_source->origin().file:"global",
					problem_source->origin().line,
#endif
					problem_source->cstr(),
					e.comment(),
					e.type()?e.type()->cstr():"-",
					e.code()?e.code()->cstr():"-"
				);
			else
				SAPI::log(pool(),
					"%s [%s %s]",
					e.comment(),
					e.type()?e.type()->cstr():"-",
					e.code()?e.code()->cstr():"-"
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
				if(Value *value=main_class->get_element(*exception_method_name))
					if(Junction *junction=value->get_junction())
						if(const Method *method=junction->method) {
							// preparing to pass parameters to 
							//	@exception[origin;source;comment;type;code]
							VMethodFrame frame(pool(), *junction);
							frame.set_self(*main_class);

							const String *problem_source=e.problem_source();
							// origin
							String origin_name(pool(), "origin");
							Value *origin_value=0;
#ifndef NO_STRING_ORIGIN
							if(problem_source) {
								const Origin& origin=problem_source->origin();
								if(origin.file) {
									char *buf=(char *)malloc(MAX_STRING);
									snprintf(buf, MAX_STRING, "%s(%d):", 
										origin.file, 1+origin.line);
									String *origin_file_line=NEW String(pool(),
										buf, true);
									origin_value=NEW VString(*origin_file_line);
								}
							}
#endif
							frame.store_param(origin_name, 
								origin_value?origin_value:NEW VUnknown(pool()));

							// source
							String source_name(pool(), "source");
							Value *source_value=0;
							if(problem_source) {
								String& problem_source_copy=*NEW String(pool());
								problem_source_copy.append(*problem_source, 
									flang, true);
								source_value=NEW VString(problem_source_copy);
							}
							frame.store_param(source_name, 
								source_value?source_value:NEW VUnknown(pool()));

							// comment
							String comment_name(pool(), "comment");
							String *comment_value=NEW String(pool(),
								e.comment(), true);
							frame.store_param(comment_name, 
								NEW VString(*comment_value));

							// type
							String type_name(pool(), "type");
							Value *type_value;
							if(e.type()) {
								String& type_copy=*NEW String(pool());
								type_value=NEW VString(type_copy.append(*e.type(), 
									flang, true));
							} else
								type_value=NEW VUnknown(pool());
							frame.store_param(type_name, type_value);

							// code
							String code_name(pool(), "code");
							Value *code_value;
							if(e.code()) {
								String& code_copy=*NEW String(pool());
								code_value=NEW VString(code_copy.append(*e.code(), 
									flang, true));
							} else
								code_value=NEW VUnknown(pool());
							frame.store_param(code_name, code_value);

							// future $response:body=
							//   execute ^exception[origin;source;comment;type;code]
							body_string=execute_method(frame, *method);
						}
			}
			
			if(!body_string) {  // couldn't report an error beautifully?
				// doing that ugly

				// make up result: $origin $source $comment $type $code
				char *buf=(char *)malloc(MAX_STRING);
				size_t printed=0;
				const String *problem_source=e.problem_source();
				if(problem_source) {
#ifndef NO_STRING_ORIGIN
					const Origin& origin=problem_source->origin();
					if(origin.file)
						printed+=snprintf(buf+printed, MAX_STRING-printed, "%s(%d): ", 
						origin.file, 1+origin.line);
#endif
					printed+=snprintf(buf+printed, MAX_STRING-printed, "'%s' ", 
						problem_source->cstr());
				}
				printed+=snprintf(buf+printed, MAX_STRING-printed, "%s", 
					e.comment());
				const String *type=e.type();
				if(type) {
					printed+=snprintf(buf+printed, MAX_STRING-printed, "  type: %s", 
						type->cstr());
					const String *code=e.code();
					if(code)
						printed+=snprintf(buf+printed, MAX_STRING-printed, ", code: %s", 
						code->cstr());
				}

				// future $response:content-type
				response.fields().put(*content_type_name, 
					NEW VString(*NEW String(pool(), UNHANDLED_EXCEPTION_CONTENT_TYPE)));
				// future $response:body
				body_string=NEW String(pool(), buf);
			}

			// ERROR. write it out
			output_result(*body_string, header_only);
		}
		CATCH(e) {
			// exception in request exception handler
			// remember to rethrow it
			rethrow_me=e;  need_rethrow=true; 
		}
		END_CATCH
	}
	END_CATCH // do not use pool() after this point - no exception handler set
	          // any throw() would try to use zero exception() pointer 

	if(need_rethrow) // there were an exception set for us to rethrow?
		THROW(rethrow_me.type(), rethrow_me.code(),
			rethrow_me.problem_source(),
			rethrow_me.comment());

}

VStateless_class *Request::use_file(const String& file_spec, bool fail_on_read_problem,
									const String *name, 
									VStateless_class *base_class) {
	// cyclic dependence check
	if(used_files.get(file_spec))
		return base_class;
	used_files.put(file_spec, (Hash::Val *)true);


	char *source=file_read_text(pool(), file_spec, fail_on_read_problem);
	if(!source)
		return base_class;

	return use_buf(source, file_spec.cstr(), 0/*new class*/, name, base_class);
}

VStateless_class *Request::use_buf(const char *source, const char *file,
								   VStateless_class *aclass, const String *name, 
								   VStateless_class *base_class) {
	// compile loaded class
	VStateless_class& cclass=COMPILE(source, aclass, name, base_class, file);

	// locate and execute possible @auto[] static method
	execute_method(cclass, *auto_method_name, false /*no result needed*/);
	return &cclass;
}

/**
	- fail_if_junction(true, junction = fail
	- fail_if_junction(false, not junction = fail
*/
void Request::fail_if_junction_(bool is, Value& value, 
								const String& method_name, const char *msg) {

	if((value.get_junction()!=0) ^ !is)
		THROW(0, 0,
			&method_name,
			msg);
}

const String& Request::relative(const char *apath, const String& relative_name) {
	int lpath_buf_size=strlen(apath)+1;
    char *lpath=(char *)malloc(lpath_buf_size);
	memcpy(lpath, apath, lpath_buf_size);
    rsplit(lpath, '/');
	String& result=*NEW String(pool(), lpath);
    result.APPEND_CONST("/");
	result.append(relative_name, String::UL_PASS_APPENDED);
    return result;
}

const String& Request::absolute(const String& relative_name) {
	char *relative_name_cstr=relative_name.cstr();
	if(relative_name_cstr[0]=='/') {
		String& result=*NEW String(pool(), info.document_root);
		result.append(relative_name, String::UL_PASS_APPENDED);
		return result;
	} else 
		return relative(info.path_translated, relative_name);
}

void Request::output_result(const String& body_string, bool header_only) {
	// header: cookies
	cookie.output_result();
	
	// set default content-type
	response.fields().put_dont_replace(*content_type_name, 
		default_content_type?default_content_type
		:NEW VString(*NEW String(pool(), DEFAULT_CONTENT_TYPE)));

	// prepare header: $response:fields without :body
	response.fields().for_each(add_header_attribute, /*excluding*/ body_name);

	// prepare...
	const char *body=body_string.cstr();
	size_t content_length=strlen(body);

	// prepare header: content-length
	if(content_length) { // useful for redirecting [header "location: http://..."]
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%lu", content_length);
		SAPI::add_header_attribute(pool(), "content-length", content_length_cstr);
	}

	// send header
	SAPI::send_header(pool());
	
	// send body
	if(!header_only)
		SAPI::send_body(pool(), body, content_length);
}
