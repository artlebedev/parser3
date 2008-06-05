/** @file
	Parser: @b stateless_object class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

static const char * const IDENT_VSTATELESS_OBJECT_H="$Date: 2008/06/05 13:27:18 $";

// include

#include "pa_vjunction.h"
#include "pa_vstateless_class.h"

/**	the object of some class. 
	
	"of some class" means "with some set of methods and CLASS_fields".
*/
class VStateless_object: public Value {
public: // Value
	
	/// VStateless_object: class_transparent
	override Value* get_element(const String& aname, Value& aself, bool alooking_up) {
		return get_class()->get_element(aname, aself, alooking_up);
	}

	/// VStateless_object: class_transparent
	override const VJunction* put_element(Value& aself, const String& aname, Value* avalue, bool areplace) { 
		return get_class()->put_element(aself, aname, avalue, areplace);
	}

	/// VStateless_object: class_transparent
	override Value* get_default_getter(Value& aself, const String& aname) { 
		return get_class()->get_default_getter(aself, aname);
	}

};

#endif
