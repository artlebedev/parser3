/** @file
	Parser: @b request class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vrequest.h,v 1.11 2001/09/26 10:32:26 parser Exp $
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

#include "pa_common.h"
#include "pa_value.h"
#include "pa_string.h"

class Request;

#define REQUEST_CLASS_NAME "request"

/// request class
class VRequest : public Value {
public: // Value
	
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
