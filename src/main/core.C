/*
$Id: core.C,v 1.41 2001/02/25 16:36:12 paf Exp $
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

void Request::use(char *file, String *alias) {
	char *source=file_read(pool(), file);
	if(!source)
		THROW(0,0,
			0,
			"use: can not read '%s' file", file);

	VClass& vclass=COMPILE(source, alias, file);
	String& vclass_name=vclass.name();
	//TODO: обнаружить, что грузят не объект, а операторы.
	// загрузить операторы
	classes_array()+=&vclass;
	classes().put(vclass_name, &vclass);
	if(alias)
		classes().put(*alias, &vclass);
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
			if(junction) {// it even has junction!
				const Method *method=junction->method;
				if(method) { // and junction is method-junction! call it
					// initialize contexts
					root=rcontext=self=vclass;
					wcontext=NEW WWrapper(pool(), vclass, false /* not constructing */);
					
					// execute!	
					execute(method->code);
					
					// return chars
					return wcontext->get_string()->cstr();
				}
			}
		}
	}
	
	THROW(0,0,
		0, 
		"'"MAIN_METHOD_NAME"' method not found");
	return 0;
}
