/** @file
	Parser: @b request class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

static const char* IDENT_VREQUEST_H="$Id: pa_vrequest.h,v 1.18 2002/08/01 11:26:56 paf Exp $";

#include "pa_common.h"
#include "pa_value.h"

class Request;

#define REQUEST_CLASS_NAME "request"

/// request class
class VRequest : public Value {
public: // Value
	
	const char *type() const { return "request"; }
	/// VRequest: 0
	VStateless_class *get_class() { return 0; }

	/// request: fields
	Value *get_element(const String& name);

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
