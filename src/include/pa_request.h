/*
  $Id: pa_request.h,v 1.6 2001/02/21 16:11:49 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_hash.h"
#include "pa_context.h"
#include "pa_value.h"

class Local_request_exception;

class Request {
	friend Local_request_exception;
public:
	
	Request(Pool& apool, 
		Value *aroot, Value *aself, Value *arcontext, WContext *awcontext) : 
		fpool(apool), 
		root(aroot), self(aself), rcontext(arcontext), wcontext(awcontext),
		fclasses(apool)
		{
	}
	~Request() {}

	// IMPORTANT: don't use pool without  Local_request_exception 
	Pool& pool() { return fpool; }

	Exception& exception() { return *fexception; }
	Hash& classes() { return fclasses; }

	// core request processing
	void core();

protected: // core

	// classes
	Hash fclasses;

	// contexts
	Value *root, *self, *rcontext;
	WContext *wcontext;
	
	Array& load_and_compile_RUN();
	VClass *construct_class(String& name, Array& compiled_methods);
	char *execute_MAIN(Value *class_RUN);

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


#endif
