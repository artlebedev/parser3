/** @file
	Parser: response class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vresponse.h,v 1.12 2001/09/21 08:38:28 parser Exp $
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

#include "pa_vstateless_object.h"
#include "pa_string.h"

// externals

extern Methoded *response_class;

// forwards

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
		// guard charset change
		if(name == *content_type_name)
			if(Hash *hash=value->get_hash())
				if(Value *vcharset=(Value *)hash->get(*charset_name))
					pool().set_charset(vcharset->as_string());		
		
		ffields.put(name, value);
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VResponse(Pool& apool) : VStateless_object(apool, *response_class),
		ffields(apool) {
	}
public:	

	Hash& fields() { return ffields; }

private:

	Hash ffields;

};

#endif
