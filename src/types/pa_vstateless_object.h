/** @file
	Parser: @b stateless_object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

static const char* IDENT_VSTATELESS_OBJECT_H="$Date: 2002/08/13 13:02:42 $";

#include "pa_vjunction.h"
#include "pa_vstateless_class.h"

/**	the object of some class. 
	
	"of some class" means "with some set of methods and CLASS_fields".
*/
class VStateless_object : public Value {
public: // Value
	
	/// VStateless_object: $method, $CLASS_field 
	Value *get_element(const String& aname, Value *aself) {
		return get_class()->get_element(aname, aself);
	}

public: // creation

	VStateless_object(Pool& apool) : Value(apool) {}

};

#endif
