/** @file
	Parser: @b request class decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VREQUEST_H
#define PA_VREQUEST_H

static const char * const IDENT_VREQUEST_H="$Date: 2004/02/11 15:33:19 $";

// includes

//#include "pa_common.h"
#include "pa_value.h"

// defines

#define REQUEST_CLASS_NAME "request"

// forwards

class Request_info;

/// request class
class VRequest: public Value {

	Request_info& finfo;
	Request_charsets& fcharsets;

public: // Value
	
	override const char* type() const { return "request"; }
	/// VRequest: 0
	override VStateless_class *get_class() { return 0; }

	/// request: fields
	override Value* get_element(const String& name, Value& aself, bool /*looking_up*/);

	/// request: (key)=value
	override bool put_element(const String& aname, Value* avalue, bool replace);

public: // usage

	VRequest(Request_info& ainfo, Request_charsets& acharsets): 
		finfo(ainfo), fcharsets(acharsets) {}

};

#endif
