/*
$Id: pa_request.C,v 1.4 2001/03/10 14:05:36 paf Exp $
*/

#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_common.h"
#include "pa_vclass.h"
#include "classes/_root.h"
#include "classes/_env.h"

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
	String ROOT(pool()); ROOT.APPEND_CONST(ROOT_NAME);
	classes().put(ROOT, &root_class);

	// env class
	initialize_env_class(pool(), env_class);
	String ENV(pool()); ENV.APPEND_CONST(ENV_NAME);
	classes().put(ENV, &env_class);
}

void Request::core() {
	TRY {
		char *file="Y:\\parser3\\src\\test.p";
		String RUN(pool()); RUN.APPEND_CONST(RUN_NAME);
		use(file, &RUN);

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

void Request::use(char *file, String *name) {
	// TODO: обнаружить|решить cyclic dependences
	char *source=file_read(pool(), file);
	if(!source)
		THROW(0,0,
			0,
			"use: can not read '%s' file", file);

	VClass& vclass=COMPILE(source, name, file);

	// locate and execute possible @auto[] static method
	String name_auto(pool());
	name_auto.APPEND_CONST(AUTO_METHOD_NAME);
	execute_static(vclass, name_auto, false);
}

char *Request::execute_MAIN() {
	// locate class with @main & it's code
	String name_main(pool());
	name_main.APPEND_CONST(MAIN_METHOD_NAME);

	// looking for latest known @main
	for(int i=classes_array().size(); --i>=0;) {
		VClass *vclass=static_cast<VClass *>(classes_array().get(i));
		char *result=execute_static(*vclass, name_main, true);
		if(result)
			return result;
	}
	
	THROW(0,0,
		0, 
		"'"MAIN_METHOD_NAME"' method not found");
	return 0;
}
