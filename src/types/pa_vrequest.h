/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vrequest.h,v 1.5 2001/03/19 20:07:39 paf Exp $
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

#include "pa_common.h"
#include "pa_vstateless_class.h"
#include "pa_string.h"
#include "_request.h"

class Request;

class VRequest : public VStateless_class {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "request"; }

	// request: CLASS,BASE,method,fields
	Value *get_element(const String& aname);

public: // usage

	VRequest(Pool& apool, Request& arequest) : 
		VStateless_class(apool, request_base_class),
		
		frequest(arequest) {
	}

private:

	Request& frequest;
};

#endif
