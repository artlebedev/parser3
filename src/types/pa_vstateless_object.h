/** @file
	Parser: stateless object decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstateless_object.h,v 1.6 2001/03/19 21:47:02 paf Exp $
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

#include "pa_valiased.h"
#include "pa_vjunction.h"
#include "pa_vclass.h"

/**	the object of some class. 
	
	"of some class" means "with some set of methods".
*/
class VStateless_object : public VAliased {
public: // Value
	
	/// VStateless_object: fclass_real
	VStateless_class *get_class() { return &fclass_real; }

	/// VStateless_object: (CLASS)=vclass;(BASE)=base;(method)=method_ref
	Value *get_element(const String& name) {
		// $CLASS=my class
		if(name==CLASS_NAME)
			return fclass_alias;
		// $BASE=my parent
		if(name==BASE_NAME)
			return fclass_alias->base();
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
