/** @file
	Parser: @b request class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vrequest.h,v 1.14 2001/12/15 21:28:22 paf Exp $
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

#include "pa_common.h"
#include "pa_value.h"

class Request;

#define REQUEST_CLASS_NAME "request"

/// request class
class VRequest : public Value {
public: // Value
	
	virtual const char *type() const { return "request"; }

	/// request: fields
	virtual Value *get_element(const String& name);

	/// request: (key)=value
	void put_element(const String& name, Value *value);

public: // usage

	VRequest(Pool& apool, Request& arequest) : Value(apool),
		frequest(arequest) {
	}

private:

	Request& frequest;
};

#endif
