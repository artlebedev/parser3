/** @file
	Parser: response class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VRESPONSE_H
#define PA_VRESPONSE_H

static const char* IDENT_VRESPONSE_H="$Date: 2002/10/31 15:01:56 $";

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
	VStateless_class *get_class() { return response_class; }

	/// Response: ffields
	Hash *get_hash(const String * /*source*/) { return &ffields; }

	/// Response: method,fields
	Value *get_element(const String& aname, Value& aself, bool /*looking_up*/);

	/// Response: (attribute)=value
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace);

public: // usage

	VResponse(Pool& apool) : VStateless_object(apool),
		ffields(apool) {
	}
public:	

	Hash& fields() { return ffields; }

private:

	Hash ffields;

};

// helper funcs

/**
	$content-type[text/html] -> 
		content-type: text/html
	$content-type[$value[text/html] charset[windows-1251]] -> 
		content-type: text/html; charset=windows-1251
*/
const String& attributed_meaning_to_string(Value& meaning, String::Untaint_lang lang);

#endif
