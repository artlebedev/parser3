/*
  $Id: pa_request.h,v 1.2 2001/01/29 21:51:51 paf Exp $
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
	Error request_error;

	// these errors handled on local(operator) level
	Error operator_error;

	Request() {}
	~Request() {}
};

#endif
