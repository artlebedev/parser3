/** @file
	Parser: response class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vresponse.h,v 1.7 2001/04/02 16:00:17 paf Exp $
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

#include "pa_vstateless_object.h"
#include "pa_string.h"
#include "_response.h"

class Response;

/// value of type 'response'
class VResponse : public VStateless_object {
public: // Value
	
	/// all: for error reporting after fail(), etc
	const char *type() const { return "response"; }

	/// Response: ffields
	Hash *get_hash() { return &ffields; }

	/// Response: CLASS,BASE,method,fields
	Value *get_element(const String& name) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_object::get_element(name))
			return result;
		
		// $field
		return static_cast<Value *>(ffields.get(name));
	}

	/// Response: (attribute)=value
	void put_element(const String& name, Value *value) { 
		ffields.put(name, value);
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VResponse(Pool& apool) : VStateless_object(apool, *response_class),
		ffields(apool) {
	}
	
	Hash& fields() { return ffields; }

private:

	Hash ffields;

};

#endif
