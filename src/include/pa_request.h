/*
  $Id: pa_request.h,v 1.3 2001/01/30 11:51:07 paf Exp $
*/

#ifndef PA_REQUEST_H
#define PA_REQUEST_H

#include "pa_pool.h"

class Request {
public:
	
	// all request' objects are allocated in this pool
	// they die with request
	Pool& pool() { return fpool; }
	
	Request(Pool& apool) : fpool(apool) {}
	~Request() {}

protected:

	Pool& fpool;
};

#endif
