/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

static const char* IDENT_VOBJECT_H="$Date: 2002/08/12 14:21:52 $";

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

	/// VObject: fclass
	VStateless_class *get_class() { return &fclass; }
	/// VObject: fbase
	/*override*/ Value *base_object() { return fbase; }
	/// VObject : true
	Value *as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }
	/// VObject : true
	bool as_bool() const { return true; }

	/// VObject: child or self or parent method junction
	/*override*/ Junction *get_junction(const String& name, bool looking_down) {
		if(fderived) {
			if(Junction *result=fderived->get_junction(name, true))
				return result;
		}
		if(Method *method=static_cast<Method *>(get_class()->get_method(name)))
			return new(name.pool()) Junction(name.pool(), *this, get_class(), method, 0,0,0,0);
		if(!looking_down && fbase)
			return fbase->get_junction(name, false);
		return 0; 
	}

	/// VObject : (field)=value;(CLASS)=vclass;(method)=method_ref
	Value *get_element(const String& name) {
		// $field=ffields.field
		if(Value *result=static_cast<Value *>(ffields.get(name)))
			return result;

		// $CLASS
		if(name==CLASS_NAME)
			return get_class();

		// $method of last child or upper
		return 
			VStateless_object::get_element(name);
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
	/*override*/ Value *set_derived(Value *aderived) { 
		Value *result=fderived;
		fderived=aderived;
		return fderived;
	}

public: // creation

	VObject(Pool& apool, VStateless_class& aclass) : VStateless_object(apool), 
		fclass(aclass),
		ffields(apool),
		fderived(0),
		fbase(fclass.base_class()?fclass.base_class()->create_new_value(apool):0) {
		if(fbase)
			fbase->set_derived(this);
	}

private:

	VStateless_class& fclass;
	Hash ffields;
	Value *fderived;
	Value *fbase;
};

class Temp_derived {
	Value& fvalue;
	Value *fsaved_derived;
public:
	Temp_derived(Value& avalue, Value *aderived) : 
		fvalue(avalue),
		fsaved_derived(avalue.set_derived(aderived)) {}
	~Temp_derived() { fvalue.set_derived(fsaved_derived); }
};

#endif
