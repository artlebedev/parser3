/** @file
	Parser: @b request class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

static const char* IDENT_VREQUEST_H="$Date: 2002/08/14 14:18:31 $";

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
	/*override*/ Value *get_element(const String& name, Value *aself, bool /*looking_up*/);

	/// request: (key)=value
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace);

public: // usage

	VRequest(Pool& apool, Request& arequest) : Value(apool),
		frequest(arequest) {
	}

private:

	Request& frequest;
};

#endif
