/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

static const char* IDENT_VOBJECT_H="$Date: 2002/08/09 14:18:40 $";

#include "pa_vjunction.h"
#include "pa_vclass.h"
#include "pa_vstateless_object.h"

/**	parser class instance,
	stores class VObject::fclass_real;
	stores fields VObject::ffields (dynamic, not static, which are stored in class).
*/
class VObject : public VStateless_object {
public: // Value
	
	const char *type() const { return fclass_real.name_cstr(); }
	Value *as(const char *atype) { return fclass_real.as(atype); }

	/// VObject: fclass_real
	VStateless_class *get_class() { return &fclass_real; }
	/// VObject : true
	Value *as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }
	/// VObject : true
	bool as_bool() const { return true; }

	/// VObject : (field)=value;(CLASS)=vclass;(method)=method_ref
	Value *get_element(const String& name) {
		// $field=ffields.field
		if(Value *result=static_cast<Value *>(ffields.get(name)))
			return result;

		// $CLASS,$method
		return VStateless_object::get_element(name);
	}

	/// VObject : (field)=value
	void put_element(const String& name, Value *value) {
		// speed1:
		//   will not check for '$CLASS(subst)' trick
		//   will hope that user ain't THAT self-hating person
		// speed2:
		//   will not check for '$method_name(subst)' trick
		//   -same-

		ffields.put(name, value);
	}

public: // creation

	VObject(Pool& apool, VStateless_class& aclass_real) : VStateless_object(apool), 
		fclass_real(aclass_real),
		ffields(apool) {
	}

private:

	VStateless_class& fclass_real;
	Hash ffields;
};

#endif
