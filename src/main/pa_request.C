/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.20 2001/03/12 17:16:50 paf Exp $
*/

#include <string.h>

#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_common.h"
#include "pa_vclass.h"
#include "_root.h"
#include "_env.h"
#include "_table.h"
#include "core.h"

#include <stdio.h>

Request::Request(Pool& apool) : Pooled(apool),
	stack(apool),
	root_class(apool),
	env_class(apool),
	fclasses(apool),
	fclasses_array(apool),
	flang(String::Untaint_lang::HTML_TYPO)
{
	// root class
	initialize_root_class(pool(), root_class);
	// adding root superclass, 
	//   parent of all classes, 
	//   operators holder
	classes().put(*root_class_name, &root_class);
	//root_class.set_name(*root_class_name);

	// env class
	initialize_env_class(pool(), env_class);
	classes().put(*env_class_name, &env_class);

	// table class
	classes().put(*table_class_name, table_class);	
	table_class->set_name(*table_class_name);


	// web
	// todo: ifdef WIN32 flip \\ to /
	document_root="Y:/parser3/src/";
	page_filespec="Y:/parser3/src/test.p";
}

void Request::core() {
	TRY {
		// loading system auto.p
		char *sys_auto_file="C:\\temp\\auto.p";
		VClass *main_class=use_file(
			sys_auto_file, false/*ignore possible read problem*/,
			0/*new class*/, main_class_name);

		// TODO: использовать $MAIN:limits здесь, пока их не сломали враги

		// TODO: load site auto.p files, all assigned bases from upper dir
		char *site_auto_file="Y:\\parser3\\src\\auto.p";
		main_class=use_file(
			site_auto_file, false/*ignore possible read problem*/,
			0/*new class*/, main_class_name, main_class);

		// there must be some auto.p
		if(!main_class)
			THROW(0,0,
				0,
				"'auto.p' not found");

		// compiling requested file
		use_file(page_filespec, true/*don't ignore read problem*/,
			0/*new class*/, main_class_name, main_class);

		// executing some @main[]
		char *result=execute_MAIN();
		printf("result-----------------\n%sEOF----------------\n", result);
	} 
	CATCH(e) {
		printf("\nERROR: ");
		const String *problem_source=e.problem_source();
#ifndef NO_STRING_ORIGIN
		if(problem_source) {
			const Origin& origin=problem_source->origin();
			if(origin.file)
				printf("%s(%d): ",
				origin.file, 1+origin.line);
			printf("'%s' ", 
				problem_source->cstr());
		}
#endif
		printf("%s", e.comment());
		const String *type=e.type();
		if(type) {
			printf("  type: %s", type->cstr());
			const String *code=e.code();
			if(code)
				printf(", code: %s", code->cstr());
		}
		printf("\n");
	}
	END_CATCH
}

VClass *Request::use_file(
						  const char *file, bool fail_on_read_problem,
						  VClass *aclass, const String *name, 
						  VClass *base_class) {
	// TODO: обнаружить|решить cyclic dependences
	char *source=file_read(pool(), file, fail_on_read_problem);
	if(!source)
		return base_class;

	return use_buf(source, file, aclass, name, base_class);
}

VClass *Request::use_buf(
						  const char *source, const char *file,
						  VClass *aclass, const String *name, 
						  VClass *base_class) {
	// compile loaded class
	VClass& cclass=COMPILE(source, aclass, name, base_class, file);

	// locate and execute possible @auto[] static method
	execute_static_method(cclass, *auto_method_name, false /*no result needed*/);
	return &cclass;
}

char *Request::execute_MAIN() {
	// locate class with @main[] & execute it
	for(int i=classes_array().size(); --i>=0;) {
		VClass *vclass=static_cast<VClass *>(classes_array().get(i));
		char *result=execute_static_method(*vclass, *main_method_name, true /*result needed*/);
		if(result)
			return result;
	}
	
	THROW(0,0,
		0, 
		"'"MAIN_METHOD_NAME"' method not found");
	return 0;
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
    char *result=static_cast<char *>(malloc(strlen(path)+strlen(file)+1));
	strcpy(result, path);
    rsplit(result,'/');
    strcat(result, "/");
    strcat(result, file);
    return result;
}

char *Request::absolute(const char *name) {
	if(name[0]=='/') {
		char *result=static_cast<char *>(malloc(strlen(document_root)+strlen(name)+1));
		strcpy(result, document_root);
		strcat(result, name);
		return result;
	} else 
		return relative(page_filespec, name);
}
