/*
$Id: pa_request.C,v 1.1 2001/02/21 16:12:11 paf Exp $
*/

#include "pa_request.h"
#include "compile.h"
#include "pa_context.h"

void Request::core() {
	Exception exception;
	Local_request_exception subst(*this, exception);
	if(EXCEPTION_TRY(exception())) {
		String name_RUN(pool()); name_RUN.APPEND_CONST("RUN");
		execute_MAIN(construct_class(name_RUN, load_and_compile_RUN())));
	} else {
		Exception& e=exception();
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
}

Array& Request::load_and_compile_RUN() {
	char *file="c:\\temp\\test.p";
	char *source=file_read(pool(), file);
	Array& compiled_methods=COMPILE(request, source, file);
	return compiled_methods;
}

VClass *Request::construct_class(String& name, Array& compiled_methods) {
	// create new 'name' vclass, add it to request's classes
	Array immediate_parents(pool);
	// TODO: immediate_parents=@PARENTS

	VClass *vclass=new(pool) VClass(pool, name, immediate_parents);
	request.classes().put(name, vclass);
		
	for(int i=0; i<compiled_methods.size(); i++) {
		// TODO: filter out @PARENTS & ?co?
		Method &method=*static_cast<Method *>(compiled_methods.quick_get(i));
		vclass->add_method(method.name, method);
	}
}

char *Request::execute_MAIN(Value *class_RUN) {
	// initialize contexts
	root=self=rcontext=class_RUN;
	wcontext=new(pool()) WContext(pool());

	// locate @main code
	String name_main(pool);
	name_main.APPEND_CONST(MAIN_METHOD_NAME);

	Method *method_main=class_RUN->get_method(name_main);
	if(!method_main)
		request.exception().raise(0,0,
		&class_RUN->name(),
		"no 'main' method in class");

	// execute!	
	execute(method_main->code);

	// return chars
	return wcontext.get_string()->cstr();
}
