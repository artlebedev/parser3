/*
  $Id: pa_request.h,v 1.1 2001/01/29 20:46:22 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"
#include "pa_error.h"

class Request {
public:
	
	// all request' objects are allocated in this pool
	// they die with request
	Pool pool;
	
	// these errors handled on global(request) level
	Error fatal_error;

	// these errors handled on local(operator) level
	Error error;

	Request() {}
	~Request() {}
};

#endif
