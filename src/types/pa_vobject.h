/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vobject.h,v 1.4 2001/03/11 21:23:59 paf Exp $
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

#include "pa_valiased.h"
#include "pa_vjunction.h"
#include "pa_vclass.h"
#include "pa_vstateless_object.h"

class VObject : public VStateless_object {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "object"; }

	// object_instance: (field)=value;(CLASS)=vclass;(method)=method_ref
	Value *get_element(const String& name) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_object::get_element(name))
			return result;
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

public: // creation

	VObject(Pool& apool, VClass& aclass_real) : VStateless_object(apool, aclass_real), 
		ffields(apool) {
	}

private:

	Hash ffields;
};

#endif
