/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vobject.h,v 1.2 2001/03/11 08:16:38 paf Exp $
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

#include "pa_valiased.h"
#include "pa_vjunction.h"
#include "pa_vclass.h"

class Temp_class_alias;

class VObject : public VAliased {
	friend Temp_class_alias;
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "object"; }

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

	VObject(Pool& apool, VClass& aclass_real) : VAliased(apool, aclass_real), 
		fclass_real(aclass_real),
		ffields(apool) {
	}

private:

	VClass& fclass_real;
	Hash ffields;
};

#endif
