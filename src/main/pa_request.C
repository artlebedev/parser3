/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.41 2001/03/18 11:37:52 paf Exp $
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

#define NEW_STRING(name, value)  name=NEW String(pool()); name->APPEND_CONST(value)
#define LOCAL_STRING(name, value)  String name(pool()); name.APPEND_CONST(value)

Request::Request(Pool& apool,
				 Info& ainfo,
				 String::Untaint_lang alang) : Pooled(apool),
	stack(apool),
	ROOT(apool),
	env(apool),
	form(apool),
	request(apool, *this),
	response(apool),
	fclasses(apool),
	flang(alang),
	info(ainfo)
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

		// $MAIN:limits hash used here,
		//	until someone with less privileges have overriden them
		Value *limits=main_class?main_class->get_element(*limits_name):0;
		Value *element;
		// $limits.post_max_size default 10M
		element=limits?limits->get_element(*post_max_size_name):0;
		int value=element?(size_t)element->get_double():0;
		int post_max_size=value?value:10*0x400*400;

		//response.fields().put(*content_type_name, TODO);

		form.fill_fields(*this, post_max_size);

		// TODO: load site auto.p files, all assigned bases from upper dir
		/*char *site_auto_file="Y:\\parser3\\src\\auto.p";
		main_class=use_file(
			site_auto_file, false/*ignore possible read problem* /,
			main_class_name, main_class);*/

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

		// store 'body' unless response already have one
		response.fields().put_dont_replace(*body_name, NEW VString(*body_string));
	} 
	CATCH(e) {
		TRY {
			// we're returning not result, but error explanation
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
							LOCAL_STRING(origin_name, "origin");
							Value *origin_value=0;
#ifndef NO_STRING_ORIGIN
							if(problem_source) {
								const Origin& origin=problem_source->origin();
								if(origin.file) {
									char *buf=(char *)malloc(MAX_STRING);
									snprintf(buf, MAX_STRING, "%s(%d): ", 
										origin.file, 1+origin.line);
									String *NEW_STRING(origin_file_line, buf);
									origin_value=NEW VString(*origin_file_line);
								}
							}
#endif
							frame.store_param(origin_name, 
								origin_value?origin_value:NEW VUnknown(pool()));

							// source
							LOCAL_STRING(source_name, "source");
							Value *source_value=0;
							if(problem_source)
								source_value=NEW VString(*problem_source);
							frame.store_param(source_name, 
								source_value?source_value:NEW VUnknown(pool()));

							// comment
							LOCAL_STRING(comment_name, "comment");
							String *NEW_STRING(comment_value, e.comment());
							frame.store_param(comment_name, 
								NEW VString(*comment_value));

							// type
							LOCAL_STRING(type_name, "type");
							Value *type_value;
							if(e.type())
								type_value=NEW VString(*e.type());
							else
								type_value=NEW VUnknown(pool());
							frame.store_param(type_name, type_value);

							// code
							LOCAL_STRING(code_name, "code");
							Value *code_value;
							if(e.code())
								code_value=NEW VString(*e.code());
							else
								code_value=NEW VUnknown(pool());
							frame.store_param(code_name, code_value);

							{
								Temp_lang temp_lang(*this, 
									String::Untaint_lang::HTML_TYPO);
								body_string=execute_method(frame, *method);
							}
						}
			}
			
			if(!body_string) {  // couldn't report an error beautifully?
				// doing that ugly

				// assign content-type
				String &content_type_value=*NEW String(pool());
				content_type_value.APPEND_CONST("text/plain");
				response.fields().put(*content_type_name, 
					NEW VString(content_type_value));

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
				String *error_string=NEW String(pool()); error_string->APPEND_CONST(buf);
				body_string=error_string;
			}

			// store 'body' overwriting any response already had
			response.fields().put(*body_name, NEW VString(*body_string));
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
