/*
  $Id: pa_vobject.h,v 1.12 2001/03/08 17:08:13 paf Exp $
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

#include "pa_valiased.h"
#include "pa_vjunction.h"

class Temp_class_alias;

class VObject : public VAliased {
	friend Temp_class_alias;
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "object"; }

	// clone
	Value *clone() const { return NEW VObject(fclass_real, ffields); };

	// object_instance: (field)=value;(CLASS)=vclass;(method)=method_ref
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
		// $field=ffields.field
		return static_cast<Value *>(ffields.get(name));
	}

	// object_instance: (field)=value
	void put_element(const String& name, Value *value) {
		// speed1:
		//   will not check for '$CLASS(subst)' trick
		//   will hope that user ain't THAT self-hating person
		// speed2:
		//   will not check for '$method_name(subst)' trick
		//   -same-

		ffields.put(name, value);
	}

	// object_class, object_instance: object_class
	VClass *get_class() { return &fclass_real; }

public: // creation

	VObject(VClass& aclass_real) : VAliased(aclass_real.pool(), aclass_real), 
		fclass_real(aclass_real),
		ffields(*new(aclass_real.pool()) Hash(aclass_real.pool())) {
	}

	VObject(VClass& aclass_real, Hash& afields) : VAliased(aclass_real.pool(), aclass_real), 
		fclass_real(aclass_real),
		ffields(afields) {
	}

private:

	VClass& fclass_real;
	Hash& ffields;
};

#endif
