/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

static const char* IDENT_VOBJECT_H="$Date: 2002/08/12 11:22:55 $";

#include "pa_vjunction.h"
#include "pa_vclass.h"
#include "pa_vstateless_object.h"

// defines

#define CLASS_NAME "CLASS"
#define BASE_NAME "BASE"

/**	parser class instance,
	stores class VObject::fclass;
	stores fields VObject::ffields (dynamic, not static, which are stored in class).
*/
class VObject: public VStateless_object {
public: // Value
	
	const char *type() const { return fclass.name_cstr(); }
	Value *as(const char *atype) { return fclass.as(atype); }

	/// VObject: class of derived  or  fclass
	VStateless_class *get_class() { return fderived?fderived->get_class():&fclass; }
	/// VObject : true
	Value *as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }
	/// VObject : true
	bool as_bool() const { return true; }

	/// VObject : (field)=value;(CLASS)=vclass;(method)=method_ref
	Value *get_element(const String& name) {
		// $field=ffields.field
		if(Value *result=static_cast<Value *>(ffields.get(name)))
			return result;

		// $CLASS
		if(name==CLASS_NAME)
			return &fclass;

		// $method of last child or upper
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

	/// VObject: remember derived [the only client] */
	/*override*/ void set_derived(Value& aderived) { 
		fderived=&aderived;
	}

public: // creation

	VObject(Pool& apool, VStateless_class& aclass) : VStateless_object(apool), 
		fclass(aclass),
		ffields(apool),
		fderived(0),
		fbase(fclass.base()?fclass.base()->create_new_value(apool):0) {
		if(fbase)
			fbase->set_derived(*this);
	}

private:

	VStateless_class& fclass;
	Hash ffields;
	Value *fderived;
	Value *fbase;
};

#endif
