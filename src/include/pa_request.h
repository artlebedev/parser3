/*
  $Id: pa_request.h,v 1.22 2001/03/10 11:03:47 paf Exp $
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
#define RUN_NAME "RUN"
#define ROOT_NAME "ROOT"

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS char *source, String *name, char *file
#	define COMPILE(source, name, file) real_compile(source, name, file)
#else
#	define COMPILE_PARAMS char *source, String *name
#	define COMPILE(source, name, file) real_compile(source, name)
#endif

class Local_request_exception;

class Request : public Pooled {
public:
	
	Request(Pool& apool);
	~Request() {}

	// global classes
	Hash& classes() { return fclasses; }
	Array& classes_array() { return fclasses_array; }

	// core request processing
	void core();

	void use(char *file, String *alias); // core.C
	Value& autocalc(Value& value, bool make_string=true); // execute.C

	void write(String& astring) {
		wcontext->write(astring, lang); // append string, assigning untaint language
	}

public:
	
	// default base
	VClass root_class;

	// contexts
	Value *self, *root, *rcontext;
	WContext *wcontext;

private: // core data

	// classes
	Hash fclasses;
	Array fclasses_array;

	// execution stack
	Stack stack;

private: // core.C

	char *execute_MAIN();

private: // compile.C

	VClass& real_compile(COMPILE_PARAMS);

private: // execute.C

	void execute(const Array& ops);

	Value *get_element();

private: // lang&raw 
	
	String::Untaint_lang lang;

private: // lang&raw manipulation



};

// core func
void core();

#endif
