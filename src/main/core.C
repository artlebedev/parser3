/*
$Id: core.C,v 1.35 2001/02/24 15:26:03 paf Exp $
*/

#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_common.h"
#include "pa_vclass.h"

#include <stdio.h>

void core() {
	Pool pool;
	Request request(pool);
	request.core();
}

void Request::core() {
	TRY {
		char *file="Y:\\parser3\\src\\test.p";
		String RUN(pool()); RUN.APPEND_CONST(NAME_RUN);
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
	char *source=file_read(pool(), file);
	VClass& vclass=COMPILE(source, file);
	if(name) // they forced some name?
		vclass.set_name(*name);
	name=vclass.name();
	if(!name)
		return; //TODO: add operators 
	classes_array()+=&vclass;
	classes().put(*name, &vclass);
}

char *Request::execute_MAIN() {
	// locate class with @main & it's code
	String name_main(pool());
	name_main.APPEND_CONST(MAIN_METHOD_NAME);

	// looking for latest known @main
	for(int i=classes_array().size(); --i>=0;) {
		VClass *vclass=static_cast<VClass *>(classes_array().get(i));
		Value *main=vclass->get_element(name_main);
		if(main) { // found some 'main' element
			Junction *junction=main->get_junction();
			const Method *method=junction->method;
			if(method) { // that element is a method, call it
				// initialize contexts
				self=root=rcontext=vclass;
				wcontext=NEW WWrapper(pool(), vclass);

				// execute!	
				execute(method->code);
				
				// return chars
				return wcontext->get_string()->cstr();
			}
		}
	}
	
	THROW(0,0,
		0, 
		"'"MAIN_METHOD_NAME"' method not found");
	return 0;
}
