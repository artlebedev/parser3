/*
  $Id: pa_vobject.h,v 1.7 2001/02/25 16:36:11 paf Exp $
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
	const char *type() const { return "Object"; }

	// object_instance: (field)=value;(CLASS)=vclass;(method)=method_ref
	Value *get_element(const String& name) {
		// $CLASS=my class
		if(name==CLASS_NAME)
			return class_alias;
		// $BASE=my parent
		if(name==BASE_NAME)
			return class_alias->base();
		// $method=junction(self+class+method)
		if(Junction *junction=class_real.get_junction(*this, name))
			return NEW VJunction(*junction);
		// $field=fields.field
		return static_cast<Value *>(fields.get(name));
	}

	// object_instance: (field)=value
	void put_element(const String& name, Value *value) {
		// speed1:
		//   will not check for '$CLASS(subst)' trick
		//   will hope that user ain't THAT self-hating person
		// speed2:
		//   will not check for '$method_name(subst)' trick
		//   -same-

		fields.put(name, value);
	}

	// object_class, object_instance: object_class
	VClass *get_class() { return &class_real; }

public: // creation

	VObject(Pool& apool, VClass& vclass) : VAliased(apool, vclass), 
		class_real(vclass),
		fields(apool) {
	}

private:

	VClass& class_real;
	Hash fields;
};

#endif
