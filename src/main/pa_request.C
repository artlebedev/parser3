/** @file
	Parser: request class main part. @see compile.C and execute.C.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.69 2001/03/23 10:27:31 paf Exp $
*/

#include <string.h>

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_vclass.h"
#include "_root.h"
#include "_table.h"
#include "pa_globals.h"
#include "pa_vint.h"
#include "pa_vmframe.h"
#include "pa_types.h"

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
	fdefault_content_type(0)
{
	// root superclass, 
	//   parent of all classes, 
	//   operators holder
	initialize_root_class(pool(), ROOT);
	classes().put(*root_class_name, &ROOT);
	// table class
	classes().put(*table_class_name, table_class);	

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
			strncpy(auto_filespec, root_auto_path, MAX_STRING-strlen("/" AUTO_FILE_NAME));
			strcat(auto_filespec, "/" AUTO_FILE_NAME);
			main_class=use_file(
				auto_filespec, root_auto_fail,
				main_class_name, main_class);
		}

		// loading site auto.p
		if(site_auto_path) {
			strncpy(auto_filespec, site_auto_path, MAX_STRING-strlen("/" AUTO_FILE_NAME));
			strcat(auto_filespec, "/" AUTO_FILE_NAME);
			main_class=use_file(
				auto_filespec, site_auto_fail,
				main_class_name, main_class);
		}

		Value *element;
		// $MAIN:limits hash used here,
		//	until someone with less privileges have overriden them
		Value *limits=main_class?main_class->get_element(*limits_name):0;
		// $limits.post_max_size default 10M
		element=limits?limits->get_element(*post_max_size_name):0;
		int value=element?(size_t)element->get_double():0;
		int post_max_size=value?value:10*0x400*400;

		form.fill_fields(*this, post_max_size);
		cookie.fill_fields(*this);

		// TODO: load site auto.p files, all assigned bases from upper dir
		/*char *site_auto_file="Y:\\parser3\\src\\auto.p";
		main_class=use_file(
			site_auto_file, false/*ignore possible read problem* /,
			main_class_name, main_class);*/

		// $MAIN:defaults
		Value *defaults=main_class?main_class->get_element(*defaults_name):0;
		fdefault_content_type=defaults?defaults->get_element(*content_type_name):0;

		// compiling requested file
		main_class=use_file(info.path_translated, true/*don't ignore read problem*/,
			main_class_name, main_class);

		// execute @main[]
		const String *body_string=execute_method(*main_class, *main_method_name);
		if(!body_string)
			THROW(0,0,
			0, 
			"'"MAIN_METHOD_NAME"' method not found");

		// extract response body
		Value *body_value=static_cast<Value *>(
			response.fields().get(*body_name));
		if(body_value) // there is some $request.body
			body_string=&body_value->as_string();// TODO: IMAGE&FILE

		// OK. write out the result
		output_result(*body_string, header_only);
	} 
	CATCH(e) {
		TRY {
			// we're returning not result, but error explanation

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
					NEW VString(*NEW String(pool(), "text/plain")));
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

/// @todo find|solve cyclic dependences
VStateless_class *Request::use_file(const char *file, bool fail_on_read_problem,
									const String *name, 
									VStateless_class *base_class) {
	char *source=file_read_text(pool(), file, fail_on_read_problem);
	if(!source)
		return base_class;

	return use_buf(source, file, 0/*new class*/, name, base_class);
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

char *Request::relative(const char *path, const char *file) {
    char *result=(char *)malloc(strlen(path)+strlen(file)+1);
	strcpy(result, path);
    rsplit(result, '/');
    strcat(result, "/");
    strcat(result, file);
    return result;
}

char *Request::absolute(const char *name) {
	if(name[0]=='/') {
		char *result=(char *)malloc(strlen(info.document_root)+strlen(name)+1);
		strcpy(result, info.document_root);
		strcat(result, name);
		return result;
	} else 
		return relative(info.path_translated, name);
}

void Request::output_result(const String& body_string, bool header_only) {
	// header: cookies
	cookie.output_result();
	
	// set default content-type
	if(fdefault_content_type)
		response.fields().put_dont_replace(*content_type_name, fdefault_content_type);

	// prepare header: $response:fields without :body
	response.fields().foreach(add_header_attribute, /*excluding*/ body_name);

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
