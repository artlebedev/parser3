/** @file
	Parser: @b stateless_object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

static const char* IDENT_VSTATELESS_OBJECT_H="$Date: 2002/08/01 11:41:25 $";

#include "pa_valiased.h"
#include "pa_vjunction.h"
#include "pa_vstateless_class.h"

/**	the object of some class. 
	
	"of some class" means "with some set of methods".
*/
class VStateless_object : public VAliased {
public: // Value
	
	/// VStateless_object: +$method
	Value *get_element(const String& name) {
		// $CLASS
		if(Value *result=VAliased::get_element(name))
			return result;

		// $method=junction(self+class+method)
		if(Junction *junction=get_class()->get_junction(*this, name))
			return NEW VJunction(*junction);

		return 0;
	}

public: // creation

	VStateless_object(Pool& apool) : VAliased(apool) {}

};

#endif
