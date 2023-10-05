/** @file
	Parser: @b stateless_object class decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VSTATELESS_OBJECT_H
#define PA_VSTATELESS_OBJECT_H

#define IDENT_PA_VSTATELESS_OBJECT_H "$Id: pa_vstateless_object.h,v 1.47 2023/10/05 01:28:08 moko Exp $"

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
		// $methods (CLASS, CLASS_NAME only if no OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL)
		return get_class()->get_element(*this, aname);
	}

#ifdef FEATURE_GET_ELEMENT4CALL
	/// same as get_element, but with proper error reporting
	override Value* get_element4call(const String& aname) {
		if(Value* result=get_class()->get_element(*this, aname))
			return result;
		return bark("%s method not found", &aname);
	}
#endif

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

class VSimple_stateless_object: public VStateless_object {
public: // Value

	/// VSimple_stateless_object: class_transparent
	override Value* get_element(const String& aname) {
		// $methods (CLASS, CLASS_NAME only if no OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL)
		if(Value* result=get_class()->get_element(*this, aname))
			return result;

		// bad $type.field
		return Value::get_element(aname);
	}
};

#endif
