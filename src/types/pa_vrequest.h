/** @file
	Parser: request class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vrequest.h,v 1.6 2001/03/19 22:56:45 paf Exp $
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_string.h"
#include "_request.h"

class Request;

/// request class
class VRequest : public VStateless_object {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "request"; }

	// request: CLASS,BASE,method,fields
	Value *get_element(const String& aname);

public: // usage

	VRequest(Pool& apool, Request& arequest) : 
		VStateless_object(apool, *request_class),
		
		frequest(arequest) {
	}

private:

	Request& frequest;
};

#endif
