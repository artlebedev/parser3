/*
  $Id: pa_request.h,v 1.5 2001/02/21 11:10:02 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_hash.h"

class Local_request_exception;

class Request {
	friend Local_request_exception;
public:
	
	Request(Pool& apool) : 
		fpool(apool), 
		fclasses(new(apool) Hash(apool)) {
	}
	~Request() {}

	// IMPORTANT: don't use pool without  Local_request_exception 
	Pool& pool() { return fpool; }

	Exception& exception() { return *fexception; }
	Hash& classes() { return *fclasses; }

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

	// defined classes
	Hash *fclasses;

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
