/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vobject.h,v 1.11 2001/09/26 10:32:26 parser Exp $
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

#include "pa_valiased.h"
#include "pa_vjunction.h"
#include "pa_vclass.h"
#include "pa_vstateless_object.h"


/**	parser class instance,
	stores fields VObject::ffields (dynamic, not static, which are stored in class).
*/
class VObject : public VStateless_object {
public: // Value
	
	const char *type() const { return "object"; }

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

	VObject(Pool& apool, VStateless_class& aclass_real) : 
		VStateless_object(apool, aclass_real), 

		ffields(apool) {
	}

private:

	Hash ffields;
};

#endif
