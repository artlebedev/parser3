/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstateless_object.h,v 1.1 2001/03/11 21:23:59 paf Exp $
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

#include "pa_valiased.h"
#include "pa_vjunction.h"
#include "pa_vclass.h"

class VStateless_object : public VAliased {
public: // Value
	
	// object_base: (CLASS)=vclass;(BASE)=base;(method)=method_ref
	Value *get_element(const String& name) {
		// $CLASS=my class
		if(name==CLASS_NAME)
			return fclass_alias;
		// $BASE=my parent
		if(name==BASE_NAME)
			return fclass_alias->base();
		// $method=junction(self+class+method)
		if(Junction *junction=fclass_real.get_junction(*this, name))
			return NEW VJunction(*junction);;
		return 0;
	}

	// object_class, object_instance: object_class
	VClass *get_class() { return &fclass_real; }

public: // creation

	VStateless_object(Pool& apool, VClass& aclass_real) : VAliased(apool, aclass_real), 
		fclass_real(aclass_real) {
	}

private:

	VClass& fclass_real;
};

#endif
