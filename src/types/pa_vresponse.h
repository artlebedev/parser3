/** @file
	Parser: response class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vresponse.h,v 1.21 2002/02/08 08:30:20 paf Exp $
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

#include "pa_vstateless_object.h"
#include "pa_string.h"
#include "classes.h"

// forwards

class Response;

// externals

extern Methoded *response_class;

/// value of type 'response'
class VResponse : public VStateless_object {
public: // Value
	
	const char *type() const { return "response"; }

	/// Response: ffields
	Hash *get_hash(const String * /*source*/) { return &ffields; }

	/// Response: method,fields
	Value *get_element(const String& name);

	/// Response: (attribute)=value
	void put_element(const String& name, Value *value);

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
