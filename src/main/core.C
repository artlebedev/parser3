/*
$Id: core.C,v 1.24 2001/02/22 10:43:45 paf Exp $
*/

#include "pa_request.h"
#include "pa_wcontext.h"
#include "pa_common.h"
#include "pa_vclass.h"

#include <stdio.h>

void core() {
	Exception exception;
	Pool pool(exception);
	Request request(pool);
	request.core();
}

void Request::core() {
	TRY {
		String name_RUN(pool()); name_RUN.APPEND_CONST("RUN");
		char *result=execute_MAIN(construct_class(name_RUN, load_and_compile_RUN()));
		printf("-----------------\n%s\n----------------\n", result);
	} 
	CATCH(e) {
		printf("operator error occured: %s\n", e.comment());
		const String *type=e.type();
		if(type) {
			printf("  type: %s", type->cstr());
			const String *code=e.code();
			if(code)
				printf(", code: %s", code->cstr());
			printf("\n");
		}
		const String *problem_source=e.problem_source();
		if(problem_source) {
			const Origin& origin=problem_source->origin();
			printf("  '%s'\n", 
				problem_source->cstr());
			if(origin.file)
				printf(" [%s:%d]",
				origin.file, origin.line);
			printf("\n");
		}
	}
	END_CATCH
}

Array& Request::load_and_compile_RUN() {
	char *file="Y:\\parser3\\src\\test.p";
	char *source=file_read(pool(), file);
	Array& compiled_methods=COMPILE(source, file);
	return compiled_methods;
}

VClass *Request::construct_class(String& name, Array& compiled_methods) {
	// create new 'name' vclass, add it to request's classes
	Array immediate_parents(pool());
	// TODO: immediate_parents=@PARENTS

	VClass *result=new(pool()) VClass(pool(), name, immediate_parents);
	classes().put(name, result);
		
	for(int i=0; i<compiled_methods.size(); i++) {
		// TODO: filter out @PARENTS & ?co?
		Method &method=*static_cast<Method *>(compiled_methods.quick_get(i));
		result->add_method(method.name, method);
	}

	return result;
}

char *Request::execute_MAIN(VClass *class_RUN) {
	// initialize contexts
	root=self=rcontext=class_RUN;
	wcontext=new(pool()) WContext(pool(), class_RUN);

	// locate @main code
	String name_main(pool());
	name_main.APPEND_CONST(MAIN_METHOD_NAME);

	Method *method_main=class_RUN->get_method(name_main);
	if(!method_main)
		THROW(0,0,
			&class_RUN->name(),
			"no '"MAIN_METHOD_NAME"' method in class");

	// execute!	
	execute(method_main->code);

	// return chars
	return wcontext->get_string()->cstr();
}
