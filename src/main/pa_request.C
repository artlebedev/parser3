/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.48 2001/03/18 17:24:55 paf Exp $
*/

#include <string.h>

#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_common.h"
#include "pa_vclass.h"
#include "_root.h"
#include "_env.h"
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
//	table_class->set_name(*table_class_name);

	// env class
	classes().put(*env_class_name, &env);
	// form class
	classes().put(*form_class_name, &form);	
	// request class
	classes().put(*request_class_name, &request);	
	// response class
	classes().put(*response_class_name, &response);	
}

static void append_attribute_subattribute(const Hash::Key& akey, Hash::Value *avalue, 
										  void *info) {
	if(akey==VALUE_NAME)
		return;

	// ...; charset=windows1251
	String *string=static_cast<String *>(info);
	string->APPEND_CONST("; ");
	
	string->append(akey, String::Untaint_lang::URI, true);

	string->APPEND_CONST("=");

	Value *value=static_cast<Value *>(avalue);
	string->append(value->as_string(), String::Untaint_lang::URI, true);
}
static void output_response_attribute(const Hash::Key& aattribute, Hash::Value *ameaning, 
									  void *info) {
	String *attribute_to_exclude=static_cast<String *>(info);
	if(aattribute==*attribute_to_exclude)
		return;

	String attribute(aattribute.pool());
	attribute.append(aattribute, String::Untaint_lang::URI, true);

	Value *meaning=static_cast<Value *>(ameaning);
	String string(meaning->pool());
	if(VHash *vhash=meaning->get_hash()) {
		// $value(value) $subattribute(subattribute value)
		Hash& hash=vhash->hash();

		Value *value=static_cast<Value *>(hash.get(*value_name));
		if(value)
			string.append(value->as_string(), String::Untaint_lang::URI, true);

		hash.foreach(append_attribute_subattribute, &string);
	} else // string value
		string.append(meaning->as_string(), String::Untaint_lang::URI, true);

	(*service_funcs.output_header_attribute)(
		attribute.cstr(), 
		string.cstr());
}

void Request::core(Exception& system_exception,
				   const char *sys_auto_path1,
				   const char *sys_auto_path2) {
	VStateless_class *main_class=0;
	bool need_rethrow=false;  Exception rethrow_me;
	TRY {
		char *auto_filespec=(char *)malloc(MAX_STRING);
		
		// load MAIN class,
		//	it consists of all the auto.p files we'd manage to find
		//	plus
		//	the file user requested us to process
		//	all located classes become children of one another,
		//	composing class we name 'MAIN'

		// loading system auto.p 1
		if(sys_auto_path1) {
			strncpy(auto_filespec, sys_auto_path1, MAX_STRING-strlen(AUTO_FILE_NAME));
			strcat(auto_filespec, AUTO_FILE_NAME);
			main_class=use_file(
				auto_filespec, false/*ignore possible read problem*/,
				main_class_name, main_class);
		}

		// loading system auto.p 2
		if(sys_auto_path2) {
			strncpy(auto_filespec, sys_auto_path2, MAX_STRING-strlen(AUTO_FILE_NAME));
			strcat(auto_filespec, AUTO_FILE_NAME);
			main_class=use_file(
				auto_filespec, false/*ignore possible read problem*/,
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

		// TODO: load site auto.p files, all assigned bases from upper dir
		/*char *site_auto_file="Y:\\parser3\\src\\auto.p";
		main_class=use_file(
			site_auto_file, false/*ignore possible read problem* /,
			main_class_name, main_class);*/

		// $MAIN:defaults
		Value *defaults=main_class?main_class->get_element(*defaults_name):0;
		// $defaults.content-type
		if(fdefault_content_type=defaults?defaults->get_element(*content_type_name):0)
			response.fields().put(*content_type_name, fdefault_content_type);

		// there must be some auto.p
		if(!main_class)
			THROW(0,0,
				0,
				"no 'auto.p' found (nither system nor any site's)");

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
		output_result(*body_string);
	} 
	CATCH(e) {
		TRY {
			// we're returning not result, but error explanation

			// reset language to default
			flang=fdefault_lang;
			
			// reset response
			response.fields().clear();

			// this is what we'd return in $response:content-type
			Value *content_type;

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

							// future $response:content-type=
							//   content-type from any auto.p
							content_type=fdefault_content_type;
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
				String &content_type_value=*NEW String(pool(), "text/plain");
				content_type=NEW VString(content_type_value);
				// future $response:body
				body_string=NEW String(pool(), buf);
			}

			// set $response:content-type
			if(content_type)
				response.fields().put(*content_type_name, content_type);

			// ERROR. write it out
			output_result(*body_string);
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
		system_exception._throw(rethrow_me.type(), rethrow_me.code(),
			rethrow_me.problem_source(),
			rethrow_me.comment());

}

VStateless_class *Request::use_file(
									const char *file, bool fail_on_read_problem,
									const String *name, 
									VStateless_class *base_class) {
	// TODO: обнаружить|решить cyclic dependences
	char *source=file_read(pool(), file, fail_on_read_problem);
	if(!source)
		return base_class;

	return use_buf(source, file, 0/*new class*/, name, base_class);
}

VStateless_class *Request::use_buf(
								   const char *source, const char *file,
								   VStateless_class *aclass, const String *name, 
								   VStateless_class *base_class) {
	// compile loaded class
	VStateless_class& cclass=COMPILE(source, aclass, name, base_class, file);

	// locate and execute possible @auto[] static method
	execute_method(cclass, *auto_method_name, false /*no result needed*/);
	return &cclass;
}

void Request::fail_if_junction_(bool is, 
								Value& value, const String& method_name, char *msg) {

	// fail_if_junction(true, junction = fail
	// fail_if_junction(false, not junction = fail
	if((value.get_junction()!=0) ^ !is)
		THROW(0, 0,
			&method_name,
			msg);
}

char *Request::relative(const char *path, const char *file) {
    char *result=(char *)malloc(strlen(path)+strlen(file)+1);
	strcpy(result, path);
    rsplit(result, PATH_DELIMITER_CHAR);
    strcat(result, PATH_DELIMITER_STRING);
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
		return relative(info.uri, name);
}

void Request::output_result(const String& body_string) {
	// header: response fields 
	response.fields().foreach(output_response_attribute, /*excluding*/ body_name);
	const char *body=body_string.cstr();
	if(size_t content_length=strlen(body)) {
		// header: content-length
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%d", content_length);
		(*service_funcs.output_header_attribute)("content-length", 
			content_length_cstr);
		// body
		(*service_funcs.output_body)(body, content_length);
	}
}