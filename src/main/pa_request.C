/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_request.C,v 1.28 2001/03/13 17:54:14 paf Exp $
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

Request::Request(Pool& apool,
				 String::Untaint_lang alang,
				 char *adocument_root,
				 char *apage_filespec) : Pooled(apool),
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

char *Request::core(bool& error) {
	error=false;
	char *result;
	TRY {
		// loading system auto.p
		char *sys_auto_file="C:\\temp\\auto.p";
		VStateless_class *main_class=use_file(
			sys_auto_file, false/*ignore possible read problem*/,
			main_class_name);

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
		result=execute_method(*main_class, *main_method_name, 
			true /*result needed*/);
		if(!result)
			THROW(0,0,
			0, 
			"'"MAIN_METHOD_NAME"' method not found");
	} 
	CATCH(e) {
		// we're returning not result, but error explanation
		error=true;
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
    rsplit(result,'/');
    strcat(result, "/");
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
