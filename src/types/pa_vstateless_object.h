/** @file
	Parser: @b stateless_object class decl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

static const char* IDENT_VSTATELESS_OBJECT_H="$Date: 2003/01/21 15:51:21 $";

#include "pa_vjunction.h"
#include "pa_vstateless_class.h"

/**	the object of some class. 
	
	"of some class" means "with some set of methods and CLASS_fields".
*/
class VStateless_object : public Value {
public: // Value
	
	/// VStateless_object: class_transparent
	Value *get_element(const String& aname, Value& aself, bool looking_up) {
		return get_class()->get_element(aname, aself, looking_up);
	}

	/// VStateless_object: class_transparent
	bool put_element(const String& aname, Value *avalue, bool areplace) { 
		return get_class()->put_element(aname, avalue, areplace);
	}

public: // creation

	VStateless_object(Pool& apool) : Value(apool) {}

};

#endif
