/** @file
	Parser: request class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vrequest.h,v 1.7 2001/03/19 23:07:24 paf Exp $
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

#include "pa_common.h"
#include "pa_value.h"
#include "pa_string.h"

class Request;

/// request class
class VRequest : public Value {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "request"; }

	// request: fields
	Value *get_element(const String& aname);

public: // usage

	VRequest(Pool& apool, Request& arequest) : Value(apool),
		frequest(arequest) {
	}

private:

	Request& frequest;
};

#endif
