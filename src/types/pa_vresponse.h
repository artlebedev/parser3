/** @file
	Parser: response class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vresponse.h,v 1.10 2001/05/07 14:00:54 paf Exp $
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

#include "pa_vstateless_object.h"
#include "pa_string.h"

extern Methoded *response_class;

class Response;

/// value of type 'response'
class VResponse : public VStateless_object {
public: // Value
	
	const char *type() const { return "response"; }

	/// Response: ffields
	Hash *get_hash() { return &ffields; }

	/// Response: method,fields
	Value *get_element(const String& name) {
		// $method
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
