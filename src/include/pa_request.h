/*
  $Id: pa_request.h,v 1.14 2001/02/25 09:14:01 paf Exp $
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
#	define COMPILE_PARAMS char *source, String *default_name, char *file
#	define COMPILE(source, default_name, file) real_compile(source, default_name, file)
#else
#	define COMPILE_PARAMS char *source, String *default_name
#	define COMPILE(source, default_name, file) real_compile(source, default_name)
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

	void use(char *file, String *alias);

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
