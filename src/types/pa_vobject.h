/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vobject.h,v 1.3 2001/03/11 12:22:01 paf Exp $
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

#include "pa_valiased.h"
#include "pa_vjunction.h"
#include "pa_vclass.h"
#include "pa_vobject_base.h"

class VObject : public VObject_base {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "object"; }

	// object_instance: (field)=value;(CLASS)=vclass;(method)=method_ref
	Value *get_element(const String& name) {
		// $CLASS,$BASE,$method
		if(Value *result=VObject_base::get_element(name))
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

	VObject(Pool& apool, VClass& aclass_real) : VObject_base(apool, aclass_real), 
		ffields(apool) {
	}

private:

	Hash ffields;
};

#endif
