/*
  $Id: pa_request.h,v 1.4 2001/01/30 13:07:31 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_exception.h"

class Local_request_exception;

class Request {
	friend Local_request_exception;
public:
	
	Request(Pool& apool) : fpool(apool) {}
	~Request() {}

	Pool& pool() { return fpool; }

	Exception& exception() { return *fexception; }

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
