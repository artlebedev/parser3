/** @file
	Parser: @b stateless_object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

static const char* IDENT_VSTATELESS_OBJECT_H="$Date: 2002/08/12 14:21:52 $";

#include "pa_vjunction.h"
#include "pa_vstateless_class.h"

/**	the object of some class. 
	
	"of some class" means "with some set of methods".
*/
class VStateless_object : public Value {
public: // Value
	
	/// VStateless_object: class.get_junction
	/*override*/ Junction *get_junction(const String& name, bool looking_down) {
		return get_class()->get_junction(name, looking_down);
	}

	/// VStateless_object: +$method
	Value *get_element(const String& name) {
		// $method=junction(self+class+method)
		if(Junction *junction=get_junction(name, false))
			return NEW VJunction(*junction);

		return 0;
	}

public: // creation

	VStateless_object(Pool& apool) : Value(apool) {}

};

#endif
