/*
  $Id: pa_request.h,v 1.13 2001/02/25 08:50:12 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_hash.h"
#include "pa_wcontext.h"
#include "pa_value.h"
#include "pa_stack.h"
#include "pa_vclass.h"

#define MAIN_METHOD_NAME "main"
#define NAME_RUN "RUN"

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
		fclasses(apool),
		fclasses_array(apool)
		{
	}
	~Request() {}

	// global classes
	Hash& classes() { return fclasses; }
	Array& classes_array() { return fclasses_array; }

	// core request processing
	void core();

private: // core data

	// classes
	Hash fclasses;
	Array fclasses_array;

	// contexts
	Value *self, *root, *rcontext;
	WContext *wcontext;
	
	// execution stack
	Stack stack;

public: // core.C

	void use(char *file, String *name);

private: // core.C

	char *execute_MAIN();

private: // compile.C

	VClass& real_compile(COMPILE_PARAMS);

private: // execute.C

	void execute(const Array& ops);

	Value *get_element();

};

// core func
void core();

#endif
