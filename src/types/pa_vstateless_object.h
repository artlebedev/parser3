/** @file
	Parser: @b stateless_object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vstateless_object.h,v 1.14 2002/02/08 07:27:54 paf Exp $
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

#include "pa_valiased.h"
#include "pa_vjunction.h"
#include "pa_vstateless_class.h"

/**	the object of some class. 
	
	"of some class" means "with some set of methods".
*/
class VStateless_object : public VAliased {
public: // Value
	
	/// VStateless_object: fclass_real
	VStateless_class *get_class() { return &fclass_real; }

	/// VStateless_object: +$method
	Value *get_element(const String& name) {
		// $CLASS
		if(Value *result=VAliased::get_element(name))
			return result;

		// $method=junction(self+class+method)
		if(Junction *junction=fclass_real.get_junction(*this, name))
			return NEW VJunction(*junction);

		return 0;
	}

public: // creation

	VStateless_object(Pool& apool, VStateless_class& aclass_real) : 
		VAliased(apool, aclass_real), 

		fclass_real(aclass_real) {
	}

private:

	VStateless_class& fclass_real;
};

#endif
