/** @file
	Parser: response class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vresponse.h,v 1.4 2001/03/26 08:27:28 paf Exp $
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

#include "pa_vstateless_object.h"
#include "pa_string.h"
#include "_response.h"

class Response;

/// response class
class VResponse : public VStateless_object {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "response"; }

	// response: ffields
	Hash *get_hash() { return &ffields; }

	// response: CLASS,BASE,method,fields
	Value *get_element(const String& name) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_object::get_element(name))
			return result;
		
		// $field
		return static_cast<Value *>(ffields.get(name));
	}

	// response: (attribute)=value
	void put_element(const String& name, Value *value) { 
		ffields.put(name, value);
	}

protected: // VAliased

	VStateless_class *get_class_alias() { return 0; }

public: // usage

	VResponse(Pool& apool) : VStateless_object(apool, *response_class),
		ffields(apool) {
	}
	
	Hash& fields() { return ffields; }

private:

	Hash ffields;

};

#endif
