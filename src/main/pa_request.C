/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.32 2001/03/14 09:02:52 paf Exp $
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
				 String::Untaint_lang alang,
				 const char *adocument_root,
				 const char *apage_filespec) : Pooled(apool),
	stack(apool),
	root_class(apool),
	env_class(apool),
	form_class(apool),
	fclasses(apool),
	flang(alang),
	fdocument_root(adocument_root),
	fpage_filespec(apage_filespec)
{
	// root superclass, 
	//   parent of all classes, 
	//   operators holder
	initialize_root_class(pool(), root_class);
	classes().put(*root_class_name, &root_class);
	// table class
	classes().put(*table_class_name, table_class);	
//	table_class->set_name(*table_class_name);

	// env class
	classes().put(*env_class_name, &env_class);
	// form class
	classes().put(*form_class_name, &form_class);	
}

char *Request::core(const char *sys_auto_path1,
					const char *sys_auto_path2) {
	char *result;
	VStateless_class *main_class=0;
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
				main_class_name);
		}

		// loading system auto.p 2
		if(sys_auto_path2) {
			strncpy(auto_filespec, sys_auto_path2, MAX_STRING-strlen(AUTO_FILE_NAME));
			strcat(auto_filespec, AUTO_FILE_NAME);
			VStateless_class *main_class=use_file(
				auto_filespec, false/*ignore possible read problem*/,
				main_class_name);
		}

		// TODO: использовать $MAIN:limits здесь, пока их не сломали враги

		// TODO: load site auto.p files, all assigned bases from upper dir
		char *site_auto_file="Y:\\parser3\\src\\auto.p";
		main_class=use_file(
			site_auto_file, false/*ignore possible read problem*/,
			main_class_name, main_class);

		// there must be some auto.p
		if(!main_class)
			THROW(0,0,
				0,
				"no 'auto.p' found (nither system nor any site's)");

		// compiling requested file
		main_class=use_file(fpage_filespec, true/*don't ignore read problem*/,
			main_class_name, main_class);

		// execute @main[]
		result=execute_method(*main_class, *main_method_name);
		if(!result)
			THROW(0,0,
			0, 
			"'"MAIN_METHOD_NAME"' method not found");
	} 
	CATCH(e) {
		TRY {
			// we're returning not result, but error explanation
			result=0;
			
			if(main_class) { // we've managed to end up with some main_class
				// maybe we'd be lucky enough as to report an error
				// in a gracefull way...
				if(Value *value=main_class->get_element(*exception_method_name))
					if(Junction *junction=value->get_junction())
						if(const Method *method=junction->method) {
							// preparing to pass parameters to 
							//	@exception[origin;source;comment;type;code]
							VMethodFrame *frame=NEW VMethodFrame(pool(), *junction);

							const String *problem_source=e.problem_source();
							// origin
							LOCAL_STRING(origin_name, "origin");
							VString *origin_value=0;
#ifndef NO_STRING_ORIGIN
							const Origin& origin=problem_source->origin();
							if(origin.file) {
								char *buf=(char *)malloc(MAX_STRING);
								snprintf(buf, MAX_STRING, "%s(%d): ", 
									origin.file, 1+origin.line);
								String *NEW_STRING(origin_file_line, buf);
								origin_value=NEW VString(*origin_file_line);
							}
#endif
							frame->store_param(origin_name, origin_value);

							// source
							LOCAL_STRING(source_name, "source");
							frame->store_param(source_name, 
								NEW VString(*problem_source));

							// comment
							LOCAL_STRING(comment_name, "comment");
							String *NEW_STRING(comment_value, e.comment());
							frame->store_param(comment_name, 
								NEW VString(*comment_value));

							// type
							LOCAL_STRING(type_name, "type");
							Value *type_value;
							if(e.type())
								type_value=NEW VString(*e.type());
							else
								type_value=NEW VUnknown(pool());
							frame->store_param(type_name, type_value);

							// code
							LOCAL_STRING(code_name, "code");
							Value *code_value;
							if(e.code())
								code_value=NEW VString(*e.code());
							else
								code_value=NEW VUnknown(pool());
							frame->store_param(code_name, code_value);

							result=execute_method(*frame, *method);
						}
			}
			
			// couldn't report an error beautifully, doing that ugly
			if(!result) {
				result=(char *)malloc(MAX_STRING);
				result[0]=0;
				size_t printed=0;
				const String *problem_source=e.problem_source();
				if(problem_source) {
#ifndef NO_STRING_ORIGIN
					const Origin& origin=problem_source->origin();
					if(origin.file)
						printed+=snprintf(result+printed, MAX_STRING-printed, "%s(%d): ", 
						origin.file, 1+origin.line);
#endif
					printed+=snprintf(result+printed, MAX_STRING-printed, "'%s' ", 
						problem_source->cstr());
				}
				printed+=snprintf(result+printed, MAX_STRING-printed, "%s", 
					e.comment());
				const String *type=e.type();
				if(type) {
					printed+=snprintf(result+printed, MAX_STRING-printed, "  type: %s", 
						type->cstr());
					const String *code=e.code();
					if(code)
						printed+=snprintf(result+printed, MAX_STRING-printed, ", code: %s", 
						code->cstr());
				}
			}
		}
		CATCH(e) {
			// exception in exception handler occured
			// probably totally out of memory
			// can't say anything about such sad story
			// would just return 0
			result=0;
		}
		END_CATCH
	}
	END_CATCH

	return result;
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
		char *result=(char *)malloc(strlen(fdocument_root)+strlen(name)+1);
		strcpy(result, fdocument_root);
		strcat(result, name);
		return result;
	} else 
		return relative(fpage_filespec, name);
}
