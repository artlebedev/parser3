/*
$Id: core.C,v 1.46 2001/03/09 04:47:29 paf Exp $
*/

#include "pa_request.h"
#include "pa_wwrapper.h"
#include "pa_common.h"
#include "pa_vclass.h"

#include <stdio.h>
#include "classes/_string.h"

void core() {
	Pool pool;
	initialize_string_class(pool);
	Request request(pool);
	request.core();
}

Request::Request(Pool& apool) : Pooled(apool),
	stack(apool),
	root_class(apool),
	fclasses(apool),
	fclasses_array(apool)
{
	// construct_root_class
	void construct_root_class(Request& request); // classes/root
	construct_root_class(*this);

	// TODO: construct other classes, 
	// для встроенных какая-то табличка
	// для внешних - конфиг с @CLASSES файлы с классами/ворохами операторов

	// adding root superclass, 
	//   parent of all classes, 
	//   operators holder
	String ROOT(pool()); ROOT.APPEND_CONST(ROOT_NAME);
	classes().put(ROOT, &root_class);
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

	COMPILE(source, name, file);
	// TODO: запустить @STATIC[], если есть

//	if(alias)
		//classes().put(*alias, &vclass);
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
					execute(*method->parser_code);
					
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
