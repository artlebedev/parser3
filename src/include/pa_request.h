/*
  $Id: pa_request.h,v 1.10 2001/02/22 15:21:22 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_hash.h"
#include "pa_wcontext.h"
#include "pa_value.h"
#include "pa_stack.h"

#define MAIN_METHOD_NAME "main"
#define SELF_NAME "self"

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS char *source, char *file
#	define COMPILE(source, file) real_compile(source, file)
#else
#	define COMPILE_PARAMS char *source
#	define COMPILE(source, file) real_compile(source)
#endif

class Local_request_exception;

class Request : public Pooled {
public:
	
	Request(Pool& apool) : Pooled(apool),
		stack(apool),
		fclasses(apool)
		{
	}
	~Request() {}

	// global classes
	Hash& classes() { return fclasses; }

	// core request processing
	void core();

private: // core data

	// classes
	Hash fclasses;

	// contexts
	Value *root, *self, *rcontext;
	WContext *wcontext;
	
	// execution stack
	Stack stack;

private: // core.C

	Array& load_and_compile_RUN();
	VClass *construct_class(String& name, Array& compiled_methods);
	char *execute_MAIN(VClass *class_RUN);

private: // compile.C

	Array& real_compile(COMPILE_PARAMS);

private: // execute.C

	void execute(Array& ops);

	Value *get_element();

};

// core func
void core();

#endif
