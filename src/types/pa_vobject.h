/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

static const char* IDENT_VOBJECT_H="$Date: 2002/08/13 14:35:03 $";

#include "pa_vjunction.h"
#include "pa_vclass.h"
#include "pa_vstateless_object.h"

// defines

#define CLASS_NAME "CLASS"
#define BASE_NAME "BASE"

/**	parser class instance, stores 
	- class VObject::fclass;
	- fields VObject::ffields (dynamic, not static, which are stored in class).
	- links to base/derived instances [VObject::fbase, VObject::fderived]
*/
class VObject: public VStateless_object {
public: // Value
	
	const char *type() const { return fclass.name_cstr(); }
	Value *as(const char *atype) { return fclass.as(atype); }

	/// VObject: fclass
	VStateless_class *get_class() { return &fclass; }
	/// VObject: fbase
	/*override*/ Value *base_object() { return fbase; }
	/// VObject: true, todo: z base table can be 33
	Value *as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }
	/// VObject: true, todo: z base table can be false	
	bool as_bool() const { return true; }

	/// VObject: (field)=value;(CLASS)=vclass;(method)=method_ref
	Value *get_element(const String& aname, Value *aself) {
		// gets element from last_derivate upwards
		if(aself) {
			// $CLASS
			if(aname==CLASS_NAME)
				return get_class();

			// for first call, pass call to last derived VObject
			return get_last_derived()->get_element(aname, 0/*mark this call as 'not first'*/);
		}

		// $method, $CLASS_field
		{
			Temp_base temp_base(*get_class(), 0);
			if(Value *result=VStateless_object::get_element(aname, this))
				return result;
		}

		// $field=ffields.field
		if(Value *result=static_cast<Value *>(ffields.get(aname)))
			return result;

		// up the tree...
		if(fbase)
			if(Value *result=fbase->get_element(aname, fbase))
				return result;

		return 0;
	}

	/// VObject: (field)=value
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace) {
		// replaces element to last_derivate upwards or stores it in self
		// speed1:
		//   will not check for '$CLASS(subst)' trick
		//   will hope that user ain't THAT self-hating person
		// speed2:
		//   will not check for '$method_name(subst)' trick
		//   -same-

		// downwards: same as upwards

		if(fderived && fderived->put_element(aname, avalue, true))
			return true; // replaced in derived

		// upwards: copied from VClass::put_element...

		try {
			if(fbase && fbase->put_element(aname, avalue, true))
				return true; // replaced in base
		} catch(Exception) { 
			/* ignore "can not store to table&co errors for nonexistent elements */ 
			bool error;
			try {
				error=get_element(aname, this)!=0;
			} catch(Exception) { 
				error=false;
			}
			if(error)
				/*re*/throw;
		}

		if(replace)
			return ffields.put_replace(aname, avalue);
		else {
			ffields.put(aname, avalue);
			return false;
		}
	}

	/// VObject: remember derived [the only client] */
	/*override*/ VObject *set_derived(VObject *aderived) { 
		VObject *result=fderived;
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

	Value *get_last_derived() {
		return fderived?fderived->get_last_derived():this;
	}

private:

	VStateless_class& fclass;
	Hash ffields;
	VObject *fderived;
	Value *fbase;
};

class Temp_derived {
	Value& fvalue;
	VObject *fsaved_derived;
public:
	Temp_derived(Value& avalue, VObject *aderived) : 
		fvalue(avalue),
		fsaved_derived(avalue.set_derived(aderived)) {}
	~Temp_derived() { fvalue.set_derived(fsaved_derived); }
};

#endif
