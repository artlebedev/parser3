/*
  $Id: pa_vobject.h,v 1.5 2001/02/25 13:23:01 paf Exp $
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

#include "pa_value.h"
#include "pa_vjunction.h"

class VObject : public Value {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "Object"; }

	// object_instance: (field)=value;(CLASS)=vclass;(method)=method_ref
	Value *get_element(const String& name) {
		// $CLASS=my class
		if(name==CLASS_NAME)
			return &vclass;

		// $method=junction(this+method)
		if(Method *method=static_cast<Method *>(vclass.methods().get(name))) {
			Junction& j=*NEW Junction(pool(), 
				*this,
				method,0,0,0,0);

			return NEW VJunction(j);
		}

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
	VClass *get_class() { return &vclass; }

public: // creation

	VObject(Pool& apool, VClass& vclass) : 
		Value(apool), 
		vclass(vclass),
		fields(apool) {
	}

private:

	VClass& vclass;
	Hash fields;
};

#endif
