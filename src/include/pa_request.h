/*
  $Id: pa_request.h,v 1.7 2001/02/21 17:36:30 paf Exp $
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

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS char *source, char *file
#	define COMPILE(source, file) real_compile(source, file)
#else
#	define COMPILE_PARAMS char *source
#	define COMPILE(source, file) real_compile(source)
#endif

class Local_request_exception;

class Request : public Pooled {
	friend Local_request_exception;
public:
	
	Request(Pool& apool) : Pooled(apool),
		fpool(apool), 
		stack(apool),
		fclasses(apool)
		{
	}
	~Request() {}

	// IMPORTANT: don't use without  Local_request_exception 
	Exception& exception() { return *fexception; }

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

protected:

	// all request' objects are allocated in this pool
	Pool& fpool;

	// exception replacement mechanism is protected from direct usage
	// Local_request_exception object enforces paired set/restore
	Exception *set_exception(Exception *e){
		Exception *r=fexception;
		fexception=e;
		return r;
	}
	void restore_exception(Exception *e) {
		fexception=e;
	}

private:

	// current request's exception object
	Exception *fexception;

};

class Local_request_exception {
	Request request;
	Exception *saved_exception;
public:
	Local_request_exception(Request& arequest, Exception& exception) : 
		request(arequest),
		saved_exception(arequest.set_exception(&exception)) {}
	~Local_request_exception() { request.restore_exception(saved_exception); }
};

// core func
void core();

#endif
