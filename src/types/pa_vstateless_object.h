/** @file
	Parser: @b stateless_object class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

#define IDENT_PA_VSTATELESS_OBJECT_H "$Id: pa_vstateless_object.h,v 1.44 2020/12/15 17:10:43 moko Exp $"

// include

#include "pa_vjunction.h"
#include "pa_vstateless_class.h"

/**	the object of some class. 
	
	"of some class" means "with some set of methods and CLASS_fields".
*/
class VStateless_object: public Value {
public: // Value
	
	/// VStateless_object: class_transparent
	override Value* get_element(const String& aname) {
		return get_class()->get_element(*this, aname);
	}

	/// VStateless_object: class_transparent
	override const VJunction* put_element(const String& aname, Value* avalue) { 
		return get_class()->put_element(*this, aname, avalue);
	}

	/// VStateless_object: class_transparent
	override Value* get_default_getter(Value& aself, const String& aname) { 
		return get_class()->get_default_getter(aself, aname);
	}

	/// VStateless_object: class_transparent
	override Value* get_scalar(Value& aself){
		return get_class()->get_scalar(aself);
	}

};

#endif
