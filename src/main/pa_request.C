/*
$Id: pa_request.C,v 1.8 2001/03/10 16:21:49 paf Exp $
*/

#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_common.h"
#include "pa_vclass.h"
#include "classes/_root.h"
#include "classes/_env.h"
#include "core.h"

#include <stdio.h>

Request::Request(Pool& apool) : Pooled(apool),
	stack(apool),
	root_class(apool),
	env_class(apool),
	fclasses(apool),
	fclasses_array(apool),
	lang(String::Untaint_lang::HTML_TYPO)
{
	// root class
	initialize_root_class(pool(), root_class);
	// adding root superclass, 
	//   parent of all classes, 
	//   operators holder
	classes().put(*root_class_name, &root_class);

	// env class
	initialize_env_class(pool(), env_class);
	classes().put(*env_class_name, &env_class);
}

void Request::core() {
	TRY {
		// loading system auto.p
		char *sys_auto_file="C:\\temp\\auto.p";
		VClass *sys_auto_class=use(
			sys_auto_file, 
			auto_class_name, 0, 
			false/*ignore possible error*/);

		// TODO: использовать AUTO:limits здесь, пока их не сломали враги

		// TODO: load site auto.p files, all assigned bases from upper dir
		char *site_auto_file="Y:\\parser3\\src\\auto.p";
		VClass *site_auto_class=use(
			site_auto_file, 
			auto_class_name, sys_auto_class, 
			false/*ignore possible error*/);

		// there must be some auto.p
		if(!sys_auto_class && !site_auto_class)
			THROW(0,0,
				0,
				"'auto.p' not found");

		// compiling requested file
		char *test_file="Y:\\parser3\\src\\test.p";
		use(test_file, run_class_name);

		// executing some @main[]
		char *result=execute_MAIN();
		printf("result-----------------\n%sEOF----------------\n", result);
	} 
	CATCH(e) {
		printf("\nERROR: ");
		const String *problem_source=e.problem_source();
		if(problem_source) {
			const Origin& origin=problem_source->origin();
			if(origin.file)
				printf("%s(%d): ",
				origin.file, 1+origin.line);
			printf("'%s' ", 
				problem_source->cstr());
		}
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

VClass *Request::use(char *file, String *name, VClass *base_class, bool fail_on_read_problem) {
	// TODO: обнаружить|решить cyclic dependences
	char *source=file_read(pool(), file, fail_on_read_problem);
	if(!source)
		return 0;

	// compile loaded class
	VClass& vclass=COMPILE(source, name, base_class, file);

	// locate and execute possible @auto[] static method
	execute_static_method(vclass, *auto_method_name, false /*no result needed*/);
	return &vclass;
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
