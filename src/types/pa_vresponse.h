/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vresponse.h,v 1.2 2001/03/18 17:39:30 paf Exp $
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

#include "pa_vstateless_class.h"
#include "pa_string.h"
#include "_response.h"

class Response;

class VResponse : public VStateless_class {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "response"; }

	// response: ffields
	Hash *get_hash() { return &ffields; }

	// response: CLASS,BASE,method,fields
	Value *get_element(const String& name) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_class::get_element(name))
			return result;
		
		// $field
		return static_cast<Value *>(ffields.get(name));
	}

	// response: (attribute)=value
	void put_element(const String& name, Value *value) { 
		ffields.put(name, value);
	}

public: // usage

	VResponse(Pool& apool) : VStateless_class(apool, response_base_class),
		ffields(apool) {
	}
	
	Hash& fields() { return ffields; }

private:

	Hash ffields;

};

#endif
